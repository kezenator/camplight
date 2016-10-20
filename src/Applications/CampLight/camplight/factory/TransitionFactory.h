/**
 * @file
 *
 * Header file for the camplight::factory::TransitionFactory class.
 */

#ifndef __CAMPLIGHT__FACTORY__TRANSITION_FACTORY_H__
#define __CAMPLIGHT__FACTORY__TRANSITION_FACTORY_H__

#include <camplight/render/Transition.h>
#include <memory>
#include <map>
#include <functional>

namespace camplight
{
    namespace factory
    {

        /**
         * A factory that creates patterns.
         */
        class TransitionFactory
        {
            TransitionFactory(const TransitionFactory &) = delete;
            TransitionFactory &operator =(const TransitionFactory &) = delete;

        public:
            TransitionFactory();
            ~TransitionFactory();

            std::vector<std::string> GetNames() const;
            std::unique_ptr<render::Transition> CreateTransition(const std::string &name, bbox::rt::TimeSpan duration) const;

        private:
            std::map<std::string, std::function<std::unique_ptr<render::Transition>(bbox::rt::TimeSpan duration)>> m_constructors;
        };

    } // namespace camplight::factory
} // namespace camplight

#endif // __CAMPLIGHT__FACTORY__TRANSITION_FACTORY_H__
