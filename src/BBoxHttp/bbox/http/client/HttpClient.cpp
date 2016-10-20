/**
 * @file
 *
 * Implementation for the bbox::http::client::HttpClient class.
 */

#include <bbox/http/client/HttpClient.h>
#include <bbox/http/client/ClientRequest.h>
#include <bbox/rt/OneShotWork.h>
#include <bbox/Assert.h>
#include <bbox/ScopedDebugIndent.h>

namespace bbox
{
    namespace http
    {
        namespace client
        {

            struct HttpClient::Pimpl
            {
                struct CompareRequests
                {
                    bool operator()(const ClientRequest &a, const ClientRequest &b) const
                    {
                        return std::less<ClientRequest::Pimpl *>()(a.m_pimpl.get(), b.m_pimpl.get());
                    }
                };

                HttpClient &m_parent;
                rt::OneShotWork m_check_stopped_work;
                std::set<ClientRequest, CompareRequests> m_current_requests;

                Pimpl(HttpClient &parent)
                    : m_parent(parent)
                    , m_check_stopped_work("check-stopped-work", parent, std::bind(&HttpClient::CheckStopped, &parent))
                    , m_current_requests()
                {
                }

                ~Pimpl()
                {
                    BBOX_ASSERT(m_current_requests.empty());
                }
            };

            HttpClient::HttpClient(const std::string &name, rt::Resource &parent)
                : rt::Resource(name, parent)
                , m_pimpl(std::make_unique<Pimpl>(*this))
            {
            }

            HttpClient::HttpClient(const std::string &name, rt::Service &parent)
                : rt::Resource(name, parent)
                , m_pimpl(std::make_unique<Pimpl>(*this))
            {
            }

            HttpClient::~HttpClient()
            {
            }

            void HttpClient::HandleStopping()
            {
                m_pimpl->m_check_stopped_work.Schedule();
            }

			void HttpClient::PrintState(bbox::DebugOutput &out) const
			{
				out.Format("Pending Requests: %d\n", m_pimpl->m_current_requests.size());
				ScopedDebugIndent indent(out, 4);

				for (const auto &request : m_pimpl->m_current_requests)
				{
					out.Format("\n");
					request.PrintState(out);
				}
			}

            void HttpClient::CheckStopped()
            {
                if ((GetLocalRunLevel() == rt::RunLevel::STOPPING)
                    && m_pimpl->m_current_requests.empty())
                {
                    NotifyStopped();
                    RequestStopAllChildren();
                }
            }

            void HttpClient::AddNewRequest(const ClientRequest &request)
            {
                BBOX_ASSERT(GetLocalRunLevel() == rt::RunLevel::RUNNING);

                auto insert_result = m_pimpl->m_current_requests.insert(request);

                BBOX_ASSERT(insert_result.second);
            }

            void HttpClient::RemoveCompletedRequest(const ClientRequest &request)
            {
                BBOX_ASSERT((GetLocalRunLevel() == rt::RunLevel::RUNNING)
                    || (GetLocalRunLevel() == rt::RunLevel::STOPPING));

                auto erase_result = m_pimpl->m_current_requests.erase(request);

                BBOX_ASSERT(erase_result == 1);

                if (GetLocalRunLevel() == rt::RunLevel::STOPPING)
                {
                    m_pimpl->m_check_stopped_work.Schedule();
                }
            }

        } // namespace bbox::http::client
    } // namespace bbox::http
} // namespace bbox
