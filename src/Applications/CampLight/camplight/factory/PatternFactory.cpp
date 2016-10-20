/**
 * @file
 *
 * Implementation file for the camplight::factory::PatternFactory class.
 */

#include <camplight/factory/PatternFactory.h>

#include <camplight/pattern/Solid.h>
#include <camplight/pattern/DiagonalRainbow.h>
#include <camplight/pattern/TestPattern.h>
#include <camplight/pattern/Text.h>

namespace camplight
{
    namespace factory
    {

        PatternFactory::PatternFactory()
        {
            m_constructors["Black"] = []() { return std::make_unique<pattern::Solid>(render::Color(0, 0, 0)); };
            m_constructors["Red"] = []() { return std::make_unique<pattern::Solid>(render::Color(255, 0, 0)); };
            m_constructors["Green"] = []() { return std::make_unique<pattern::Solid>(render::Color(0, 255, 0)); };
            m_constructors["Blue"] = []() { return std::make_unique<pattern::Solid>(render::Color(0, 0, 255)); };
            m_constructors["Cyan"] = []() { return std::make_unique<pattern::Solid>(render::Color(0, 255, 255)); };
            m_constructors["Yellow"] = []() { return std::make_unique<pattern::Solid>(render::Color(255, 255, 0)); };
            m_constructors["Magenta"] = []() { return std::make_unique<pattern::Solid>(render::Color(255, 0, 255)); };
            m_constructors["White"] = []() { return std::make_unique<pattern::Solid>(render::Color(255, 255, 255)); };

            m_constructors["Rainbow"] = []() { return std::make_unique<pattern::DiagonalRainbox>(); };

            m_constructors["Test Pattern"] = []() { return std::make_unique<pattern::TestPattern>(); };

            m_constructors["CampIt 2016"] = []() { return std::make_unique<pattern::Text>("CampIt 2016 "); };
        }
        
        PatternFactory::~PatternFactory()
        {
        }

        std::vector<std::string> PatternFactory::GetNames() const
        {
            std::vector<std::string> result;

            result.reserve(m_constructors.size());

            for (const auto &entry : m_constructors)
                result.push_back(entry.first);

            return result;
        }

        std::unique_ptr<render::Pattern> PatternFactory::CreatePattern(const std::string &name) const
        {
            auto it = m_constructors.find(name);

            if (it == m_constructors.end())
                return std::make_unique<pattern::Solid>(render::Color(0, 0, 0));
            else
                return it->second();
        }

    } // namespace camplight::factory
} // namespace camplight
