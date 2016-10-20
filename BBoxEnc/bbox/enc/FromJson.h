/**
 * @file
 *
 * Header file for the bbox::enc::FromJson class.
 *
 */

#ifndef __BBOX__ENC__FROM_JSON_H__
#define __BBOX__ENC__FROM_JSON_H__

#include <bbox/enc/FromTextFormat.h>
#include <json/json.hpp>

namespace bbox
{
    namespace enc
    {

        /**
         * A class that can decode values from XML.
         */
        class FromJson: public FromTextFormat
        {
        private:
            FromJson() = delete;

        public:
            /**
             * Constructor from a JSON string.
             */
            explicit FromJson(const std::string &json_str);

            /**
             * Destructor.
             */
            ~FromJson();

            // Implements FromTextFormat
            size_t EstimateNumNamedItemsInStructure() override;
            bool HasNextNamedItem(const char *name) override;
            void StartNamedItem(const char *name) override;
            void CompleteNamedItem() override;
            void StartStructure() override;
            void CompleteStructure() override;
            std::string DecodeStringValue() override;
            void EnsureAllDecoded() override;

        private:

            using json_type = nlohmann::json;
        
            json_type m_top_value;

            struct StackEntry
            {
                bool is_value;
                json_type *value_ptr;
                std::string name_in_parent;
                bool value_decoded;
            };

            std::vector<StackEntry> m_stack;
        };

    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__FROM_JSON_H__
