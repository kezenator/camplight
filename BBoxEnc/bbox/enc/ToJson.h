/**
 * @file
 *
 * Header file for the bbox::enc::ToJson class.
 *
 */

#ifndef __BBOX__ENC__TO_JSON_H__
#define __BBOX__ENC__TO_JSON_H__

#include <bbox/enc/ToTextFormat.h>
#include <sstream>
#include <vector>

namespace bbox
{
    namespace enc
    {

        /**
         * A class that converts values to a JSON text format.
         */
        class ToJson : public ToTextFormat
        {
        public:

            explicit ToJson(E_TARGET target = MACHINE_ROUND_TRIP);
            ~ToJson();

            std::string ToString() const
            {
                return m_stream.str();
            }

            // Implements ToTextFormat
            void StartNamedItem(const char *name) override;
            void CompleteNamedItem() override;
            void StartStructure() override;
            void CompleteStructure() override;
            void SetStringValue(const std::string &canonical) override;

        private:

            std::string EscapeString(const std::string &val);

            std::stringstream m_stream;
            std::vector<bool> m_first_in_structure;
        };

    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__TO_JSON_H__
