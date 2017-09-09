/**
 * @file
 *
 * Header file for the bbox::http::debug::HttpDebugWebsite class.
 */

#include <bbox/rt/Service.h>
#include <bbox/rt/Timer.h>
#include <bbox/http/server/RequestHandler.h>
#include <bbox/debug/DebugTarget.h>

#include <bbox/enc/api/MethodSet.h>
#include <bbox/http/debug/api/QueryResultEntry.h>

namespace bbox {
    namespace http {
        namespace debug {

            class HttpDebugWebsite: public bbox::rt::Service, private bbox::debug::DebugTarget
            {
            public:
                HttpDebugWebsite(const std::string &name, bbox::rt::Service &parent, bbox::http::server::HttpServer &server);
                ~HttpDebugWebsite();

            private:
                // Implements bbox::rt::Service
                void HandleStarting() override;
                void HandleStopping() override;
                void PrintState(bbox::DebugOutput &out) const override;

                // Implements bbox::debug::DebugTarget
                void OutputToTarget(bbox::debug::DebugReport report) override;

                void HandlePendingTimerExpiry();

                void HandleResourceRequest(http::Request &request);
                void HandleApiRequest(http::Request &request);

                bbox::Error HandleApiQueryStateRequest(const std::string &query, std::vector<api::QueryResultEntry> &results);
                bbox::Error HandleApiGetOutput(uint64_t req_id, const std::vector<std::string> &debug_enables, uint64_t &next_id, std::vector<api::DebugReport> &reports);

                void UpdateOverallDebugEnables();

                static void ConvertDebugReport(const bbox::debug::DebugReport &src, api::DebugReport &dest); 
                static std::string HtmlEscape(std::string &&str);
                static std::string UriEscapeExceptForwardSlash(std::string &&str);

                static constexpr size_t TIMEOUT_SECONDS = 5;

                server::RequestHandler m_api_request_handler;
                server::RequestHandler m_reources_request_handler;

                bbox::enc::api::MethodSet m_method_set;

                std::set<std::string> m_debug_enables;

                struct PendingEntry
                {
                    size_t counter;
                    std::set<std::string> debug_enables;
                    std::vector<bbox::debug::DebugReport> pending_reports;
                };

                bbox::rt::Timer m_pending_timer;

                uint64_t m_next_id;
                std::map<uint64_t, PendingEntry> m_pending_entries;
            };

        } // namespace bbox::http::debug
    } // namespace bbox::http
} // namespace bbox
