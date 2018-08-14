/**
 * @file
 *
 * Implementation for the bbox::http::client::ClientRequest class.
 */

#include <bbox/http/client/ClientRequest.h>
#include <bbox/http/client/HttpClient.h>

#include <bbox/Assert.h>
#include <bbox/DebugOutput.h>
#include <bbox/Format.h>
#include <bbox/Base64.h>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace bbox
{
    namespace http
    {
        namespace client
        {

			using tcp = boost::asio::ip::tcp;
			namespace http = boost::beast::http;

			using request_type = http::request<http::string_body>;
			using response_type = http::response<http::string_body>;

			using request_ptr = std::unique_ptr<request_type>;
			using response_ptr = std::unique_ptr<response_type>;

            struct ClientRequest::Pimpl
            {
                HttpClient &m_client;
                std::string m_url;
				const char *m_progress;

                bool m_send_called;
                bool m_completed;
                CompleteCallback m_complete_callback;

                ClientRequest m_stored_ref;

				request_ptr m_request_ptr;
				response_ptr m_response_ptr;

                boost::asio::ip::tcp::resolver m_resolver;
				tcp::socket m_socket;
				boost::beast::flat_buffer m_buffer;

                Pimpl(HttpClient &client, const std::string &url)
                    : m_client(client)
                    , m_url(url)
					, m_progress("Constructed")
                    , m_send_called(false)
                    , m_completed(false)
                    , m_complete_callback()
                    , m_stored_ref()
                    , m_request_ptr(std::make_unique<request_type>())
					, m_response_ptr(std::make_unique<response_type>())
                    , m_resolver(m_client.GetProactor().GetIoService())
					, m_socket(m_client.GetProactor().GetIoService())
					, m_buffer()
				{
					m_request_ptr->method(http::verb::get);
                }

                ~Pimpl()
                {
                    BBOX_ASSERT(!m_stored_ref.m_pimpl);
                }

                void Send(ClientRequest &parent, CompleteCallback &&callback)
                {
                    // Save that we've had send called

                    BBOX_ASSERT(!m_send_called);

					m_progress = "Send Called";
					m_send_called = true;
                    m_complete_callback = std::move(callback);

                    // Mark us as pending

                    m_stored_ref = parent;
                    m_client.AddNewRequest(parent);

                    // Fill in the remainder of the request

                    {
                        std::string host, resource_and_query;
                        uint16_t port;

                        if (!DecodeUrl(host, port, resource_and_query))
                        {
                            // Invalid URL called - just post a failure

                            m_client.GetProactor().Post(std::bind(
                                &Pimpl::Complete,
                                this,
                                bbox::Error(boost::system::errc::invalid_argument)));
                        }
                        else
                        {
                            m_request_ptr->set(http::field::host, host);
							m_request_ptr->set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                            m_request_ptr->target(resource_and_query);

                            // Start resolving the host name

                            m_resolver.async_resolve(
                                boost::asio::ip::tcp::resolver::query(host, ""),
                                std::bind(
                                    &Pimpl::HandleResolveComplete,
                                    this,
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    port));
                        }
                    }
                }

                bool DecodeUrl(std::string &host, uint16_t &port, std::string &resource_and_query)
                {
                    // TODO - needs to be much better!!!

                    if (m_url.substr(0, 7) != "http://")
                        return false;

                    size_t resource_pos = m_url.find('/', 7);

                    if (resource_pos == 7)
                    {
                        // No host
                        return false;
                    }
                    else if (resource_pos == std::string::npos)
                    {
                        // No resource - use root path
                        host = m_url.substr(7);
                        resource_and_query = "/";
                    }
                    else
                    {
                        host = m_url.substr(7, resource_pos - 7);
                        resource_and_query = m_url.substr(resource_pos);
                    }

                    // Now, see if the host has a port number specified
                    size_t port_sep_pos = host.rfind(':');

                    if (port_sep_pos == std::string::npos)
                    {
                        // No port specified - host is full string
                        port = 80;
                    }
                    else
                    {
                        std::string port_str = host.substr(port_sep_pos + 1);
                        host = host.substr(0, port_sep_pos);

                        try
                        {
                            unsigned long plong = std::stoul(port_str, 0, 10);
                            if (plong > 65535)
                                return false;
                            port = (uint16_t)plong;
                        }
                        catch (...)
                        {
                            return false;
                        }
                    }
                    return true;
                }

                void HandleResolveComplete(
                    const boost::system::error_code &ec,
					tcp::resolver::results_type results,
                    uint16_t port)
                {
					m_progress = "Resolve Completed";

                    if (ec)
                    {
                        Complete(ec);
                    }
                    else
                    {
                        // Try and connect to the host

                        // TODO - debug enable
                        //bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Activity);
                        //if (out)
                        //{
                        //    out.Format("HTTP Client: Resolve response %s => %s port %d\n",
                        //        iterator->host_name(),
                        //        iterator->endpoint().address().to_string(),
                        //        iterator->endpoint().port());
                        //}

						boost::asio::async_connect(
							m_socket,
							results.begin(),
							results.end(),
							std::bind(
                                &Pimpl::HandleConnectComplete,
                                this,
                                std::placeholders::_1));
                    }
                }

                void HandleConnectComplete(const boost::system::error_code &ec)
                {
					m_progress = "Connect Completed";

                    if (ec)
                    {
                        Complete(ec);
                    }
                    else
                    {
						http::async_write(
							m_socket,
							*m_request_ptr,
							std::bind(
                                &Pimpl::HandleRequestWritten,
                                this,
                                std::placeholders::_1,
								std::placeholders::_2));
                    }
                }

                void HandleRequestWritten(const boost::system::error_code &ec, std::size_t bytes_transferred)
                {
					boost::ignore_unused(bytes_transferred);

					m_progress = "Request Written";

                    if (ec)
                    {
                        Complete(ec);
                    }
                    else
                    {
						http::async_read(
							m_socket,
							m_buffer,
							*m_response_ptr,
							std::bind(
                                &Pimpl::HandleResponseRead,
                                this,
                                std::placeholders::_1,
                                std::placeholders::_2));
                    }
                }

                void HandleResponseRead(const boost::system::error_code &ec, std::size_t bytes_transferred)
                {
					boost::ignore_unused(bytes_transferred);

					m_progress = "Response Read";

                    if (ec)
                    {
                        Complete(ec);
                    }
                    else
                    {
                        // Wow - it actually succeeded!

                        Complete(bbox::Error());
                    }
                }

                void Complete(bbox::Error error)
                {
                    BBOX_ASSERT(!m_completed);
                    BBOX_ASSERT(m_stored_ref.m_pimpl);
                    BBOX_ASSERT(m_stored_ref.m_pimpl.get() == this);

					if (error)
						m_progress = "Completed with errors";
					else
						m_progress = "Completed successfully";

					m_completed = true;

                    // Call the handler if provided

                    if (m_complete_callback)
                    {
						response_ptr response;

						if (!error)
							response = std::move(m_response_ptr);

                        m_complete_callback(error, ClientResponse(std::move(response)));
                    }

                    // Clear us down

					boost::system::error_code ec;

					m_socket.shutdown(tcp::socket::shutdown_both, ec);

                    m_client.RemoveCompletedRequest(m_stored_ref);

                    ClientRequest copy{ std::move(m_stored_ref) };

                    m_stored_ref.m_pimpl = nullptr;

                    // Copy will be cleared down as this method returns,
                    // causing us to be destroyed (if the user has no more
                    // references to us).
                }
            };

            ClientRequest::ClientRequest()
            {
            }

            ClientRequest::ClientRequest(HttpClient &client, const std::string &url)
                : m_pimpl(std::make_shared<Pimpl>(client, url))
            {
            }

			void ClientRequest::PrintState(bbox::DebugOutput &out) const
			{
				if (!m_pimpl)
				{
					out.Format("NULL\n");
				}
				else
				{
					out.Format("Progress: %s\n", m_pimpl->m_progress);
					out.Format("Send Called: %s\n", m_pimpl->m_send_called);
					out.Format("Completed: %s\n", m_pimpl->m_completed);
                    out.Format("URL: ");
                    {
                        bbox::DebugOutput url_out(BBOX_FUNC, out, DebugOutput::Mime_Text_Url);
                        url_out << m_pimpl->m_url;
                    }
                    out << std::endl;
					out.Format("Method: %s\n", m_pimpl->m_request_ptr->method());
					out.Format("Content Length: %d\n", m_pimpl->m_request_ptr->body().size());

					for (const auto &field : *m_pimpl->m_request_ptr)
					{
						out.Format("Header: %s: %s", field.name(), field.value());
					}
				}
			}

            void ClientRequest::SetMethod(Method method)
            {
                BBOX_ASSERT(m_pimpl);
                BBOX_ASSERT(!m_pimpl->m_send_called);

                m_pimpl->m_request_ptr->method(method);
            }

            void ClientRequest::SetHeader(const std::string &header, const std::string &value)
            {
                BBOX_ASSERT(m_pimpl);
                BBOX_ASSERT(!m_pimpl->m_send_called);

                m_pimpl->m_request_ptr->set(header, value);
            }

            void ClientRequest::SetHeader_BasicAuthentication(const std::string &user_name, const std::string &password)
            {
                std::string combined = bbox::Format("%s:%s", user_name, password);
                std::string base64 = bbox::Base64::Encode(combined);

                SetHeader("Authorization", bbox::Format("Basic %s", base64));
            }

            void ClientRequest::SetContent(const std::string &content)
            {
                BBOX_ASSERT(m_pimpl);
                BBOX_ASSERT(!m_pimpl->m_send_called);

                m_pimpl->m_request_ptr->body() = content;
				m_pimpl->m_request_ptr->content_length(content.size());
            }

            void ClientRequest::Send(CompleteCallback &&complete_callback)
            {
                BBOX_ASSERT(m_pimpl);

                m_pimpl->Send(*this, std::move(complete_callback));
            }

        } // namespace bbox::http::client
    } // namespace bbox::http
} // namespace bbox
