/**
 * @file
 *
 * Implementation file for the camplight::factory::SequenceFactory class.
 */

#include <camplight/factory/SequenceFactory.h>

namespace camplight
{
    namespace factory
    {

        SequenceFactory::SequenceFactory()
        {
            m_constructors["White"] = []()
            {
                return std::vector<render::SequenceEntry>({
                    {
                        "Slow Fade",
                        "White",
                        "Slow Fade",
                        "White",
                        bbox::rt::TimeSpan::Minutes(10),
                    },
                });
            };

            m_constructors["Red and Blues"] = []()
            {
                return CreateColorWave({
                    "Orange",
                    "Red",
                    "Magenta",
                    "Blue",
                    "Cyan",
                    "Blue",
                    "Indigo",
                    "Magenta",
                    "Red" },
                    bbox::rt::TimeSpan::Seconds(10));
            };

            m_constructors["Blues and Greens"] = []()
            {
                return CreateColorWave({
                    "Indigo",
                    "Blue",
                    "Cyan",
                    "Green",
                    "Cyan",
                    "Blue" },
                    bbox::rt::TimeSpan::Seconds(10));
            };

            m_constructors["Rainbow"] = []()
            {
                return CreateColorWave({
                    "Blue",
                    "Cyan",
                    "Green",
                    "Yellow",
                    "Orange",
                    "Red",
                    "Magenta",
                    "Indigo" },
                    bbox::rt::TimeSpan::Seconds(10));
            };

            m_constructors["Stuff"] = []()
            {
                return CreateColorWave({
                    "CampIt 2016",
                    "Rainbow" },
                    bbox::rt::TimeSpan::Seconds(30));
            };
        }
        
        SequenceFactory::~SequenceFactory()
        {
        }

        std::vector<std::string> SequenceFactory::GetNames() const
        {
            std::vector<std::string> result;

            result.reserve(m_constructors.size());

            for (const auto &entry : m_constructors)
                result.push_back(entry.first);

            return result;
        }

        std::vector<render::SequenceEntry> SequenceFactory::CreateDefaultSequenceEntries() const
        {
            return CreateSequenceEntries("White");
        }

        std::vector<render::SequenceEntry> SequenceFactory::CreateSequenceEntries(const std::string &name) const
        {
            auto it = m_constructors.find(name);

            if (it == m_constructors.end())
                return std::vector<render::SequenceEntry>();
            else
                return it->second();
        }

        std::vector<render::SequenceEntry> SequenceFactory::CreateColorWave(std::vector<std::string> colors, bbox::rt::TimeSpan duration)
        {
            std::vector<render::SequenceEntry> result;

            bbox::rt::TimeSpan half_duration = bbox::rt::TimeSpan::Milliseconds(duration.ToMilliseconds() / 2);

            for (size_t i = 0; i < colors.size(); ++i)
            {
                result.emplace_back(render::SequenceEntry(
                    "Random",
                    colors[i],
                    "Random",
                    colors[i],
                    half_duration));

                result.emplace_back(render::SequenceEntry(
                    "Random",
                    colors[(i + 1) % colors.size()],
                    "Random",
                    colors[i],
                    half_duration));
            }

            return result;
        }

    } // namespace camplight::factory
} // namespace camplight
