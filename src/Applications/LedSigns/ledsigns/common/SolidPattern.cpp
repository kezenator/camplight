/**
 * @file
 *
 * Implementation file for the ledsigns::common::SolidPattern class.
 */

#include <ledsigns/common/SolidPattern.h>

namespace ledsigns
{
    namespace common
    {

        SolidPattern::SolidPattern(const common::RenderState & /*render*/, leds::Color color)
            : m_color(color)
        {
        }

        SolidPattern::~SolidPattern()
        {
        }

        std::string SolidPattern::GetName() const
        {
            return bbox::Format("Solid %s", m_color.ToString());
        }
        
        void SolidPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("No state");
        }

        std::vector<leds::Color> SolidPattern::Render(const common::RenderState &render)
        {
            return std::vector<leds::Color>(render.layout.num_leds, m_color);
        }

    } // namespace ledsigns::common
} // namespace ledsigns
