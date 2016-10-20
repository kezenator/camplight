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

#include <pion/tcp/connection.hpp>
#include <pion/http/request.hpp>
#include <pion/http/response.hpp>

#include <pion/http/request_writer.hpp>
#include <pion/http/response_reader.hpp>

#include <boost/asio/ip/tcp.hpp>

namespace bbox
{
    namespace http
    {
        namespace client
        {

            struct ClientRequest::Pimpl
            {
                HttpClient &m_client;
                std::string m_url;
				const char *m_progress;

                bool m_send_called;
                bool m_completed;
                CompleteCallback m_complete_callback;

                ClientRequest m_stored_ref;

                pion::http::request_ptr m_request_ptr;
                pion::tcp::connection_ptr m_conn_ptr;
                pion::http::request_writer_ptr m_writer_ptr;

                pion::http::response_reader_ptr m_reader_ptr;
                pion::http::response_ptr m_response_ptr;

                boost::asio::ip::tcp::resolver m_resolver;

                Pimpl(HttpClient &client, const std::string &url)
                    : m_client(client)
                    , m_url(url)
					, m_progress("Constructed")
                    , m_send_called(false)
                    , m_completed(false)
                    , m_complete_callback()
                    , m_stored_ref()
                    , m_request_ptr(new pion::http::request())
                    , m_conn_ptr(new pion::tcp::connection(m_client.GetProactor().GetIoService()))
                    , m_writer_ptr()
                    , m_reader_ptr()
                    , m_response_ptr()
                    , m_resolver(m_client.GetProactor().GetIoService())
                {
                    m_request_ptr->set_method("GET");
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
                                bbox::Error(boost::system::errc::invalid_argument),
                                pion::http::response_ptr()));
                        }
                        else
                        {
                            m_request_ptr->add_header("Host", host);
                            m_request_ptr->set_resource(resource_and_query);

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
                    boost::asio::ip::tcp::resolver::iterator iterator,
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

                        m_conn_ptr->set_lifecycle(pion::tcp::connection::LIFECYCLE_CLOSE);

                        m_conn_ptr->async_connect(
                            boost::asio::ip::tcp::endpoint(
                                iterator->endpoint().address(),
                                port),
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
                        m_writer_ptr = pion::http::request_writer::create(
                            m_conn_ptr,
                            m_request_ptr,
                            std::bind(
                                &Pimpl::HandleRequestWritten,
                                this,
                                std::placeholders::_1));

                        m_writer_ptr->send();
                    }
                }

                void HandleRequestWritten(const boost::system::error_code &ec)
                {
					m_progress = "Request Written";

                    if (ec)
                    {
                        Complete(ec);
                    }
                    else
                    {
                        m_reader_ptr = pion::http::response_reader::create(
                            m_conn_ptr,
                            *m_request_ptr,
                            std::bind(
                                &Pimpl::HandleResponseRead,
                                this,
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

                        // TODO
                        //if (has_timeout)
                        {
                            m_reader_ptr->set_timeout(30); // Timeout in seconds
                        }

                        // TODO
                        //if (has_max_content_length)
                        {
                            m_reader_ptr->set_max_content_length(500 * 1024 * 1024);
                        }

                        m_reader_ptr->receive();
                    }
                }

                void HandleResponseRead(pion::http::response_ptr response_ptr, pion::tcp::connection_ptr conn_ptr, const boost::system::error_code &ec)
                {
					m_progress = "Response Read";

                    if (ec)
                    {
                        Complete(ec);
                    }
                    else
                    {
                        // Wow - it actually succeeded!

                        BBOX_ASSERT(response_ptr);

                        Complete(bbox::Error(), response_ptr);
                    }
                }

                void Complete(bbox::Error error, pion::http::response_ptr response_ptr = pion::http::response_ptr())
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
                        m_complete_callback(error, ClientResponse(response_ptr));
                    }

                    // Clear us down

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
					out.Format("Method: %s\n", m_pimpl->m_request_ptr->get_method());
					out.Format("Content Length: %d\n", m_pimpl->m_request_ptr->get_content_length());

					for (const auto &entry : m_pimpl->m_request_ptr->get_headers())
					{
						out.Format("Header: %s: %s", entry.first, entry.second);
					}
				}
			}

            void ClientRequest::SetMethod(const std::string &method)
            {
                BBOX_ASSERT(m_pimpl);
                BBOX_ASSERT(!m_pimpl->m_send_called);

                m_pimpl->m_request_ptr->set_method(method);
            }

            void ClientRequest::SetHeader(const std::string &header, const std::string &value)
            {
                BBOX_ASSERT(m_pimpl);
                BBOX_ASSERT(!m_pimpl->m_send_called);

                m_pimpl->m_request_ptr->add_header(header, value);
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

                m_pimpl->m_request_ptr->set_content(content);
            }

            void ClientRequest::Send(CompleteCallback &&complete_callback)
            {
                BBOX_ASSERT(m_pimpl);

                m_pimpl->Send(*this, std::move(complete_callback));
            }

        } // namespace bbox::http::client
    } // namespace bbox::http
} // namespace bbox
