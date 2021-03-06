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

		switch (input)
		{
			// Gay-sign support - hardware on the box is reverse logic - 1 means not pressed
		case 17:
			return (GetKeyState(VK_LCONTROL) & 0x8000) == 0;

			// Button box support
		case 6:
			return (GetKeyState('A') & 0x8000) == 0;
		case 13:
			return (GetKeyState('S') & 0x8000) == 0;
		case 19:
			return (GetKeyState('D') & 0x8000) == 0;
		case 26:
			return (GetKeyState('F') & 0x8000) == 0;
		case 12:
			return (GetKeyState('G') & 0x8000) == 0;
		case 16:
			return (GetKeyState('H') & 0x8000) == 0;
		case 20:
			return (GetKeyState('P') & 0x8000) == 0;
		case 21:
			return (GetKeyState('B') & 0x8000) == 0;

		default:
			BBOX_ASSERT(false);
			return false;
		}

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
