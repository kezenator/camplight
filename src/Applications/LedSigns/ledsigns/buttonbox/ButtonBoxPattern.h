/**
* @file
*
* Header file for the ledsigns::buttonbox::ButtonBoxPattern class.
*/

#ifndef __LEDSIGNS__BUTTONBOX__BUTTON_BOX_PATTERN_H__
#define __LEDSIGNS__BUTTONBOX__BUTTON_BOX_PATTERN_H__

#include <ledsigns/common/Pattern.h>
#include <leds/GpioClient.h>

#include <bbox/http/client/WebSocketClient.h>

#include <bbox/enc/Dispatcher.h>

#include <mn/msgs/RetransmitRequired.h>
#include <mn/msgs/ButtonColors.h>
#include <mn/msgs/ButtonStates.h>

namespace ledsigns
{
    namespace buttonbox
    {

        /**
         * The main pattern for the gay sign.
         */
        class ButtonBoxPattern : public common::Pattern
        {
        public:

            ButtonBoxPattern(bbox::rt::Service &parent, leds::GpioClient &gpio_client, const common::RenderState &render);
            ~ButtonBoxPattern();

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:

            void HandleWebSocketState(const bbox::Error &error);
            void HandleWebSocketReceive(const std::string &msg);

            bool HandleRetransmitRequired(const mn::msgs::RetransmitRequired &msg);
            bool HandleButtonColors(const mn::msgs::ButtonColors &colors);

            void UpdateButtonStates();
            void CreateDisconnectedPattern(const common::RenderState &render);

            void Send(const bbox::enc::MsgAnyPtr &msg);

            leds::Color ConvertColour(const std::string &css_color);

            leds::GpioClient &m_gpio_client;
            bbox::http::client::WebSocketClient m_web_socket_client;
            bbox::enc::Dispatcher m_dispatcher;
            bbox::Error m_socket_error;
            mn::msgs::ButtonColors m_button_colors;
            mn::msgs::ButtonStates m_button_states;
        };

    } // namespace ledsigns::buttonbox
} // namespace ledsigns

#endif // __LEDSIGNS__BUTTONBOX__BUTTON_BOX_PATTERN_H__
