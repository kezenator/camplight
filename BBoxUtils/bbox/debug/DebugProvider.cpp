/**
* @file
*
* Implementation file for the bbox::debug::DebugProvider class.
*/

#include <bbox/debug/DebugProvider.h>
#include <bbox/debug/DebugRoot.h>
#include <bbox/debug/DebugStream.h>
#include <bbox/debug/DebugTarget.h>
#include <bbox/Assert.h>
#include <map>
#include <set>
#include <functional>

namespace bbox
{
    namespace debug
    {
        thread_local DebugProvider *DebugProvider::t_provider_ptr;

		struct DebugProvider::Pimpl
		{
			Pimpl()
				: m_active_stream_ptr(nullptr)
				, m_target_ptrs()
			{
			}

			DebugStream *m_active_stream_ptr;
			std::set<DebugTarget *, std::less<DebugTarget *>> m_target_ptrs;
			std::map<std::string, DebugRoot *> m_root_ptr_map;
		};

        DebugProvider::DebugProvider()
            : m_pimpl(std::make_unique<Pimpl>())
        {
            BBOX_ASSERT(t_provider_ptr == nullptr);
            t_provider_ptr = this;
        }

        DebugProvider::~DebugProvider()
        {
            BBOX_ASSERT(t_provider_ptr == this);
			BBOX_ASSERT(m_pimpl->m_active_stream_ptr == nullptr);
			BBOX_ASSERT(m_pimpl->m_target_ptrs.empty());
			BBOX_ASSERT(m_pimpl->m_root_ptr_map.empty());

            t_provider_ptr = nullptr;
        }

		DebugStream *DebugProvider::AllocateStream(const char *func, DebugReport *user_report_ptr, std::string &&reason)
		{
			DebugReport::Report *report_ptr = nullptr;
			DebugStream *parent_ptr = nullptr;

			if (!user_report_ptr)
			{
				// It's an async output - see if there is an existing active
				// stream that we can connect this to

				if (m_pimpl->m_active_stream_ptr)
				{
					report_ptr = m_pimpl->m_active_stream_ptr->m_report_ptr;
					parent_ptr = m_pimpl->m_active_stream_ptr;
				}
			}

			// Ensure any parent is flushed before we start
			// new output in a sub-output

			if (parent_ptr)
			{
				parent_ptr->flush();
			}

			// Create the report if we're a top level stream

			if (!report_ptr)
			{
				report_ptr = new DebugReport::Report();
			}

			// Always create a new output item for this stream

			report_ptr->output_entries.push_back(DebugReport::OutputEntry(func, std::move(reason), DebugOutput::Mime_Text_Plain));

			// Create the stream instance

			DebugStream *stream = new DebugStream(
                this,
                parent_ptr,
                report_ptr,
                report_ptr->output_entries.size() - 1,
                user_report_ptr);

			// If it's an async entry then register it

			if (!user_report_ptr)
			{
				m_pimpl->m_active_stream_ptr = stream;
			}

			// All done

			return stream;
		}

        DebugStream *DebugProvider::AllocateStream(const char *func, DebugStream *parent_ptr, DebugOutput::E_MIME_TYPE mime_type)
        {
            BBOX_ASSERT(parent_ptr);
            BBOX_ASSERT(parent_ptr->m_report_ptr);

            if (!parent_ptr->m_user_report_ptr)
            {
                // It's an async output - ensure that it's the active stream

                BBOX_ASSERT(m_pimpl->m_active_stream_ptr == parent_ptr);
            }

            // Ensure any parent is flushed before we start
            // new output in a sub-output

            if (parent_ptr)
            {
                parent_ptr->flush();
            }

            // Always create a new output item for this stream

            parent_ptr->m_report_ptr->output_entries.push_back(DebugReport::OutputEntry(func, "Change MIME type", mime_type));

            // Create the stream instance

            DebugStream *stream = new DebugStream(
                this,
                parent_ptr,
                parent_ptr->m_report_ptr,
                parent_ptr->m_report_ptr->output_entries.size() - 1,
                parent_ptr->m_user_report_ptr);

            // If it's an async entry then register it

            if (!parent_ptr->m_user_report_ptr)
            {
                m_pimpl->m_active_stream_ptr = stream;
            }

            // All done

            return stream;
        }

		void DebugProvider::DestroyStream(DebugStream *stream)
		{
			BBOX_ASSERT(stream);

			// If it's an async entry then ensure it's the active stream -
			// i.e. they have to be created and destroyed on the stack,
			// and return it's parent as the active stream

			if (!stream->m_user_report_ptr)
			{
				BBOX_ASSERT(m_pimpl->m_active_stream_ptr == stream);

				m_pimpl->m_active_stream_ptr = stream->m_parent_stream_ptr;
			}

			// Save the stream to RAII variables
			// in case there are any errors processing the results

			std::unique_ptr<DebugStream> raii_stream_ptr(stream);

			// Ensure all data is flushed to the report

			raii_stream_ptr->flush();

			// If it has no parent then we need to process
			// it's output

			if (!stream->m_parent_stream_ptr)
			{
                DebugReport raii_report(stream->m_report_ptr);
                
                if (stream->m_user_report_ptr)
				{
					// We've been provided with a report
					// to store the data in - just do that

					*stream->m_user_report_ptr = std::move(raii_report);
				}
				else
				{
					// It's an async output - we pass it to all
					// registered handlers

					for (DebugTarget *target_ptr : m_pimpl->m_target_ptrs)
					{
						target_ptr->OutputToTarget(raii_report);
					}
				}
			}
		}

		void DebugProvider::LoginRoot(DebugRoot *root_ptr)
		{
			BBOX_ASSERT(t_provider_ptr == this);

			auto insert_result = m_pimpl->m_root_ptr_map.insert(std::make_pair(root_ptr->GetName(), root_ptr));

			BBOX_ASSERT(insert_result.second);
		}

		void DebugProvider::LogoutRoot(DebugRoot *root_ptr)
		{
			BBOX_ASSERT(t_provider_ptr == this);

			auto erase_result = m_pimpl->m_root_ptr_map.erase(root_ptr->GetName());

			BBOX_ASSERT(erase_result == 1);
		}

		std::map<std::string, DebugRoot *> &DebugProvider::GetRootMap()
		{
			BBOX_ASSERT(t_provider_ptr == this);

			return m_pimpl->m_root_ptr_map;
		}

		void DebugProvider::LoginTarget(DebugTarget *target_ptr)
		{
			BBOX_ASSERT(t_provider_ptr == this);

			auto insert_result = m_pimpl->m_target_ptrs.insert(target_ptr);

			BBOX_ASSERT(insert_result.second);
		}

		void DebugProvider::LogoutTarget(DebugTarget *target_ptr)
		{
			BBOX_ASSERT(t_provider_ptr == this);

			auto erase_result = m_pimpl->m_target_ptrs.erase(target_ptr);

			BBOX_ASSERT(erase_result == 1);
		}

    }
}
