/**
 * @file
 *
 * Implementation file for the bbox::http::debug::HttpDebugWebsite class.
 */

#include <bbox/http/debug/HttpDebugWebsite.h>
#include <bbox/http/debug/Resources.h>

#include <bbox/debug/DebugQuery.h>
#include <bbox/debug/DebugReport.h>

#include <bbox/http/server/HttpServer.h>

#include <bbox/enc/api/Describe.h>
#include <bbox/enc/FromXml.h>
#include <bbox/enc/ToXml.h>
#include <bbox/enc/ToDebugString.h>

#include <boost/algorithm/string/replace.hpp>

namespace bbox {
    namespace http {
        namespace debug {

            HttpDebugWebsite::HttpDebugWebsite(const std::string &name, bbox::rt::Service &parent, bbox::http::server::HttpServer &server)
                : bbox::rt::Service(name, parent)
                , m_api_request_handler("api-request-handler", *this, server)
                , m_reources_request_handler("resource-request-handler", *this, server)
                , m_debug_enables()
                , m_pending_timer("pending-timer", *this)
                , m_next_id(1)
                , m_pending_entries()
            {
                SetThisDependantOn(server);

                m_pending_timer.SetHandler(std::bind(&HttpDebugWebsite::HandlePendingTimerExpiry, this));

                m_method_set = bbox::enc::api::MethodSet({
                    bbox::enc::api::describe::Method(
                        "query_state",
                        "Queries for the state of one or more objects",
                        bbox::enc::api::describe::ParamPack(
                            bbox::enc::api::describe::Param<
                                std::string,
                                bbox::enc::api::describe::Input>(
                                "query",
                                "The query string"),
                            bbox::enc::api::describe::Param<
                                std::vector<api::QueryResultEntry>,
                                bbox::enc::api::describe::Output>(
                                "results",
                                "The returned results, or empty if no matching entries were found")
                        ),
                        this,
                        &HttpDebugWebsite::HandleApiQueryStateRequest),
                    bbox::enc::api::describe::Method(
                        "get_output",
                        "Gets any pending output from the application",
                        bbox::enc::api::describe::ParamPack(
                            bbox::enc::api::describe::Param<
                                uint64_t,
                                bbox::enc::api::describe::Input>(
                                "req_id",
                                "The request ID - 0 to start, or the next_id result from a previous call"),
                            bbox::enc::api::describe::Param<
                                std::vector<std::string>,
                                bbox::enc::api::describe::Input>(
                                "debug_enables",
                                "The set of debug enables to turn on"),
                            bbox::enc::api::describe::Param<
                                uint64_t,
                                bbox::enc::api::describe::Output>(
                                "next_id",
                                "The next req_id that should be passed to get a continuous stream of output"),
                            bbox::enc::api::describe::Param<
                                std::vector<api::DebugReport>,
                                bbox::enc::api::describe::Output>(
                                "reports",
                                "The list of reports output since the last query")
                        ),
                        this,
                        &HttpDebugWebsite::HandleApiGetOutput),
                });
            }

            HttpDebugWebsite::~HttpDebugWebsite()
            {
            }

            void HttpDebugWebsite::HandleStarting()
            {
                m_reources_request_handler.Login("/debug", std::bind(&HttpDebugWebsite::HandleResourceRequest, this, std::placeholders::_1));
                m_api_request_handler.Login("/debug/api", std::bind(&HttpDebugWebsite::HandleApiRequest, this, std::placeholders::_1));

                NotifyStarted();
            }
            
            void HttpDebugWebsite::HandleStopping()
            {
                m_api_request_handler.Logout();
                m_reources_request_handler.Logout();
                m_pending_timer.Cancel();
                m_pending_entries.clear();

                RequestStopAllChildren();
                NotifyStopped();
            }

            void HttpDebugWebsite::PrintState(bbox::DebugOutput &out) const
            {
                out.Format("Location: ");
                {
                    bbox::DebugOutput link_out(BBOX_FUNC, out, bbox::DebugOutput::Mime_Text_Url);
                    link_out.Format("/debug");
                }
                out << std::endl;

                out.Format("Debug Enables: %s\n", bbox::enc::ToDebugString(m_debug_enables));

                if (!m_pending_entries.empty())
                {
                    out.Format("Pending entries:\n");

                    for (const auto &entry : m_pending_entries)
                    {
                        out.Format("   %d: counter=%d, num_pending=%d, debug_enables=%s\n",
                            entry.first,
                            entry.second.counter,
                            entry.second.pending_reports.size(),
                            bbox::enc::ToDebugString(entry.second.debug_enables));
                    }
                }
            }

            void HttpDebugWebsite::OutputToTarget(bbox::debug::DebugReport report)
            {
                // We only handle debug while in the running state

                if (GetLocalRunLevel() == bbox::rt::RunLevel::RUNNING)
                {
                    for (auto &entry : m_pending_entries)
                    {
                        entry.second.pending_reports.push_back(report);
                    }
                }
            }

            void HttpDebugWebsite::HandlePendingTimerExpiry()
            {
                bool need_update_debug_enables = false;

                auto it = m_pending_entries.begin();
                while (it != m_pending_entries.end())
                {
                    auto cur = it;
                    ++it;

                    if (cur->second.counter > 0)
                        --cur->second.counter;

                    if (cur->second.counter == 0)
                    {
                        if (!cur->second.debug_enables.empty())
                            need_update_debug_enables = true;

                        m_pending_entries.erase(cur);
                    }
                }

                if (m_pending_entries.empty())
                {
                    m_pending_timer.Cancel();
                }

                if (need_update_debug_enables)
                {
                    UpdateOverallDebugEnables();
                }
            }

            void HttpDebugWebsite::HandleResourceRequest(http::Request &request)
            {
                std::string resource = request.GetResource();

                if ((resource == "/debug")
                    || (resource == "/debug/")
                    || (resource.substr(0, 13) == "/debug/state/"))
                {
                    request.RespondWithResourceOrNotFoundError(g_resource_files, "/debug/index.html");
                    return;
                }

                request.RespondWithResourceOrNotFoundError(g_resource_files);
            }

            void HttpDebugWebsite::HandleApiRequest(http::Request &request)
            {
                if (request.GetMethod() != http::Request::Method::post)
                {
                    request.RespondWithMethodNotAllowedError("POST");
                    return;
                }

                std::string method_name = request.GetResource().substr(11);

                if (!m_method_set.HasMethodWithName(method_name))
                {
                    request.RespondWithNotFoundError();
                    return;
                }

                const bbox::enc::api::Method &method = m_method_set.GetMethodByName(method_name);

                bbox::enc::FromXml from_xml(request.GetContent());

                boost::any params_tuple = method.GetParamPack().CreateDefaultConstructedTuple();

                from_xml.StartNamedItem("params");
                method.GetParamPack().TupleFromTextFormat(from_xml, true, params_tuple);
                from_xml.CompleteNamedItem();
                from_xml.EnsureAllDecoded();

                if (from_xml.HasError())
                {
                    request.RespondWithBadRequestError();
                    return;
                }

                bbox::Error error = method.Call(params_tuple);

                if (error)
                {
                    request.RespondWithServerError(error.ToString());
                    return;
                }

                tinyxml2::XMLDocument result_doc;
                bbox::enc::ToXml to_xml(result_doc, "results");

                method.GetParamPack().TupleToTextFormat(to_xml, false, params_tuple);

                tinyxml2::XMLPrinter printer;
                result_doc.Print(&printer);

                bbox::http::Response response(request);

                response.SetHeader_ContentType("text/xml");
                response.SetHeader_NoCache();
                response.SetResponse_OK();
                response.SetContent(std::string(printer.CStr()));

#ifdef _DEBUG
                response.SetHeader("Access-Control-Allow-Origin", "*");
#endif

                response.Send();
            }

            bbox::Error HttpDebugWebsite::HandleApiQueryStateRequest(const std::string &query, std::vector<api::QueryResultEntry> &results)
            {
                std::vector<bbox::debug::DebugQueryResult> query_results =
                    bbox::debug::DebugQuery::DoQuery(query);

                results.clear();
                results.reserve(query_results.size());

                for (const bbox::debug::DebugQueryResult &entry : query_results)
                {
                    api::QueryResultEntry new_entry;

                    new_entry.path = entry.full_path;
                    new_entry.parent = entry.parent_path;
                    for (const bbox::debug::DebugChildEntry &child : entry.children)
                    {
                        new_entry.children.emplace_back(child.name, child.path, child.short_description);
                    }

                    ConvertDebugReport(entry.report, new_entry.debug_report);

                    results.push_back(std::move(new_entry));
                }

                return bbox::Error();
            }

            bbox::Error HttpDebugWebsite::HandleApiGetOutput(uint64_t req_id, const std::vector<std::string> &debug_enables, uint64_t &next_id, std::vector<api::DebugReport> &reports)
            {
                next_id = m_next_id;
                ++m_next_id;
                
                reports.clear();

                bool enables_changed = false;
                std::set<std::string> new_debug_enables;
                for (const std::string &e : debug_enables)
                    new_debug_enables.insert(e);

                if (req_id != 0)
                {
                    auto it = m_pending_entries.find(req_id);

                    if (it == m_pending_entries.end())
                    {
                        return boost::system::errc::not_connected;
                    }
                    else
                    {
                        PendingEntry &entry = it->second;

                        reports.resize(entry.pending_reports.size());

                        for (size_t i = 0; i < entry.pending_reports.size(); ++i)
                        {
                            ConvertDebugReport(entry.pending_reports[i], reports[i]);
                        }

                        enables_changed = (new_debug_enables != entry.debug_enables);

                        m_pending_entries.erase(it);
                    }
                }
                else
                {
                    // New query - enables have changed if there's any listed

                    enables_changed = !new_debug_enables.empty();
                }

                auto &entry = m_pending_entries[next_id];
                entry.counter = TIMEOUT_SECONDS;
                entry.pending_reports.clear();
                entry.debug_enables = std::move(new_debug_enables);

                if (enables_changed)
                {
                    UpdateOverallDebugEnables();
                }

                if (!m_pending_timer.IsRunning())
                {
                    m_pending_timer.StartPeriodic(bbox::rt::TimeSpan::Seconds(1));
                }

                return bbox::Error();
            }

            void HttpDebugWebsite::UpdateOverallDebugEnables()
            {
                std::set<std::string> new_debug_enables;

                for (const auto &entry : m_pending_entries)
                {
                    new_debug_enables.insert(entry.second.debug_enables.begin(), entry.second.debug_enables.end());
                }

                if (new_debug_enables != m_debug_enables)
                {
                    m_debug_enables = new_debug_enables;

                    UpdateDebugEnables(std::move(new_debug_enables));
                }
            }

            void HttpDebugWebsite::ConvertDebugReport(const bbox::debug::DebugReport &src, api::DebugReport &dest)
            {
                std::stringstream stream;

                // Print out the report data in the correct format

                const std::string &data = src.Data();
                const std::vector<bbox::debug::DebugReport::OutputEntry> &output_entries = src.OutputEntries();
                const std::vector<bbox::debug::DebugReport::SpanEntry> &span_entries = src.SpanEntries();

                for (const bbox::debug::DebugReport::SpanEntry &span : span_entries)
                {
                    const bbox::debug::DebugReport::OutputEntry &output = output_entries[span.output_index];

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
                        stream << "<a href='" << quoted_url << "'>" << quoted_url << "</a>";
                    }
                    break;

                    case DebugOutput::Mime_Text_Debug_Full_Path:
                    {
                        std::string path_html = HtmlEscape(data.substr(span.data_start_index, span.data_length));
                        std::string path_uri = UriEscapeExceptForwardSlash(data.substr(span.data_start_index, span.data_length));
                        stream << "<a href='/debug/state" << path_uri <<
                            "' onclick='return app.pushState_QueryState(\"" << path_html << "\");'>"
                            << path_html << "</a>";
                    }
                    break;
                    }
                }

                dest.contents_html = stream.str();
            }

            std::string HttpDebugWebsite::HtmlEscape(std::string &&str)
            {
                std::string result(std::move(str));
                boost::replace_all(result, "&", "&amp;");
                boost::replace_all(result, "<", "&lt;");
                boost::replace_all(result, ">", "&gt;");
                boost::replace_all(result, "\"", "&quot;");
                boost::replace_all(result, "\'", "&apos;");
                return result;
            }

            std::string HttpDebugWebsite::UriEscapeExceptForwardSlash(std::string &&str)
            {
                static const char constexpr lookup[] = "0123456789ABCDEF";

                std::stringstream stream;

                for (char ch : str)
                {
                    if (std::isalnum(ch)
                        || (ch == '-')
                        || (ch == '_')
                        || (ch == '.')
                        || (ch == '~')
                        || (ch == '/'))
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

        } // namespace bbox::http::debug
    } // namespace bbox::http
} // namespace bbox
