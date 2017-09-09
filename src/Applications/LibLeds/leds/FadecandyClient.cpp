/**
 * @file
 *
 * Implementation file for the leds::FadecandyClient class.
 */

#include <leds/FadecandyClient.h>

#include <bbox/Assert.h>
#include <bbox/rt/Timer.h>

#include <boost/asio.hpp>

namespace leds
{

    struct FadecandyClient::Pimpl
    {
        Pimpl(FadecandyClient &owner, bbox::rt::net::TcpEndpoint remote_endpoint)
            : m_owner(owner)
            , m_remote_endpoint(remote_endpoint)
            , m_socket(owner.GetProactor().GetIoService())
            , m_reconnect_timer("reconnect-timer", m_owner)
            , m_outstanding_ops(0)
            , m_socket_open(false)
            , m_pending_data()
        {
            m_reconnect_timer.SetHandler(std::bind(&Pimpl::ReconnectTimeout, this));
        }

        void ReconnectTimeout()
        {
            BBOX_ASSERT(m_outstanding_ops == 0);

            m_outstanding_ops += 1;
            m_socket.async_connect(
                m_remote_endpoint,
                std::bind(&FadecandyClient::Pimpl::ConnectComplete, this, std::placeholders::_1));
        }

        void ConnectComplete(const boost::system::error_code &ec)
        {
            BBOX_ASSERT(m_outstanding_ops > 0);
            m_outstanding_ops -= 1;

            if (ec)
            {
                m_socket_open = false;

                if (m_owner.GetLocalRunLevel() == bbox::rt::RunLevel::STOPPING)
                {
                    CheckShutdown();
                }
                else
                {
                    m_reconnect_timer.StartSingleShot(bbox::rt::TimeSpan::Seconds(30));
                }
            }
            else
            {
                // We're open! - try writing the first bytes

                m_socket_open = true;

                if (m_pending_data.size() > 0)
                {
                    StartWrite();
                }
            }
        }

        void CheckShutdown()
        {
            if ((m_owner.GetLocalRunLevel() == bbox::rt::RunLevel::STOPPING)
                && (m_outstanding_ops == 0))
            {
                m_reconnect_timer.Cancel();

                m_owner.RequestStopAllChildren();
                m_owner.NotifyStopped();
            }
        }

        void StartWrite()
        {
            BBOX_ASSERT(m_pending_data.size() > 0);

            if (m_socket_open
                && (m_outstanding_ops == 0))
            {
                m_writing_data = std::move(m_pending_data);
                m_pending_data.clear();

                m_outstanding_ops += 1;

                boost::asio::async_write(
                    m_socket,
                    boost::asio::buffer(&m_writing_data[0], m_writing_data.size()),
                    std::bind(&Pimpl::WriteComplete, this, std::placeholders::_1, std::placeholders::_2));
            }
        }

        void WriteComplete(const boost::system::error_code &ec, size_t bytes_transferred)
        {
            BBOX_ASSERT(m_outstanding_ops > 0);
            m_outstanding_ops -= 1;
            m_writing_data.clear();

            BBOX_ASSERT(m_outstanding_ops == 0);

            if (ec)
            {
                m_socket_open = false;
                m_socket.close();
                m_reconnect_timer.StartSingleShot(bbox::rt::TimeSpan::Seconds(1));
            }
            else
            {
                if (m_pending_data.size() > 0)
                    StartWrite();
            }

            CheckShutdown();
        }

        FadecandyClient &m_owner;
        const bbox::rt::net::TcpEndpoint m_remote_endpoint;
        boost::asio::ip::tcp::socket m_socket;
        bbox::rt::Timer m_reconnect_timer;
        size_t m_outstanding_ops;
        bool m_socket_open;
        std::vector<uint8_t> m_pending_data;
        std::vector<uint8_t> m_writing_data;
    };

    FadecandyClient::FadecandyClient(const std::string &name, Service &parent, bbox::rt::net::TcpEndpoint remote_endpoint)
        : bbox::rt::Service(name, parent)
        , m_pimpl(std::make_unique<Pimpl>(*this, remote_endpoint))
    {
    }

    FadecandyClient::~FadecandyClient()
    {
    }

    void FadecandyClient::HandleStarting()
    {
        m_pimpl->m_reconnect_timer.StartSingleShot(bbox::rt::TimeSpan::Seconds(1));

        NotifyStarted();
    }

    void FadecandyClient::HandleStopping()
    {
        if (m_pimpl->m_socket.is_open())
            m_pimpl->m_socket.close();

        m_pimpl->CheckShutdown();
    }

    void FadecandyClient::PrintState(bbox::DebugOutput &out) const
    {
        // TODO
    }

    void FadecandyClient::UpdateLeds(const std::vector<Color> &colors)
    {
        bbox::enc::ToBinary m;
        m.WriteUint8(0); // Channel
        m.WriteUint8(0); // Set Pixels Command
        m.WriteUint16BE(colors.size() * 3); // Data Size
        for (const auto &color : colors)
        {
            m.WriteUint8(color.GetRed());
            m.WriteUint8(color.GetGreen());
            m.WriteUint8(color.GetBlue());
        }

        m_pimpl->m_pending_data = m.ExtractData();
        m_pimpl->StartWrite();
    }


} // namespace leds
