/**
 * @file
 *
 * Implementation file for the camplight::factory::TransitionFactory class.
 */

#include <camplight/factory/TransitionFactory.h>

#include <camplight/transition/LinearFade.h>
#include <camplight/transition/RandomDither.h>
#include <camplight/transition/Immediate.h>
#include <camplight/transition/Swipe.h>

namespace camplight
{
    namespace factory
    {

        TransitionFactory::TransitionFactory()
        {
            m_constructors["Fade"] = []() { return std::make_unique<transition::LinearFade>(bbox::rt::TimeSpan::Seconds(2)); };
            m_constructors["Slow Fade"] = []() { return std::make_unique<transition::LinearFade>(bbox::rt::TimeSpan::Seconds(10)); };
            m_constructors["Random Dither"] = []() { return std::make_unique<transition::RandomDither>(bbox::rt::TimeSpan::Seconds(1)); };
            m_constructors["Immediate"] = []() { return std::make_unique<transition::Immediate>(); };

            m_constructors["Random"] = []() -> std::unique_ptr<render::Transition>
            {
                switch (rand() % 4)
                {
                case 0:
                    return std::make_unique<transition::RandomDither>(bbox::rt::TimeSpan::Seconds(1));
                case 1:
                    return std::make_unique<transition::Swipe>(bbox::rt::TimeSpan::Seconds(1));
                case 2:
                case 3:
                default:
                    return std::make_unique<transition::LinearFade>(bbox::rt::TimeSpan::Seconds(2));
                }
            };
        }
        
        TransitionFactory::~TransitionFactory()
        {
        }

        std::vector<std::string> TransitionFactory::GetNames() const
        {
            std::vector<std::string> result;

            result.reserve(m_constructors.size());

            for (const auto &entry : m_constructors)
                result.push_back(entry.first);

            return result;
        }

        std::unique_ptr<render::Transition> TransitionFactory::CreateTransition(const std::string &name) const
        {
            auto it = m_constructors.find(name);

            if (it == m_constructors.end())
                return std::make_unique<transition::LinearFade>(bbox::rt::TimeSpan::Seconds(1));
            else
                return it->second();
        }

    } // namespace camplight::factory
} // namespace camplight
