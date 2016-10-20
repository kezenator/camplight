/**
 * @file
 *
 * Implementation file for the camplight::pattern::Solid class.
 */

#include <camplight/pattern/Solid.h>
#include <bbox/Format.h>

namespace camplight
{
    namespace pattern
    {

        Solid::Solid(render::Color color)
            : m_color(color)
        {
        }

        Solid::~Solid()
        {
        }

        std::string Solid::GetDescription() const
        {
            return bbox::Format("Solid[color=%s]", m_color.ToString());
        }

        void Solid::RenderTo(render::ColorGrid &dest, render::Timestamp timestamp)
        {
            for (size_t i = 0; i < GRID_SIZE; ++i)
            {
                for (size_t j = 0; j < GRID_SIZE; ++j)
                {
                    dest[i][j] = m_color;
                }
            }
        }

    } // namespace camplight::pattern
} // namespace camplight
