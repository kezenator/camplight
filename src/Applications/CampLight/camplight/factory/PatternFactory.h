/**
 * @file
 *
 * Header file for the camplight::factory::PatternFactory class.
 */

#ifndef __CAMPLIGHT__FACTORY__PATTERN_FACTORY_H__
#define __CAMPLIGHT__FACTORY__PATTERN_FACTORY_H__

#include <camplight/render/Pattern.h>
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
        class PatternFactory
        {
            PatternFactory(const PatternFactory &) = delete;
            PatternFactory &operator =(const PatternFactory &) = delete;

        public:
            PatternFactory();
            ~PatternFactory();

            std::vector<std::string> GetNames() const;
            std::unique_ptr<render::Pattern> CreatePattern(const std::string &name) const;

        private:
            std::map<std::string, std::function<std::unique_ptr<render::Pattern>()>> m_constructors;
        };

    } // namespace camplight::factory
} // namespace camplight

#endif // __CAMPLIGHT__FACTORY__PATTERN_FACTORY_H__
