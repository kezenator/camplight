/**
 * @file
 *
 * Header file for the camplight::factory::SequenceFactory class.
 */

#ifndef __CAMPLIGHT__FACTORY__SEQUENCE_FACTORY_H__
#define __CAMPLIGHT__FACTORY__SEQUENCE_FACTORY_H__

#include <camplight/render/SequenceEntry.h>
#include <vector>
#include <map>
#include <functional>

namespace camplight
{
    namespace factory
    {

        /**
         * A factory that creates patterns.
         */
        class SequenceFactory
        {
            SequenceFactory(const SequenceFactory &) = delete;
            SequenceFactory &operator =(const SequenceFactory &) = delete;

        public:
            SequenceFactory();
            ~SequenceFactory();

            std::vector<std::string> GetNames() const;
            std::vector<render::SequenceEntry> CreateDefaultSequenceEntries() const;
            std::vector<render::SequenceEntry> CreateSequenceEntries(const std::string &name) const;

        private:
            static std::vector<render::SequenceEntry> CreateColorWave(std::vector<std::string> colors, bbox::rt::TimeSpan duration);

            std::map<std::string, std::function<std::vector<render::SequenceEntry>()>> m_constructors;
        };

    } // namespace camplight::factory
} // namespace camplight

#endif // __CAMPLIGHT__FACTORY__SEQUENCE_FACTORY_H__
