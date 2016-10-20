/**
 * @file
 *
 * Header file for the bbox::debug::DebugStream class.
 */

#ifndef __BBOX__DEBUG__DEBUG_STREAM_H__
#define __BBOX__DEBUG__DEBUG_STREAM_H__

#include <ostream>
#include <streambuf>
#include <bbox/debug/DebugReport.h>

namespace bbox
{
    // Forward declaration
    class DebugOutput;

    namespace debug
    {
        // Forward declaration
        class DebugProvider;

        /**
         * A reference counted stream owned by one or more
         * bbox::DebugOutput instances.
         */
        class DebugStream: public std::streambuf,
			               public std::ostream
        {
            friend class ::bbox::DebugOutput;
            friend class ::bbox::debug::DebugProvider;

			DebugStream() = delete;
			DebugStream(const DebugStream &) = delete;
			DebugStream &operator =(const DebugStream &) = delete;

		public:
			~DebugStream();

		protected:
			std::streamsize xsputn(const char* s, std::streamsize n) override;
			std::streambuf::int_type overflow(std::streambuf::int_type ch) override;

        private:

			DebugStream(DebugProvider *provider_ptr, DebugStream *parent_ptr, DebugReport::Report *report_ptr, size_t report_output_index, DebugReport *user_report_ptr);

            DebugProvider *m_provider_ptr;
			DebugStream *m_parent_stream_ptr;
			DebugReport::Report *m_report_ptr;
			size_t m_report_output_index;
			DebugReport *m_user_report_ptr;
        };
    }
}

#endif // __BBOX__DEBUG__DEBUG_STREAM_H__
