/**
 * @file
 *
 * Implementation file for the camplight::factory::TransitionFactory class.
 */

#include <camplight/factory/TransitionFactory.h>

#include <camplight/transition/LinearFade.h>
#include <camplight/transition/RandomDither.h>
#include <camplight/transition/Immediate.h>

namespace camplight
{
    namespace factory
    {

        TransitionFactory::TransitionFactory()
        {
            m_constructors["Fade"] = [](bbox::rt::TimeSpan duration) { return std::make_unique<transition::LinearFade>(duration); };
            m_constructors["Random Dither"] = [](bbox::rt::TimeSpan duration) { return std::make_unique<transition::RandomDither>(duration); };
            m_constructors["Immediate"] = [](bbox::rt::TimeSpan /*duration*/) { return std::make_unique<transition::Immediate>(); };
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

        std::unique_ptr<render::Transition> TransitionFactory::CreateTransition(const std::string &name, bbox::rt::TimeSpan duration) const
        {
            auto it = m_constructors.find(name);

            if (it == m_constructors.end())
                return std::make_unique<transition::LinearFade>(duration);
            else
                return it->second(duration);
        }

    } // namespace camplight::factory
} // namespace camplight
