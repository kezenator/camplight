/**
 * @file
 *
 * Implementation file for the ledsigns::buttonbox::ButtonBoxPattern class.
 */

#include <ledsigns/buttonbox/ButtonBoxPattern.h>
#include <bbox/enc/FromXml.h>
#include <bbox/enc/ToDebugString.h>
#include <bbox/enc/ToXml.h>
#include <bbox/enc/MsgPtr.h>

#include <boost/algorithm/string.hpp>

namespace ledsigns
{
    namespace buttonbox
    {

        ButtonBoxPattern::ButtonBoxPattern(bbox::rt::Service &parent, leds::GpioClient &gpio_client, const common::RenderState & /*render*/)
            : m_gpio_client(gpio_client)
            , m_web_socket_client(
                "button-box-web-socket",
                parent,
                std::bind(&ButtonBoxPattern::HandleWebSocketState, this, std::placeholders::_1),
                std::bind(&ButtonBoxPattern::HandleWebSocketReceive, this, std::placeholders::_1))
            , m_dispatcher()
            , m_socket_error(std::errc::not_connected)
            , m_button_colors()
            , m_button_states()
        {
            m_button_colors.button_colors.resize(6);
            m_button_states.button_states.resize(6);

            m_dispatcher.Register(&ButtonBoxPattern::HandleRetransmitRequired, this);
            m_dispatcher.Register(&ButtonBoxPattern::HandleButtonColors, this);

            m_web_socket_client.Open("192.168.0.20", "/ws/buttons", "12.09.2018.buttons.miami-nights.kezenator.com");
        }

        ButtonBoxPattern::~ButtonBoxPattern()
        {
        }

        std::string ButtonBoxPattern::GetName() const
        {
            return "Button Box";
        }

        void ButtonBoxPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Socket Error: %s\n", m_socket_error.ToString());
            out.Format("Button Colours: %s\n", bbox::enc::ToDebugString(m_button_colors));
            out.Format("Button States: %s\n", bbox::enc::ToDebugString(m_button_states));
        }

        std::vector<leds::Color> ButtonBoxPattern::Render(const common::RenderState &render)
        {
            UpdateButtonStates();

            if (m_socket_error)
            {
                CreateDisconnectedPattern(render);
            }

            std::vector<leds::Color> result(render.layout.num_leds);

            auto fill = [&](size_t index, size_t length, const std::string &color_str)
            {
                leds::Color color = ConvertColour(color_str);

                for (size_t i = 0; i < length; ++i)
                {
                    result[index + i] = color;
                }
            };

            // 6 main buttons, 4 each, 1 between - in reverse order
            fill(0,  4, m_button_colors.button_colors[5]);
            fill(5,  4, m_button_colors.button_colors[4]);
            fill(10, 4, m_button_colors.button_colors[3]);
            fill(15, 4, m_button_colors.button_colors[2]);
            fill(20, 4, m_button_colors.button_colors[1]);
            fill(25, 4, m_button_colors.button_colors[0]);

            // 6 main buttons, 4 each, 1 between
            fill(29, 4, m_button_colors.play_color);
            fill(35, 4, m_button_colors.back_color);

            // 2x18 bottom LEDs
            fill(39, 36, m_button_colors.wash_color);

            return result;
        }

        void ButtonBoxPattern::HandleWebSocketState(const bbox::Error &error)
        {
            bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Testing);
            if (out)
            {
                out.Format("WebSocket: State: %s\n", error.ToString());
            }

            m_socket_error = error;
        }

        void ButtonBoxPattern::HandleWebSocketReceive(const std::string &msg)
        {
            bbox::enc::FromXml from_xml(msg);
            from_xml.StartNamedItem("message");
            bbox::enc::MsgAnyPtr msg_ptr = from_xml.DecodeMsgAnyPtr();
            from_xml.CompleteNamedItem();
            from_xml.EnsureAllDecoded();

            if (!msg_ptr)
                from_xml.SetError("Decoded null message");

            if (!from_xml.HasError())
            {
                //bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Testing);
                //if (out)
                //{
                //  out.Format("WebSocket: Rx: %s\n", bbox::enc::ToDebugString(msg_ptr));
                //}

                if (!m_dispatcher.Dispatch(msg_ptr))
                    from_xml.SetError(bbox::Format("No handler or handler error for message of type %s", msg_ptr.GetType().GetName()));
            }

            if (from_xml.HasError())
            {
                bbox::DebugOutput err(BBOX_FUNC, bbox::DebugOutput::Error);
                if (err)
                {
                    err.Format("WebSocket: Decode error: %s\n", from_xml.GetErrorString());
                }

                m_web_socket_client.CloseConnection();
            }
        }

        bool ButtonBoxPattern::HandleRetransmitRequired(const mn::msgs::RetransmitRequired & /*msg*/)
        {
            mn::msgs::ButtonStates to_send{ m_button_states };
            Send(new_message<mn::msgs::ButtonStates>(to_send));
            return true;
        }

        bool ButtonBoxPattern::HandleButtonColors(const mn::msgs::ButtonColors &colors)
        {
            // Save the colours for the next render
            m_button_colors = colors;

            return true;
        }

        void ButtonBoxPattern::UpdateButtonStates()
        {
            bool changed = false;

            auto update = [&](size_t input, const char *name, bool saved_state)
            {
                bool new_state = !m_gpio_client.GetInput(input);
                if (new_state != saved_state)
                {
                    bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Testing);
                    if (out)
                    {
                        out.Format("GPIO: Input %d \"%s\" changed to %s\n", input, name, new_state);
                    }

                    changed = true;
                }
                return new_state;
            };

            m_button_states.button_states[0] = update(19, "1", m_button_states.button_states[0]);
            m_button_states.button_states[1] = update(16, "2", m_button_states.button_states[1]);
            m_button_states.button_states[2] = update(20, "3", m_button_states.button_states[2]);
            m_button_states.button_states[3] = update(12, "4", m_button_states.button_states[3]);
            m_button_states.button_states[4] = update(26, "5", m_button_states.button_states[4]);
            m_button_states.button_states[5] = update(6, "6", m_button_states.button_states[5]);
            m_button_states.back_state = update(21, "B", m_button_states.back_state);
            m_button_states.play_state = update(13, "P", m_button_states.play_state);

            if (changed)
            {
                mn::msgs::ButtonStates to_send{ m_button_states };
                Send(new_message<mn::msgs::ButtonStates>(to_send));
            }
        }

        void ButtonBoxPattern::CreateDisconnectedPattern(const common::RenderState &render)
        {
            auto hue_offset_to_color = [&](size_t offset)
            {
                size_t hue = ((render.time_ms / 1000) + offset) % 360;
                return bbox::Format("hsl(%d,100%,50%)", hue);
            };

            m_button_colors.button_colors[0] = hue_offset_to_color(0);
            m_button_colors.button_colors[1] = hue_offset_to_color(30);
            m_button_colors.button_colors[2] = hue_offset_to_color(60);
            m_button_colors.button_colors[3] = hue_offset_to_color(120);
            m_button_colors.button_colors[4] = hue_offset_to_color(180);
            m_button_colors.button_colors[5] = hue_offset_to_color(240);

            m_button_colors.wash_color = hue_offset_to_color(300);

            m_button_colors.play_color = hue_offset_to_color(45);
            m_button_colors.back_color = hue_offset_to_color(150);
        }

        void ButtonBoxPattern::Send(const bbox::enc::MsgAnyPtr &msg)
        {
            if (m_web_socket_client.IsOpen())
            {
                m_web_socket_client.Send(bbox::enc::ToXml::ConvertToString("message", msg));
            }
        }

        leds::Color ButtonBoxPattern::ConvertColour(const std::string &css_color)
        {
            if (css_color == "black")
            {
                return leds::Color(0, 0, 0);
            }
            else if (css_color == "white")
            {
                return leds::Color(255, 255, 255);
            }
            else if ((css_color.size() > 6)
                && (css_color.substr(0, 4) == "hsl(")
                && (css_color.substr(css_color.size() - 2) == "%)"))
            {
                std::string num_string = css_color.substr(4, css_color.size() - 6);
                std::vector<std::string> num_strings;
                boost::algorithm::split(num_strings, num_string, boost::is_any_of(","));

                if ((num_strings.size() == 3)
                    && (num_strings[1].size() > 1)
                    && (num_strings[1][num_strings[1].size() - 1] == '%'))
                {
                    double h = std::atof(num_strings[0].c_str());
                    double s = std::atof(num_strings[1].substr(0, num_strings[1].size() - 1).c_str());
                    double l = std::atof(num_strings[2].c_str());

                    // CSS is in 360 degress, 100%, 100%
                    // convert s and l to 0..1

                    s = s / 100.0;
                    l = l / 100.0;

                    // Convert to RGB

                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;

                    auto HueToRGB = [](double v1, double v2, double vH)
                    {
                        if (vH < 0)
                            vH += 1;

                        if (vH > 1)
                            vH -= 1;

                        if ((6 * vH) < 1)
                            return (v1 + (v2 - v1) * 6 * vH);

                        if ((2 * vH) < 1)
                            return v2;

                        if ((3 * vH) < 2)
                            return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

                        return v1;
                    };

                    if (s == 0)
                    {
                        r = g = b = (unsigned char)(l * 255);
                    }
                    else
                    {
                        double v1, v2;
                        double hue = (double)h / 360;

                        v2 = (l < 0.5) ? (l * (1 + s)) : ((l + s) - (l * s));
                        v1 = 2 * l - v2;

                        r = (unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
                        g = (unsigned char)(255 * HueToRGB(v1, v2, hue));
                        b = (unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
                    }

                    return leds::Color(r, g, b);
                }
            }
            else if ((css_color.size() == 7)
                && (css_color[0] == '#'))
            {
                auto from_bcd = [](char ch)
                {
                    if ((ch >= '0') && (ch <= '9'))
                        return ch - '0';
                    else if ((ch >= 'A') && (ch <= 'F'))
                        return ch - 'A' + 10;
                    else if ((ch >= 'a') && (ch <= 'f'))
                        return ch - 'a' + 10;
                    else
                        return 0;
                };

                auto from_hex = [&](char c1, char c2)
                {
                    return uint8_t((from_bcd(c1) << 4) + from_bcd(c2));
                };

                return leds::Color(
                    from_hex(css_color[1], css_color[2]),
                    from_hex(css_color[3], css_color[4]),
                    from_hex(css_color[5], css_color[6]));
            }

            // Unknown color
            return leds::Color();
        }


    } // namespace ledsigns::buttonbox
} // namespace ledsigns
