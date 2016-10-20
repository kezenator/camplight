/**
* @file
*
* Implementation file for the bbox::DebugOutput class.
*/

#include <bbox/DebugOutput.h>
#include <bbox/debug/DebugProvider.h>
#include <bbox/debug/DebugStream.h>
#include <bbox/Assert.h>

namespace bbox
{

	const char *DebugOutput::c_str(E_ENABLE enable)
	{
		switch (enable)
		{
		case Never: return "Never";
		case Activity: return "Activity";
		case Testing: return "Testing";
		case Error: return "Error";
		}
		return "Unknown";
	}

	DebugOutput::DebugOutput(const char *func, bbox::debug::DebugReport &report, std::string &&reason)
        : m_stream_ptr(nullptr)
	{
		debug::DebugProvider *provider_ptr = debug::DebugProvider::t_provider_ptr;

		BBOX_ASSERT(provider_ptr);

		BBOX_ASSERT(&report != nullptr);

		m_stream_ptr = provider_ptr->AllocateStream(func, &report, std::move(reason));
	}

    DebugOutput::DebugOutput(const char *func, DebugOutput &parent, E_MIME_TYPE mime_type)
        : m_stream_ptr(nullptr)
    {
        BBOX_ASSERT(&parent != nullptr);

        if (parent)
        {
            debug::DebugProvider *provider_ptr = debug::DebugProvider::t_provider_ptr;

            BBOX_ASSERT(provider_ptr);

            m_stream_ptr = provider_ptr->AllocateStream(func, parent.m_stream_ptr, mime_type);
        }
    }

	void DebugOutput::Enable(const char *func, E_ENABLE enable)
	{
		if (!m_stream_ptr
			&& (enable != Never))
		{
			debug::DebugProvider *provider_ptr = debug::DebugProvider::t_provider_ptr;

			if (provider_ptr)
			{
				m_stream_ptr = provider_ptr->AllocateStream(func, nullptr, c_str(enable));
			}
		}
	}

	void DebugOutput::DestroyStream()
	{
		m_stream_ptr->m_provider_ptr->DestroyStream(m_stream_ptr);
	}

    std::ostream &DebugOutput::ostream()
    {
        BBOX_ASSERT(m_stream_ptr);

        return *m_stream_ptr;
    }

	void DebugOutput::IncIndent(size_t change)
	{
        BBOX_ASSERT(m_stream_ptr);

		// TODO
		(void)change;
	}

	void DebugOutput::DecIndent(size_t change)
	{
		BBOX_ASSERT(m_stream_ptr);

		// TODO
		(void)change;
	}

	void DebugOutput::SetIndent(size_t indent)
	{
		BBOX_ASSERT(m_stream_ptr);

		// TODO
		(void)indent;
	}

} // namespace bbox
