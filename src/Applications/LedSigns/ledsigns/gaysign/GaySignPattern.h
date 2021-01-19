/**
* @file
*
* Header file for the ledsigns::gaysign::GaySignPattern class.
*/

#ifndef __LEDSIGNS__GAYSIGN__GAYSIGN_PATTERN_H__
#define __LEDSIGNS__GAYSIGN__GAYSIGN_PATTERN_H__

#include <ledsigns/common/Pattern.h>
#include <leds/GpioClient.h>

namespace ledsigns
{
    namespace gaysign
    {

        /**
         * The main pattern for the gay sign.
         */
        class GaySignPattern : public common::Pattern
        {
        public:

            explicit GaySignPattern(leds::GpioClient &gpio_client, const common::RenderState &render, size_t delay_time, size_t fade_time);
            ~GaySignPattern();

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            leds::GpioClient &m_gpio_client;
            size_t m_delay_time;
            size_t m_fade_time;
        };

    } // namespace ledsigns::gaysign
} // namespace ledsigns

#endif // __LEDSIGNS__GAYSIGN__GAYSIGN_PATTERN_H__
