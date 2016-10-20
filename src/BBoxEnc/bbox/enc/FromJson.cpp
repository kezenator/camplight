/**
 * @file
 *
 * Header file for the bbox::enc::FromJson class.
 *
 */

#include <bbox/enc/FromJson.h>
#include <bbox/Format.h>
#include <bbox/Assert.h>

namespace bbox
{
    namespace enc
    {

        FromJson::FromJson(const std::string &xml)
        {
            try
            {
                m_top_value = json_type::parse(xml);
            }
            catch (const std::invalid_argument &e)
            {
                SetError(bbox::Format("Could not decode JSON: %s", e.what()));
                return;
            }

            if (!m_top_value.is_object()
                && !m_top_value.is_string())
            {
                SetError(bbox::Format("JSON value must only consist of strings and objects - arrays, numbers, booleans and null are not supported"));
                return;
            }

            m_stack.push_back(StackEntry());
            m_stack.back().is_value = true;
            m_stack.back().value_ptr = &m_top_value;
            m_stack.back().value_decoded = false;
        }

        FromJson::~FromJson()
        {
        }

        size_t FromJson::EstimateNumNamedItemsInStructure()
        {
            if (HasError())
                return 0;

            const StackEntry &back = m_stack.back();

            if (back.is_value)
            {
                SetError("EstimateNumNamedItemsInStructure called inside value - must only be called inside structure");
                return 0;
            }

            return back.value_ptr->size();
        }

        bool FromJson::HasNextNamedItem(const char *name)
        {
            if (HasError())
                return false;

            const StackEntry &back = m_stack.back();

            if (back.is_value)
            {
                SetError("HasNextNamedItem called inside value - must only be called inside structure");
                return false;
            }

            auto it = back.value_ptr->find(name);

            return (it != back.value_ptr->end());
        }

        void FromJson::StartNamedItem(const char *name)
        {
            if (HasError())
                return;

            const StackEntry &back = m_stack.back();

            if (back.is_value)
            {
                SetError("StartNamedItem called inside value - must only be called inside structure");
                return;
            }

            auto it = back.value_ptr->find(name);
            
            if (it == back.value_ptr->end())
            {
                SetError(bbox::Format("No such named item \"%s\"", name));
                return;
            }

            m_stack.push_back(back);
            StackEntry &new_back = m_stack.back();

            new_back.is_value = true;
            new_back.value_ptr = &(it.value());
            new_back.name_in_parent = name;

            if (!new_back.value_ptr->is_object()
                && !new_back.value_ptr ->is_string())
            {
                SetError(bbox::Format("JSON value must only consist of strings and objects - arrays, numbers, booleans and null are not supported"));
                return;
            }
        }

        void FromJson::CompleteNamedItem()
        {
            if (HasError())
                return;

            StackEntry &back = m_stack.back();

            if (!back.is_value)
            {
                SetError("CompleteNamedItem called inside structure - must only be called inside value");
                return;
            }

            if (m_stack.size() == 1)
            {
                SetError("CompleteNamedItem called too many times - already at top level");
                return;
            }

            std::string name_in_parent = std::move(back.name_in_parent);

            m_stack.pop_back();

            StackEntry &new_back = m_stack.back();

            // Mark as decoded by clearing it from the parent

            new_back.value_ptr->erase(name_in_parent);
        }

        void FromJson::StartStructure()
        {
            if (HasError())
                return;

            StackEntry &back = m_stack.back();

            if (!back.is_value)
            {
                SetError("StartStructure called inside structure - must only be called inside value");
                return;
            }

            if (back.value_decoded)
            {
                SetError("StartStructure called again after DecodeStringValue or StartStructure/.../CompleteStructure has already been called for this value");
            }

            if (!back.value_ptr->is_object())
            {
                SetError("StartStructure called for JSON string value - expect JSON object value");
                return;
            }

            m_stack.push_back(back);

            StackEntry &new_back = m_stack.back();
            new_back.is_value = false;
        }

        void FromJson::CompleteStructure()
        {
            if (HasError())
                return;

            StackEntry &back = m_stack.back();

            if (back.is_value)
            {
                SetError("CompleteStructure called inside value - must only be called inside structure");
                return;
            }

            if (!back.value_ptr->empty())
            {
                auto it = back.value_ptr->begin();

                SetError(bbox::Format("Un-decoded field \"%s\" remaining in structure", it.key()));
                return;
            }

            m_stack.pop_back();

            StackEntry &new_back = m_stack.back();

            new_back.value_decoded = true;
        }

        std::string FromJson::DecodeStringValue()
        {
            std::string result;

            if (HasError())
            {
                // Already has an error raised
            }
            else
            {
                StackEntry &back = m_stack.back();

                if (!back.is_value)
                {
                    SetError("DecodeStringValue call inside structure - must only be called inside value");
                }
                else if (back.value_decoded)
                {
                    SetError("DecodeStringValue called again after DecodeStringValue or StartStruction/.../CompleteStructure has already been called for this value");
                }
                else if (!back.value_ptr->is_string())
                {
                    SetError("DecodeStringValue called, expect JSON string value but found JSON object value");
                }
                else
                {
                    result = *back.value_ptr;
                    back.value_decoded = true;
                }
            }

            return result;
        }

        void FromJson::EnsureAllDecoded()
        {
            if (HasError())
                return;

            if ((m_stack.size() != 1)
                || !m_stack.back().value_decoded)
            {
                SetError("Un-decoded data remaining");
            }
        }

    } // namespace bbox::enc
} // namespace bbox
