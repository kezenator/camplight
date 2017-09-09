/**
 * @file
 *
 * Header file for the leds::GpioClient class.
 */

#ifndef __LEDS_GPIO_CLIENT_H__
#define __LEDS_GPIO_CLIENT_H__

#include <bbox/rt/Service.h>

namespace leds
{

    /**
     * Reads and writes GPIO values via the /sys/class/gpio file-system on the Raspberry Pi.
     * The pins must already be setup in the correct manner - e.g. using the "gpio" command.
     *
     */
    class GpioClient : public bbox::rt::Service
    {
    public:
        GpioClient(const std::string &name, Service &parent);
        ~GpioClient();

        void SetOutput(size_t output, bool value);
        bool GetInput(size_t input) const;

    protected:
        void HandleStarting() override;
        void HandleStopping() override;
        void PrintState(bbox::DebugOutput &out) const override;

    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> m_pimpl;
    };

} // namespace leds

#endif // __LEDS_GPIO_CLIENT_H__
