/**
 * @file
 *
 * Header file for the camplight::pattern::Text class.
 */

#ifndef __CAMPLIGHT__PATTERN__TEXT_H__
#define __CAMPLIGHT__PATTERN__TEXT_H__

#include <camplight/render/Pattern.h>

namespace camplight
{
    namespace pattern
    {

        /**
         * A pattern that displays text.
         */
        class Text : public render::Pattern
        {
            Text() = delete;

        public:
            explicit Text(const std::string &text);
            ~Text();

            std::string GetDescription() const override;
            void RenderTo(render::ColorGrid &dest, render::Timestamp timestamp) override;

        private:
            const std::string m_text;
        };

    } // namespace camplight::pattern
} // namespace camplight

#endif // __CAMPLIGHT__PATTERN__TEXT_H__
