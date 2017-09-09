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

    void DebugOutput::Enable(const char *func, std::string &&reason)
	{
		if (!m_stream_ptr)
		{
			debug::DebugProvider *provider_ptr = debug::DebugProvider::t_provider_ptr;

			if (provider_ptr)
			{
				m_stream_ptr = provider_ptr->AllocateStream(func, nullptr, std::move(reason));
			}
		}
	}

	void DebugOutput::DestroyStream()
	{
		m_stream_ptr->m_provider_ptr->DestroyStream(m_stream_ptr);
	}

    void DebugOutput::PrintData(const void *data, size_t length)
    {
        static constexpr size_t BYTES_PER_ROW = 16;

        BBOX_ASSERT(m_stream_ptr);

        const uint8_t *buffer = static_cast<const uint8_t *>(data);
        size_t offset = 0;
        while (offset < length)
        {
            Format("0x%08X:", offset);

            size_t this_time = length - offset;
            if (this_time > BYTES_PER_ROW)
                this_time = BYTES_PER_ROW;

            for (size_t i = 0; i < this_time; ++i)
                Format(" %02X", buffer[offset + i]);
            for (size_t i = this_time; i < BYTES_PER_ROW; ++i)
                *m_stream_ptr << "   ";

            *m_stream_ptr << "   ";

            for (size_t i = 0; i < this_time; ++i)
            {
                char ch = buffer[offset + i];

                Format("%c", ((ch >= ' ') && (ch <= '~')) ? ch : '.');
            }

            *m_stream_ptr << std::endl;

            offset += this_time;
        }
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
