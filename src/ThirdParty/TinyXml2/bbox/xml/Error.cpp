/**
* @file
*
* Implementation for the bbox::xml::ErrorCategory class.
*/

#include <bbox/xml/Error.h>

namespace bbox {
    namespace xml {

        /**
        * An error category for tinyxml2 errors.
        */
        class ErrorCategoryType : public boost::system::error_category
        {
        public:
            const char* name() const noexcept override;
            boost::system::error_condition default_error_condition(int ev) const noexcept override;
            std::string message(int ev) const noexcept override;
        };

        boost::system::error_category &ErrorCategory()
        {
            static ErrorCategoryType singleton;

            return singleton;
        }

        const char* ErrorCategoryType::name() const noexcept
        {
            return "tinyxml2";
        }

        boost::system::error_condition ErrorCategoryType::default_error_condition(int ev) const noexcept 
        {
            boost::system::errc::errc_t condition = boost::system::errc::not_supported;

            switch (static_cast<tinyxml2::XMLError>(ev))
            {
            case tinyxml2::XML_SUCCESS:                             condition = boost::system::errc::success; break;
            case tinyxml2::XML_NO_ATTRIBUTE:                        condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:                condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_FILE_NOT_FOUND:                condition = boost::system::errc::no_such_file_or_directory; break;
            case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:      condition = boost::system::errc::io_error; break;
            case tinyxml2::XML_ERROR_FILE_READ_ERROR:               condition = boost::system::errc::io_error; break;
            case tinyxml2::XML_ERROR_ELEMENT_MISMATCH:              condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_PARSING_ELEMENT:               condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:             condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_IDENTIFYING_TAG:               condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_PARSING_TEXT:                  condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_PARSING_CDATA:                 condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_PARSING_COMMENT:               condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_PARSING_DECLARATION:           condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_PARSING_UNKNOWN:               condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:                condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:            condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_ERROR_PARSING:                       condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_CAN_NOT_CONVERT_TEXT:                condition = boost::system::errc::bad_message; break;
            case tinyxml2::XML_NO_TEXT_NODE:                        condition = boost::system::errc::bad_message; break;
            }

            return condition;
        }

        std::string ErrorCategoryType::message(int ev) const noexcept
        {
            const char *msg = "Unknown";

            switch (static_cast<tinyxml2::XMLError>(ev))
            {
            case tinyxml2::XML_SUCCESS:                             msg = "Success"; break;
            case tinyxml2::XML_NO_ATTRIBUTE:                        msg = "No attribute"; break;
            case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:                msg = "Wrong attribute type"; break;
            case tinyxml2::XML_ERROR_FILE_NOT_FOUND:                msg = "File not found"; break;
            case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:      msg = "File could not be opened"; break;
            case tinyxml2::XML_ERROR_FILE_READ_ERROR:               msg = "File read error"; break;
            case tinyxml2::XML_ERROR_ELEMENT_MISMATCH:              msg = "Element mismatch"; break;
            case tinyxml2::XML_ERROR_PARSING_ELEMENT:               msg = "Error parsing element"; break;
            case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:             msg = "Error parsing attribute"; break;
            case tinyxml2::XML_ERROR_IDENTIFYING_TAG:               msg = "Error identifying tag"; break;
            case tinyxml2::XML_ERROR_PARSING_TEXT:                  msg = "Error parsing text"; break;
            case tinyxml2::XML_ERROR_PARSING_CDATA:                 msg = "Error parsing CDATA"; break;
            case tinyxml2::XML_ERROR_PARSING_COMMENT:               msg = "Error parsing comment"; break;
            case tinyxml2::XML_ERROR_PARSING_DECLARATION:           msg = "Error parsing declaration"; break;
            case tinyxml2::XML_ERROR_PARSING_UNKNOWN:               msg = "Error parsing unknown"; break;
            case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:                msg = "Empty document"; break;
            case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:            msg = "Mismatched element"; break;
            case tinyxml2::XML_ERROR_PARSING:                       msg = "Error parsing"; break;
            case tinyxml2::XML_CAN_NOT_CONVERT_TEXT:                msg = "Can not convert text"; break;
            case tinyxml2::XML_NO_TEXT_NODE:                        msg = "No text node"; break;
            }

            return msg;
        }

    } // namespace bbox::xml
} // namespace bbox
