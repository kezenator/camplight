/**
 * @file
 *
 * Header file for the bbox::enc::ToTextFormat class.
 *
 */

#ifndef __BBOX__ENC__TO_TEXT_FORMAT_H__
#define __BBOX__ENC__TO_TEXT_FORMAT_H__

#include <bbox/ToString.h>

#include <bbox/enc/details/MethodDetection.h>
#include <bbox/enc/details/MarshalActions.h>

namespace bbox
{
    namespace enc
    {

        /**
         * A base class that allows types to be converted
         * to textual formats, such as XML, JSON etc.
         */
        class ToTextFormat
        {
        private:
            ToTextFormat(const ToTextFormat &) = delete;
            void operator =(const ToTextFormat &) = delete;

        public:
            /**
             * The text encoding target.
             */
            enum E_TARGET
            {
                MACHINE_ROUND_TRIP,
                HUMAN_DEBUGGING,
            };

        protected:
            /**
             * Base constructor.
             */
            explicit ToTextFormat(E_TARGET target = MACHINE_ROUND_TRIP);

        public:

            /**
             * Destructor.
             */
            virtual ~ToTextFormat();

            /**
             * Gets the target for the text.
             */
            E_TARGET GetTarget() const { return m_target; }

            /**
             * Starts a new named value.
             */
            virtual void StartNamedItem(const char *name) = 0;

            /**
             * Completes the current named value.
             */
            virtual void CompleteNamedItem() = 0;

            /**
             * Starts a structure to represent the current named value.
             * A structure can contain sub-named-value pairs.
             */
            virtual void StartStructure() = 0;

            /**
             * Completes the current structure value.
             */
            virtual void CompleteStructure() = 0;

            /**
             * Adds a value to represent the current named value.
             */
            virtual void SetStringValue(const std::string &canonical, const std::string &human_readable);

            /**
             * Adds a value to represent the current named value. The canonical value is used
             * as the human readable value.
             */
            virtual void SetStringValue(const std::string &canonical) = 0;

            virtual void SetBinaryDataValue(const std::vector<uint8_t> &data);

			void SetMsgAnyPtrValue(const MsgAnyPtr &val);

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
            void AddNamedValue(const char *name, const std::string &canonical)
            {
                StartNamedItem(name);
                SetStringValue(canonical);
                CompleteNamedItem();
            }

            /**
             * Adds a named value.
             */
            void AddNamedValue(const char *name, const std::string &canonical, const std::string &human_readable)
            {
                StartNamedItem(name);
                SetStringValue(canonical, human_readable);
                CompleteNamedItem();
            }

            /**
             * Adds a named size_t value.
             */
            void AddNamedSizeT(const char *name, const size_t value)
            {
                StartNamedItem(name);
                SetStringValue(bbox::ToString(value));
                CompleteNamedItem();
            }

            template <typename Type>
            void SetValue(const Type &value)
            {
                using Action = details::ToTextFormatAction<typename details::ChooseStrategy<Type>::Strategy, Type>;

                Action::Impl(*this, value);
            }

            /**
             * Adds a value using the ToTextFormat method of the value.
             */
            template <typename Type>
            void AddNamedValue(const char *name, const Type &value)
            {
                using Action = details::ToTextFormatAction<typename details::ChooseStrategy<Type>::Strategy, Type>;

                StartNamedItem(name);
                Action::Impl(*this, value);
                CompleteNamedItem();
            }

        private:
                E_TARGET m_target;
        };

    } // namespace bbox::enc
} // namespace bbox

#ifndef __BBOX__ENC__MSG_PTR_H__
#include <bbox/enc/details/ToTextFormatAction.h>
#endif // not __BBOX__ENC__MSG_PTR_H__

#endif // __BBOX__ENC__TO_TEXT_FORMAT_H__
