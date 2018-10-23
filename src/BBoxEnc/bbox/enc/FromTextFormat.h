/**
 * @file
 *
 * Header file for the bbox::enc::FromTextFormat class.
 *
 */

#ifndef __BBOX__ENC__FROM_TEXT_FORMAT_H__
#define __BBOX__ENC__FROM_TEXT_FORMAT_H__

#include <bbox/ToString.h>

#include <bbox/enc/details/MethodDetection.h>
#include <bbox/enc/details/MarshalActions.h>

namespace bbox
{
    namespace enc
    {

        /**
         * A base class that allows types to be converted
         * from a textual formats, such as XML, JSON etc.
         */
        class FromTextFormat
        {
        private:
            FromTextFormat(const FromTextFormat &) = delete;
            void operator =(const FromTextFormat &) = delete;

        protected:
            /**
             * Base constructor.
             */
            FromTextFormat();

        public:

            /**
             * Destructor.
             */
            virtual ~FromTextFormat();

            /**
             * Determines if an error has been raised.
             */
            bool HasError() const { return m_Error; }

            /**
             * Gets the error string that has been raised.
             */
            const std::string &GetErrorString() const { return m_ErrorStr; }

            /**
             * Sets an error on the decoder.
             */
            void SetError(const std::string &error_str);

            /**
             * Sets an error on the decoder.
             */
            void SetError(std::string &&error_str);

            /**
             * Estimates the number of named items
             * in the current structure.
             */
            virtual size_t EstimateNumNamedItemsInStructure() = 0;

            /**
             * Determines if there is another named item
             * with the specified name.
             */
            virtual bool HasNextNamedItem(const char *name) = 0;
            
            /**
             * Determines if there is another named item
             * with the specified name.
             */
            virtual bool HasNextNamedItem(const std::string &name);

            /**
             * Starts a new named value.
             */
            virtual void StartNamedItem(const char *name) = 0;

            /**
             * Completes the current named value.
             */
            virtual void CompleteNamedItem() = 0;

            /**
             * Starts decoding the current value as a structure.
             */
            virtual void StartStructure() = 0;

            /**
             * Completes the current structure, returning to expect the
             * next named item in the parent.
             */
            virtual void CompleteStructure() = 0;

            /**
             * Decodes the current named value as a string.
             */
            virtual std::string DecodeStringValue() = 0;

			/**
			 * Decodes a MsgAnyPtr object.
			 */
			MsgAnyPtr DecodeMsgAnyPtr();

            /**
             * Ensures there is no decoded data left.
             */
            virtual void EnsureAllDecoded() = 0;

            /**
             * Decodes the current named value as binary data.
             */
            virtual std::vector<uint8_t> DecodeBinaryDataValue();

            /**
             * Starts a named structure.
             */
            void StartNamedStructure(const char *name)
            {
                StartNamedItem(name);
                StartStructure();
            }

            /**
             * Ends a named structure.
             */
            void CompleteNamedStructure()
            {
                CompleteStructure();
                CompleteNamedItem();
            }

            /**
             * Adds a named value.
             */
            void DecodeNamedValue(const char *name, std::string &str_value)
            {
                StartNamedItem(name);
                str_value = DecodeStringValue();
                CompleteNamedItem();
            }

            /**
             * Decodes a named size_t value.
             */
            virtual void DecodeNamedSizeT(const char *name, size_t &value);

            template <typename Type>
            void DecodeValue(Type &value)
            {
                using Action = details::FromTextFormatAction<typename details::ChooseStrategy<Type>::Strategy, Type>;

                Action::Impl(*this, value);
            }

            /**
             * Adds a value using the ToTextFormat method of the value.
             */
            template <typename Type>
            void DecodeNamedValue(const char *name, Type &value)
            {
                using Action = details::FromTextFormatAction<typename details::ChooseStrategy<Type>::Strategy, Type>;

                StartNamedItem(name);
                Action::Impl(*this, value);
                CompleteNamedItem();
            }

        private:
            bool m_Error;
            std::string m_ErrorStr;
        };

    } // namespace bbox::enc
} // namespace bbox

#ifndef __BBOX__ENC__MSG_PTR_H__
#include <bbox/enc/details/FromTextFormatAction.h>
#endif // not __BBOX__ENC__MSG_PTR_H__

#endif // __BBOX__ENC__FROM_TEXT_FORMAT_H__
