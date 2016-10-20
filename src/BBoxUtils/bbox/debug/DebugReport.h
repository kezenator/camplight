/**
 * @file
 *
 * Header file for the bbox::debug::DebugReport class.
 *
 */

#ifndef __BBOX__DEBUG__DEBUG_REPORT_H__
#define __BBOX__DEBUG__DEBUG_REPORT_H__

#include <memory>
#include <string>
#include <vector>

#include <bbox/DebugOutput.h>

namespace bbox
{
	namespace debug
	{
		// Forward declaration
		class DebugProvider;

		/**
		 * A cheap reference counted handle to
		 * a collect of debug generated from a single
		 * DebugOutput instance (or hierarchy of instances).
		 */
		class DebugReport
		{
			friend class ::bbox::debug::DebugProvider;

		public:

			struct OutputEntry
			{
				std::string function;
				std::string reason;
                DebugOutput::E_MIME_TYPE mime_type;

				OutputEntry(std::string &&_function, std::string &&_reason, DebugOutput::E_MIME_TYPE _mime_type)
					: function(std::move(_function))
					, reason(std::move(_reason))
					, mime_type(_mime_type)
				{
				}
			};

			struct SpanEntry
			{
				size_t output_index;
				size_t data_start_index;
				size_t data_length;

				SpanEntry(size_t _output_index, size_t _data_start_index)
					: output_index(_output_index)
					, data_start_index(_data_start_index)
					, data_length(0)
				{
				}
			};

			struct Report
			{
				std::string data;
				std::vector<OutputEntry> output_entries;
				std::vector<SpanEntry> span_entries;
			};

			DebugReport() = default;
			DebugReport(const DebugReport &) = default;
			DebugReport(DebugReport &&) = default;
			~DebugReport() = default;
			DebugReport &operator =(const DebugReport &) = default;
			DebugReport &operator =(DebugReport &&) = default;

            const std::string &Data() const
            {
                return m_report_ptr->data;
            }

            const std::vector<DebugReport::OutputEntry> &OutputEntries() const
            {
                return m_report_ptr->output_entries;
            }

            const std::vector<DebugReport::SpanEntry> &SpanEntries() const
            {
                return m_report_ptr->span_entries;
            }

		private:

			explicit DebugReport(Report *report_ptr)
				: m_report_ptr(report_ptr)
			{
			}

			std::shared_ptr<Report> m_report_ptr;
		};

	} // namespace bbox::debug
} // namespace bbox

#endif // __BBOX__DEBUG__DEBUG_REPORT_H__
