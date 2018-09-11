/**
 * @file
 *
 * Header file for the bbox::enc::ToXml class.
 *
 */

#ifndef __BBOX__ENC__TO_XML_H__
#define __BBOX__ENC__TO_XML_H__

#include <tinyxml2/tinyxml2.h>

#include <bbox/enc/ToTextFormat.h>

namespace bbox
{
    namespace enc
    {

        /**
         * A class that assists marshaling data to an XML format.
         */
        class ToXml: public ToTextFormat
        {
        private:
            ToXml() = delete;
            ToXml(const ToXml &) = delete;
            void operator =(const ToXml &) = delete;

        public:

			/**
			 * Converts a value to an XML string.
			 */
			template <typename Type>
			static std::string ConvertToString(const char *root_element_name, const Type &value)
			{
				tinyxml2::XMLDocument doc;
				ToXml to_xml(doc, root_element_name);
				to_xml.SetValue(value);

				tinyxml2::XMLPrinter printer(nullptr, true);
				doc.Print(&printer);

				std::string result;

				result.reserve(printer.CStrSize());

				result.append(printer.CStr());

				return result;
			}
			
			/**
             * Converts a value to XML, using pretty printing.
             */
            template <typename Type>
            static std::string ConvertToPretty(const char *root_element_name, const Type &value)
            {
                tinyxml2::XMLDocument doc;
                ToXml to_xml(doc, root_element_name);
                to_xml.SetValue(value);

                tinyxml2::XMLPrinter printer;
                doc.Print(&printer);

                std::string result;
                
                result.reserve(printer.CStrSize() + 1);
                
                result.append(printer.CStr());
                result.push_back('\n');

                return result;
            }

            /**
             * Constructs to write to an automatically allocated
             * resizing buffer.
             */
            ToXml(tinyxml2::XMLDocument &doc, const char *root_element_name, E_TARGET target = MACHINE_ROUND_TRIP);

            /**
             * Destructor.
             */
            virtual ~ToXml();

            // Implements ToTextFormat
            void StartNamedItem(const char *name) override;
            void CompleteNamedItem() override;
            void StartStructure() override;
            void CompleteStructure() override;
            void SetStringValue(const std::string &canonical) override;

        private:
            tinyxml2::XMLDocument &m_doc;
            tinyxml2::XMLElement *m_cur_element;
        };

    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__TO_XML_H__
