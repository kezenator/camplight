/**
 * @file
 *
 * Header file for the camplight::pattern::Solid class.
 */

#ifndef __CAMPLIGHT__PATTERN__SOLID_H__
#define __CAMPLIGHT__PATTERN__SOLID_H__

#include <camplight/render/Pattern.h>

namespace camplight
{
    namespace pattern
    {

        /**
         * A pattern that is a solid color.
         */
        class Solid: public render::Pattern
        {
            Solid() = delete;

        public:
            explicit Solid(render::Color color);
            ~Solid();

            std::string GetDescription() const override;
            void RenderTo(render::ColorGrid &dest, render::Timestamp timestamp) override;

        private:
            const render::Color m_color;
        };

    } // namespace camplight::pattern
} // namespace camplight

#endif // __CAMPLIGHT__PATTERN__SOLID_H__
