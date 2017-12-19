/**
 * @file
 *
 * Header file for the ledsigns::common::NoisePattern class.
 */

#ifndef __LEDSIGNS__COMMON__NOISE_PATTERN_H__
#define __LEDSIGNS__COMMON__NOISE_PATTERN_H__

#include <ledsigns/common/Pattern.h>
#include <ledsigns/common/Gradient.h>
#include <FastNoise.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that generates a perlin noise type pattern.
         */
        class NoisePattern : public common::Pattern
        {
        public:

            NoisePattern(const common::RenderState &render,
                         Gradient::Ptr &&gradient,
                         double patch_size,
                         double change_rate_ms);

            ~NoisePattern();

            static common::Pattern::Factory Factory(
                const Gradient::Ptr &gradient,
                double patch_size,
                double change_rate_ms)
            {
                return [=](const common::RenderState &render)
                {
                    Gradient::Ptr grad_copy(gradient);
                    return std::make_unique<NoisePattern>(
                        render,
                        std::move(grad_copy),
                        patch_size,
                        change_rate_ms);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            const uint64_t m_start_time;
            const Gradient::Ptr m_gradient;
            double m_patch_size;
            double m_change_rate_ms;
            FastNoise m_noise;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__NOISE_PATTERN_H__
