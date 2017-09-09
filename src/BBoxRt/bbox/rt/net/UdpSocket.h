/**
 * @file
 *
 * Header for the bbox::rt::net::UdpSocket class.
 */

#ifndef __BBOX__RT__NET__UDP_SOCKET_H__
#define __BBOX__RT__NET__UDP_SOCKET_H__

#pragma once

#include <string>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/ip/multicast.hpp>
#include <boost/function.hpp>

#include <bbox/Error.h>
#include <bbox/rt/Resource.h>
#include <bbox/rt/DebugEnable.h>
#include <bbox/rt/net/UdpEndpoint.h>

namespace bbox {
    namespace rt { 
        namespace net {

            /**
             * An UDP Socket.
             */
            class UdpSocket: public Resource
            {
            public:

                typedef boost::function<void(const Error &err, const void *data, size_t num_bytes, const UdpEndpoint &from)> ReadCallback;

                UdpSocket(const std::string &name, Resource &parent);
                UdpSocket(const std::string &name, Service &parent);
                ~UdpSocket();

                Error Open(boost::asio::ip::udp::endpoint::protocol_type protocol);
                Error Bind(const UdpEndpoint &endpoint);

                template <typename Option>
                Error SetOption(const Option &option)
                {
                    boost::system::error_code ec;

                    m_socket.set_option(option, ec);

                    return ec;
                }

                const UdpEndpoint &GetLocalEndpoint() const { return m_local_endpoint; }

                void EnableReading(ReadCallback &&read_callback);
                void DisableReading();

                Error SendTo(const void *data, size_t length, const UdpEndpoint &destination);

            private:
                void HandleStopping() override;
				void PrintState(bbox::DebugOutput &out) const override;
                void CheckShutdown();

                void HandleRead(const boost::system::error_code &ec, size_t bytes_recevied);

                struct ReadBuffer
                {
                    uint8_t data[65536];
                };

                boost::asio::ip::udp::socket m_socket;
                UdpEndpoint m_local_endpoint;

                std::unique_ptr<ReadBuffer> m_read_buffer_ptr;
                boost::asio::ip::udp::endpoint m_read_from_endpoint;
                size_t m_pending_reads;
                ReadCallback m_user_read_callback;
                boost::function<void(const boost::system::error_code &ec, size_t bytes_received)> m_internal_read_callback;

				uint64_t m_send_packets;
				uint64_t m_send_bytes;
				uint64_t m_send_errs;
				uint64_t m_recv_packets;
				uint64_t m_recv_bytes;
				uint64_t m_recv_errs;

				Error m_setup_err;
				Error m_send_last_err;
				Error m_recv_last_err;

                DebugEnable m_debug;
            };

        } // namespace bbox::rt::net
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__UDP_SOCKET_H__
