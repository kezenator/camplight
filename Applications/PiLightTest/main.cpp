#include <iostream>
#include <vector>
#include <ws2811.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void usage()
{
    std::cout << "Usage: pilighttest gpio num_strips strip_length" << std::endl;
    std::cout << "            gpio = Raspberry Pi GPIO number (processor pin number, not board pin number)" << std::endl;
    std::cout << "                                            (must have PWM function)" << std::endl;
    std::cout << "            num_strips = the number of strips connected in series [1..20]" << std::endl;
    std::cout << "            strip_length = ths number of LEDs on each strip [1..20]" << std::endl;
}

bool fromString(const char *arg, int &val, int min, int max)
{
    char *endptr = nullptr;
    long int result = std::strtol(arg, &endptr, 10);

    if (endptr == nullptr)
        return false;
    if (*endptr != 0)
        return false;

    if (result < min)
        return false;
    if (result > max)
        return false;

    val = int(result);

    return true;
}

static volatile int running;

static void ctrl_c_handler(int signum)
{
    running = 0;
}

static void setup_handlers(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = ctrl_c_handler;

    running = 1;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}



int main(int argc, char *argv[])
{
    int gpio, num_strips, strip_length;
  
    if ((argc != 4)
        || !fromString(argv[1], gpio, 1, 37)
        || !fromString(argv[2], num_strips, 1, 20)
        || !fromString(argv[3], strip_length, 1, 20))
    {
        usage();
        return 1;
    }

    int num_leds = num_strips * strip_length;

    std::cout << "Starting with " << num_leds << " LEDs" << std::endl;

    ws2811_t ws {
        nullptr,
        nullptr,
        WS2811_TARGET_FREQ,
        5, // DMA
        {
            {
                gpio,
                false, // invert
                num_leds,
                255, // brightness
                WS2811_STRIP_RGB,
                nullptr
            },
            {
                0,
                0,
                0,
                0,
                0,
                nullptr
            },
        },
    };

    setup_handlers();

    if (ws2811_init(&ws))
    {
        std::cerr << "Couldn't initialize LEDs" << std::endl;
        return 1;
    }

    int count = 0;

    while (running)
    {
        for (int i = 0; i < strip_length; ++i)
        {
            ws2811_led_t color = 0;

            int offset = (count + i) % strip_length;
            if (offset == 0)
                color = 0x00ff0000;
            else if (offset == 1)
                color = 0x0000ff00;
            else if (offset == 2)
                color = 0x000000ff;
            else if (offset == 3)
                color = 0x00ffffff;

            for (int j = 0; j < num_strips; j++)
            {
                ws.channel[0].leds[i + j*strip_length] = color;
            }
        }

        if (ws2811_render(&ws))
        {
            std::cerr << "Couldn't render LEDs" << std::endl;
            ws2811_fini(&ws);
            return 1;
        }

        // 4 frames /sec
        usleep(1000000 / 4);
        count = (count + 1) % strip_length;
    }

    // Turn off all LEDs when shutting down

    for (int i = 0; i < num_leds; ++i)
        ws.channel[0].leds[i] = 0;

    ws2811_render(&ws);
    ws2811_wait(&ws);
    ws2811_fini(&ws);
    
    return 0;
}