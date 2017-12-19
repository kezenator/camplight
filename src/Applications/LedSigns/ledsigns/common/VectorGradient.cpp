/**
 * @file
 *
 * Implementation file for the ledsigns::common::VectorGradient class.
 */

#include <ledsigns/common/VectorGradient.h>

namespace ledsigns
{
    namespace common
    {

        VectorGradient::VectorGradient(std::string &&name,
                                       std::map<double, leds::Color> &&color_map)
            : m_name(std::move(name))
            , m_color_map(std::move(color_map))
        {
        }

        VectorGradient::~VectorGradient()
        {
        }

        std::string VectorGradient::GetName() const
        {
            return m_name;
        }

        void VectorGradient::PrintInformation(bbox::DebugOutput &out) const
        {
            for (const auto &entry : m_color_map)
            {
                out.Format("%10.5f : %s\n",
                    entry.first,
                    entry.second.ToString());
            }
        }

        leds::Color VectorGradient::Convert(double progress) const
        {
            if (m_color_map.empty())
                return leds::Color(0, 0, 0);

            if (progress < 0.0)
                progress = 0.0;
            if (progress > 1.0)
                progress = 1.0;

            leds::Color from, to;
            double distance;

            // lower_bound returns first element that is
            // not less progress (or end()) - so we want
            // to interpolate between the previous and
            // the returned entry.

            auto it = m_color_map.lower_bound(progress);

            if (it == m_color_map.end())
            {
                // Progress is at/after last entry -
                // we want from the last to the first
                // including progress from last to 1.0.

                auto last_it = m_color_map.end();
                --last_it;

                auto first_it = m_color_map.begin();

                from = last_it->second;
                to = first_it->second;

                distance = (progress - last_it->first) / (first_it->first + 1.0 - last_it->first);
            }
            else if (it == m_color_map.begin())
            {
                // Progress is before the first entry -
                // we want from the last to the first
                // but including progress from last to 1.0

                auto last_it = m_color_map.end();
                --last_it;

                auto first_it = m_color_map.begin();

                from = last_it->second;
                to = first_it->second;

                distance = (progress + (1.0 - last_it->first)) / (first_it->first + 1.0 - last_it->first);
            }
            else
            {
                auto prev_it = it;
                --prev_it;

                from = prev_it->second;
                to = it->second;

                distance = (progress - prev_it->first) / (it->first - prev_it->first);
            }

            // Apply the smoothstep S1(x) function 3x^3 - 2x^3

            distance = distance * distance * (3.0 - 2.0 * distance);

            // Fade smoothly

            return from.Fade(to, uint8_t(distance * 255.0));
        }

    } // namespace ledsigns::common
} // namespace ledsigns
