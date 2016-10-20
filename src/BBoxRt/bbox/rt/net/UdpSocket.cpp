/**
* @file
*
* Implementation for the bbox::rt::net::UdpSocket class.
*/

#include <bbox/rt/net/UdpSocket.h>

#include <sstream>

#include <boost/bind.hpp>

namespace bbox {
    namespace rt {
        namespace net {

            UdpSocket::UdpSocket(const std::string &name, Resource &parent)
                : Resource(name, parent)
                , m_socket(GetProactor().GetIoService())
                , m_local_endpoint()
                , m_read_buffer_ptr(nullptr)
                , m_read_from_endpoint()
                , m_pending_reads(0)
                , m_user_read_callback()
                , m_internal_read_callback()
				, m_send_packets(0)
				, m_send_bytes(0)
				, m_send_errs(0)
				, m_recv_packets(0)
				, m_recv_bytes(0)
				, m_recv_errs(0)
				, m_setup_err()
			    , m_send_last_err()
				, m_recv_last_err()
			{
            }

            UdpSocket::UdpSocket(const std::string &name, Service &parent)
                : Resource(name, parent)
                , m_socket(GetProactor().GetIoService())
                , m_local_endpoint()
                , m_read_buffer_ptr(nullptr)
                , m_read_from_endpoint()
                , m_pending_reads(0)
                , m_user_read_callback()
                , m_internal_read_callback()
				, m_send_packets(0)
				, m_send_bytes(0)
				, m_send_errs(0)
				, m_recv_packets(0)
				, m_recv_bytes(0)
				, m_recv_errs(0)
				, m_setup_err()
				, m_send_last_err()
				, m_recv_last_err()
			{
            }

            UdpSocket::~UdpSocket()
            {
            }

            void UdpSocket::HandleStopping()
            {
                DisableReading();

                if (m_pending_reads != 0)
                {
                    m_socket.cancel();
                }

                CheckShutdown();
            }

            void UdpSocket::CheckShutdown()
            {
                if ((GetLocalRunLevel() == RunLevel::STOPPING)
                    && (m_pending_reads == 0))
                {
                    NotifyStopped();
                    RequestStopAllChildren();
                }
            }

            Error UdpSocket::Open(boost::asio::ip::udp::endpoint::protocol_type protocol)
            {
                boost::system::error_code ec;

                m_socket.open(protocol, ec);

                if (!ec)
                {
                    m_socket.non_blocking(true, ec);
                }

				if (ec)
				{
					m_setup_err = ec;
				}

                return ec;
            }

            Error UdpSocket::Bind(const UdpEndpoint &endpoint)
            {
                boost::system::error_code ec;

                m_socket.bind(endpoint, ec);

				if (ec)
				{
					m_setup_err = ec;
					return ec;
				}

                m_local_endpoint = m_socket.local_endpoint(ec);

				if (ec)
				{
					m_setup_err = ec;
					return ec;
				}

                return Error::Success;
            }

            void UdpSocket::EnableReading(ReadCallback &&read_callback)
            {
                DisableReading();

                m_user_read_callback = std::move(read_callback);

                if (!m_internal_read_callback)
                {
                    m_internal_read_callback = boost::bind(
                        &UdpSocket::HandleRead,
                        this,
                        _1,
                        _2);
                }

                if (!m_read_buffer_ptr)
                {
                    m_read_buffer_ptr = std::unique_ptr<ReadBuffer>(new ReadBuffer);
                }

                if (m_pending_reads == 0)
                {
                    ++m_pending_reads;

                    m_socket.async_receive_from(
                        boost::asio::buffer(m_read_buffer_ptr->data, sizeof(m_read_buffer_ptr->data)),
                        boost::ref(m_read_from_endpoint),
                        m_internal_read_callback);
                }
            }

            void UdpSocket::DisableReading()
            {
                m_user_read_callback = ReadCallback();
            }

            Error UdpSocket::SendTo(const void *data, size_t length, const UdpEndpoint &destination)
            {
                boost::system::error_code ec;

                m_socket.send_to(
                    boost::asio::buffer(data, length),
                    destination,
                    0,
                    ec);

				if (ec)
				{
					m_send_errs += 1;
					m_send_last_err = ec;
				}
				else
				{
					m_send_packets += 1;
					m_send_bytes += length;
				}

                return ec;
            }

            void UdpSocket::HandleRead(const boost::system::error_code &ec, size_t bytes_recevied)
            {
                --m_pending_reads;

				if (ec)
				{
					m_recv_errs += 1;
					m_recv_last_err = ec;
				}
				else
				{
					m_recv_packets += 1;
					m_recv_bytes += bytes_recevied;
				}

                if (m_user_read_callback)
                {
                    m_user_read_callback(
                        ec,
                        (bytes_recevied && !ec) ? m_read_buffer_ptr->data : nullptr,
                        bytes_recevied,
                        m_read_from_endpoint);
                }

                if (m_user_read_callback
					&& (m_pending_reads == 0))
                {
                    ++m_pending_reads;

                    m_socket.async_receive_from(
                        boost::asio::buffer(m_read_buffer_ptr->data, sizeof(m_read_buffer_ptr->data)),
                        boost::ref(m_read_from_endpoint),
                        m_internal_read_callback);
                }

                CheckShutdown();
            }

			void UdpSocket::PrintState(bbox::DebugOutput &out) const
			{
				out.Format("Is Open:        %s\n", m_socket.is_open());
				out.Format("Local Endpoint: %s\n", m_local_endpoint.ToString());
				out.Format("Setup Err:      %s\n", m_setup_err.ToString());
				out.Format("\n");
				out.Format("Recv Packets:   %d\n", m_recv_packets);
				out.Format("Recv Bytes:     %d\n", m_recv_bytes);
				out.Format("Recv Errors:    %d\n", m_recv_errs);
				out.Format("Recv Last Err:  %s\n", m_recv_last_err.ToString());
				out.Format("\n");
				out.Format("Send Packets:   %d\n", m_send_packets);
				out.Format("Send Bytes:     %d\n", m_send_bytes);
				out.Format("Send Errors:    %d\n", m_send_errs);
				out.Format("Send Last Err:  %s\n", m_send_last_err.ToString());
			}

        } // namespace bbox::rt::net
    } // namespace bbox::rt
} // namespace bbox
