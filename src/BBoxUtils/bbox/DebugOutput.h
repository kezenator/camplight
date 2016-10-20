/**
 * @file
 *
 * Header file for the bbox::DebugOutput class.
 */

#ifndef __BBOX__DEBUG_OUTPUT_H__
#define __BBOX__DEBUG_OUTPUT_H__

#include <ostream>
#include <bbox/Format.h>

#define BBOX_FUNC __func__ 

namespace bbox
{
	// Forward declarations
	namespace debug
	{
		class DebugReport;
		class DebugStream;
	} // namespace bbox::debug

    /**
     * A class for generating debug output
     * that provides advanced features.
     *
     * It (will) provides indenting, different types
     * (e.g. HTML, Graphviz, sequence diagrams...).
     */
    class DebugOutput
    {
        DebugOutput() = delete;
        DebugOutput(const DebugOutput &other) = delete;
		DebugOutput &operator =(const DebugOutput &other) = delete;

    public:
        /**
         * A set of generic debug enable flags.
         */
        enum E_ENABLE
        {
			Never,
            Activity,
            Testing,
            Error,
        };

        /**
         * The set of supported MIME types.
         */
        enum E_MIME_TYPE
        {
            Mime_Text_Plain,
            Mime_Text_Html,
            Mime_Text_Url,
            Mime_Text_Debug_Full_Path,
        };

        /**
         * Creates a new stack-based DebugOutput for
		 * outputting (if enabled) async debug from
		 * the operation of the programme.
         *
         * It should be created on the stack local to a function, for example:
         *
		 * @code
		 * bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Testing);
		 * @endcode
		 *
		 * All access must be checked to see if it's enabled:
		 *
		 * @code
		 * if (out)
		 * {
		 *     out.Format("foo=%d\n", 123);
		 *     out << "bar=" << 456 << std::endl;
		 * }
		 * @endcode
		 *
		 * The output of DebugOutputs like this is
		 * automatically sent to the registered targets.
         */
		DebugOutput(const char *func, E_ENABLE enable)
			: m_stream_ptr(nullptr)
		{
			if (enable != Never)
			{
				Enable(func, enable);
			}
		}

		/**
		 * Creates a DebugOutput for collecting debug
		 * as requested by the user.
		 *
		 * The life-time of this output is controlled by 
		 * the application, and the application must
		 * explicitly handle the report - it is not
		 * automatically sent to the registered targets.
		 *
		 * The report is filled in as the destructor
		 * is called - as such you cannot access the report
		 * until the DebugOutput is destroyed.
		 *
		 * For synchronous operations, you can create
		 * it on the stack:
		 *
		 * @code
		 * bbox::debug::DebugReport report;
		 * {
		 *     DebugOutput out(BBOX_FUNC, report, "My Operation");
		 *     out.Format("blah blah blah...\n");
		 * }
		 * HandleReport(report);
		 * @endcode
		 *
		 * For long-running or asynchronous operations,
		 * try using a std::unique_ptr to control
		 * the lifetime:
		 *
		 * @code
		 * class MyOperation
		 * {
		 *     bbox::debug::DebugReport report;
		 *     std::unique_ptr<bbox::DebugOutput> output_ptr;
		 *
		 *     MyOperation()
		 *     {
		 *         output_ptr = std::make_unique<bbox::DebugOutput>(
		 *             BBOX_FUNC, report, "MyOperation");
		 *     }
		 *
		 *     void HandleProgress()
		 *     {
		 *         output_ptr->Format("blah blah blah...\n");
		 *     }
		 *
		 *     void HandleComplete()
		 *     {
		 *         output_ptr.reset();
		 *         HandleReport(report);
		 *     }
		 * }
		 * @endcode
		 */
		DebugOutput(const char *func, bbox::debug::DebugReport &report, std::string &&reason);

        /**
         * Constructs a child debug-output that allows debug output.
         * to be written in a different format.
         */
        DebugOutput(const char *func, DebugOutput &parent, E_MIME_TYPE mime_type);

        /**
         * Destructor.
         */
        ~DebugOutput()
        {
            if (m_stream_ptr)
            {
				DestroyStream();
            }
        }

        /**
         * Determines if it's enabled.
         * This must always be called before outputting information.
         */
        explicit operator bool() const { return m_stream_ptr != nullptr; }

		/**
		 * Enables the debug output, if the specified enable is active.
		 *
		 * Makes no change if the debug output is already enabled.
		 */
		void Enable(const char *func, E_ENABLE enable);

		/**
         * Outputs text.
         */
        template <typename... Rest>
        inline void Format(const char *format, Rest... rest)
        {
            bbox::Format(ostream(), format, rest...);
        }

        /**
        * Outputs text.
        */
        void Format(const char *format)
        {
            bbox::Format(ostream(), format);
        }

        std::ostream &ostream();

		/**
		 * Increments the indent by the specified amount.
		 */
		void IncIndent(size_t change);

		/**
		 * Decrements the indent by the specified amount.
		 */
		void DecIndent(size_t change);

		/**
		 * Sets the indent.
		 */
		void SetIndent(size_t indent);

		static const char *c_str(E_ENABLE enable);

        template <typename Type>
        std::ostream &operator <<(Type &t)
        {
            std::ostream &ostm = ostream();
            ostm << t;
            return ostm;
        }

        std::ostream &operator <<(std::ostream& (*pf)(std::ostream&))
        {
            std::ostream &ostm = ostream();
            ostm << pf;
            return ostm;
        }

        std::ostream &operator <<(std::ios& (*pf)(std::ios&))
        {
            std::ostream &ostm = ostream();
            ostm << pf;
            return ostm;
        }

        std::ostream &operator <<(std::ios_base& (*pf)(std::ios_base&))
        {
            std::ostream &ostm = ostream();
            ostm << pf;
            return ostm;
        }

    private:

		void DestroyStream();

        debug::DebugStream *m_stream_ptr;
    };

} // namespace bbox

#endif // __BBOX__DEBUG_OUTPUT_H__
