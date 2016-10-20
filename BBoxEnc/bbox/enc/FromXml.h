/**
 * @file
 *
 * Header file for the bbox::enc::FromXml class.
 *
 */

#ifndef __BBOX__ENC__FROM_XML_H__
#define __BBOX__ENC__FROM_XML_H__

#include <bbox/enc/FromTextFormat.h>
#include <tinyxml2/tinyxml2.h>

namespace bbox
{
    namespace enc
    {

        /**
         * A class that can decode values from XML.
         */
        class FromXml: public FromTextFormat
        {
        private:
            FromXml() = delete;

        public:
            /**
             * Constructor from an XML string.
             */
            explicit FromXml(const std::string &xml);

            /**
             * Destructor.
             */
            ~FromXml();

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
            tinyxml2::XMLDocument m_doc;
            tinyxml2::XMLElement *m_cur_element;
            bool m_value_mode;
            bool m_first_in_structure;
        };

    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__FROM_XML_H__
