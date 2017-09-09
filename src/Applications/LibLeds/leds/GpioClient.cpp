/**
 * @file
 *
 * Implementation file for the leds::GpioClient class.
 */

#include <leds/GpioClient.h>
#include <bbox/Assert.h>

namespace leds
{

    struct GpioClient::Pimpl
    {
        std::map<size_t, bool> outputs;
        std::set<size_t> inputs;
    };

    GpioClient::GpioClient(const std::string &name, Service &parent)
        : bbox::rt::Service(name, parent)
        , m_pimpl(std::make_unique<Pimpl>())
    {
    }

    GpioClient::~GpioClient()
    {
    }

    bool GpioClient::GetInput(size_t input) const
    {
        m_pimpl->inputs.insert(input);

#ifdef WIN32

        // TODO - only support the gay-sign application at the moment
        // NOTE - hardware LED on box is reverse logic - 1 means not pressed
        BBOX_ASSERT(input == 17);

        return (GetKeyState(VK_LCONTROL) & 0x8000) == 0;

#else
        std::string filename("/sys/class/gpio/gpio");
        filename += std::to_string(input);
        filename += "/value";

        char ch = '0';

        int fd = open(filename.c_str(),
            O_RDONLY,
            S_IRWXU | S_IRWXG | S_IRWXO);

        if (fd != -1)
        {
            read(fd, &ch, 1);

            close(fd);
        }

        return ch != '0';
#endif
    }

    void GpioClient::SetOutput(size_t output, bool value)
    {
        m_pimpl->outputs[output] = value;

#ifdef WIN32

        // TODO - only support the gay-sign application at the moment
        BBOX_ASSERT(output == 27);

        static bool cur_value = false;
        static bool first = true;

        if (first || (cur_value != value))
        {
            first = false;
            cur_value = value;
            SetConsoleTitle(value ? L"--- LED ON ---" : L"--------------");
        }

#else
        std::string filename("/sys/class/gpio/gpio");
        filename += std::to_string(output);
        filename += "/value";

        char ch = (value ? '1' : '0');

        int fd = open(filename.c_str(),
            O_WRONLY | O_TRUNC,
            S_IRWXU | S_IRWXG | S_IRWXO);

        if (fd != -1)
        {
            write(fd, &ch, 1);

            close(fd);
        }
#endif
    }

    void GpioClient::HandleStarting()
    {
        NotifyStarted();
    }

    void GpioClient::HandleStopping()
    {
        RequestStopAllChildren();
        NotifyStopped();
    }

    void GpioClient::PrintState(bbox::DebugOutput &out) const
    {
        for (const auto &entry : m_pimpl->outputs)
        {
            out.Format("Output %d : %s\n", entry.first, entry.second ? "ON" : "OFF");
        }

        for (const size_t input : m_pimpl->inputs)
        {
            out.Format("Input %d : %s\n", input, GetInput(input) ? "ON" : "OFF");
        }
    }

} // namespace leds
