/**
 * @file
 *
 * Implementation file for the bbox::debug::DebugStream class.
 */

#include <bbox/debug/DebugStream.h>

namespace bbox
{
    namespace debug
    {

		DebugStream::DebugStream(DebugProvider *provider_ptr, DebugStream *parent_ptr, DebugReport::Report *report_ptr, size_t report_output_index, DebugReport *user_report_ptr)
			: std::streambuf()
			, std::ostream(this)
			, m_provider_ptr(provider_ptr)
			, m_parent_stream_ptr(parent_ptr)
			, m_report_ptr(report_ptr)
			, m_report_output_index(report_output_index)
			, m_user_report_ptr(user_report_ptr)
		{
		}

		DebugStream::~DebugStream()
		{
		}

		std::streamsize DebugStream::xsputn(const char* s, std::streamsize n)
		{
			// Ignore empty writes

			if (n == 0)
				return 0;

			// Check that the latest span entry is pointing to
			// our output index

			if (m_report_ptr->span_entries.empty()
				|| (m_report_ptr->span_entries.back().output_index != m_report_output_index))
			{
				m_report_ptr->span_entries.push_back(DebugReport::SpanEntry(m_report_output_index, m_report_ptr->data.size()));
			}

			// Append the data and increment the span size

			m_report_ptr->data.append(s, size_t(n));
			m_report_ptr->span_entries.back().data_length += size_t(n);

			return n;
		}

		std::streambuf::int_type DebugStream::overflow(std::streambuf::int_type ch)
		{
			if (ch != std::streambuf::traits_type::eof())
			{
				char ch_val = char(ch);

				return std::streambuf::int_type(xsputn(&ch_val, 1));
			}
			else
			{
				return std::streambuf::traits_type::eof();
			}
		}

    } // namespace bbox::debug
} // namespace bbox
