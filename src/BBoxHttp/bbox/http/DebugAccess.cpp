/**
 * @file
 *
 * Implementation for the bbox::http::DebugAccess class.
 */

#include <bbox/http/DebugAccess.h>
#include <bbox/http/Response.h>

#include <bbox/Assert.h>

#include <bbox/debug/DebugQueryResult.h>
#include <bbox/debug/DebugQuery.h>

namespace bbox
{
    namespace http
    {

        void DebugAccess::HandleDebugRequest(Request &request, const std::string &debug_root_path)
        {
            BBOX_ASSERT(request);

            std::vector<bbox::debug::DebugQueryResult> results =
                bbox::debug::DebugQuery::DoQuery(request.GetQuery("path"));

            if (results.empty())
            {
                request.RespondWithNotFoundError();
            }
            else
            {
                std::stringstream stream;

                stream << "<!DOCTYPE html>" << std::endl;
                stream << "<html><head><title>Debug</title></head><body><pre>";

                for (const bbox::debug::DebugQueryResult &result : results)
                {
                    stream << "Result: " << result.full_path << std::endl;

                    for (const std::string &child_name : result.children)
                    {
                        std::string child_path = result.full_path;

                        if (!child_path.empty()
                            && (child_path.back() != '/'))
                        {
                            child_path.push_back('/');
                        }
                        child_path.append(child_name);

                        stream << "Child: <a href=\"?path="
                            << child_path
                            << "\">" << child_name << "</a>" << std::endl;
                    }

                    stream << "Contents:" << std::endl;

                    // Print out the report data in the correct format

                    const std::string &data = result.report.Data();
                    const std::vector<debug::DebugReport::OutputEntry> &output_entries = result.report.OutputEntries();
                    const std::vector<debug::DebugReport::SpanEntry> &span_entries = result.report.SpanEntries();

                    for (const debug::DebugReport::SpanEntry &span : span_entries)
                    {
                        const debug::DebugReport::OutputEntry &output = output_entries[span.output_index];

                        switch (output.mime_type)
                        {
                        case DebugOutput::Mime_Text_Plain:
                            stream << HtmlEscape(data.substr(span.data_start_index, span.data_length));
                            break;

                        case DebugOutput::Mime_Text_Html:
                            stream.write(data.c_str() + span.data_start_index, span.data_length);
                            break;

                        case DebugOutput::Mime_Text_Url:
                            {
                                std::string quoted_url = HtmlEscape(data.substr(span.data_start_index, span.data_length));
                                stream << "<a href=\"" << quoted_url << "\">" << quoted_url << "</a>";
                            }
                            break;

                        case DebugOutput::Mime_Text_Debug_Full_Path:
                            {
                                std::string path_html = HtmlEscape(data.substr(span.data_start_index, span.data_length));
                                std::string path_uri = UriEscape(data.substr(span.data_start_index, span.data_length));
                                stream << "<a href=\"?path=" << path_uri << "\">" << path_html << "</a>";
                            }
                            break;
                        }
                    }
                }

                stream << "</pre></body></html>";

                Response(request)
                    .SetResponse_OK()
                    .SetHeader_NoCache()
                    .SetHeader_ContentType("text/html")
                    .SetContent(stream.str())
                    .Send();
            }
        }

        std::string DebugAccess::HtmlEscape(std::string &&str)
        {
            std::string result(std::move(str));
            boost::replace_all(result, "&", "&amp;");
            boost::replace_all(result, "<", "&lt;");
            boost::replace_all(result, ">", "&gt;");
            boost::replace_all(result, "\"", "&quot;");
            boost::replace_all(result, "\'", "&apos;");
            return result;
        }

        std::string DebugAccess::UriEscape(std::string &&str)
        {
            static const char constexpr lookup[] = "0123456789ABCDEF";

            std::stringstream stream;

            for (char ch : str)
            {
                if (std::isalnum(ch)
                    || (ch == '-')
                    || (ch == '_')
                    || (ch == '.')
                    || (ch == '~'))
                {
                    stream << ch;
                }
                else
                {
                    stream << '%'
                        << lookup[(ch >> 4) & 0x0F]
                        << lookup[ch & 0x0F];
                }
            }

            return stream.str();
        }

    } // namespace bbox::http
} // namespace bbox
