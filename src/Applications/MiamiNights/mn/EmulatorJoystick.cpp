/**
 * @file
 *
 * Implementation file for the mn::EmulatorJoystick class.
 */

#include <mn/EmulatorJoystick.h>

#include <public.h>
#include <vjoyinterface.h>

#include <bbox/enc/ToXml.h>

namespace mn {

static constexpr int DEVICE_ID = 1;

struct EmulatorJoystick::Pimpl
{
	bool m_enabled;
	bool m_aquired;
	msgs::ButtonStates m_states;
    bool m_emulator_running;
    bool m_timer_expired;
    bbox::rt::Timer m_timer;

    Pimpl(EmulatorJoystick &parent)
        : m_enabled(false)
        , m_aquired(false)
        , m_states()
        , m_emulator_running(false)
        , m_timer_expired(false)        , m_timer("timer", parent, std::bind(&EmulatorJoystick::HandleTimerExpired, &parent))    {    }
};

EmulatorJoystick::EmulatorJoystick(const std::string &name,	bbox::rt::Service &parent)
	: bbox::rt::Resource(name, parent)
	, m_pimpl(std::make_unique<Pimpl>(*this))
{
	m_pimpl->m_enabled = vJoyEnabled();
	m_pimpl->m_aquired = false;
}

EmulatorJoystick::~EmulatorJoystick()
{
	if (m_pimpl->m_aquired)
	{
		RelinquishVJD(DEVICE_ID);
		m_pimpl->m_aquired = false;
	}
}

void EmulatorJoystick::HandleStopping()
{
    m_pimpl->m_timer.Cancel();

	RequestStopAllChildren();
	NotifyStopped();
}

void EmulatorJoystick::PrintState(bbox::DebugOutput &out) const
{
	out.Format("Enabled: %s\n", m_pimpl->m_enabled);
	out.Format("States:\n");
	{
		out.IncIndent(4);
		out.Format("%s\n", bbox::enc::ToXml::ConvertToPretty("button-states", m_pimpl->m_states));
		out.DecIndent(4);
	}

	if (m_pimpl->m_enabled)
	{
		out.Format("Aquired: %s\n", m_pimpl->m_aquired);
		out.Format("Vendor:  %s\n", GetvJoyManufacturerString());
		out.Format("Product: %s\n", GetvJoyProductString());
		out.Format("Version: %s\n", GetvJoySerialNumberString());
	}

    out.Format("Emulator Running: %s\n", m_pimpl->m_emulator_running);
    out.Format("Timer Expired: %s\n", m_pimpl->m_timer_expired);
}

void EmulatorJoystick::SetEmulatorRunning(bool running)
{
    if (running != m_pimpl->m_emulator_running)
    {
        m_pimpl->m_emulator_running = running;
        m_pimpl->m_timer_expired = false;

        if (GetLocalRunLevel() == bbox::rt::RunLevel::RUNNING)
        {
            SetStates(msgs::ButtonStates(m_pimpl->m_states));

            if (running)
                m_pimpl->m_timer.StartSingleShot(bbox::rt::TimeSpan::Seconds(120));
            else
                m_pimpl->m_timer.Cancel();
        }
    }
}

void EmulatorJoystick::SetStates(const msgs::ButtonStates &states)
{
	m_pimpl->m_states = states;

    if (GetLocalRunLevel() == bbox::rt::RunLevel::RUNNING)
    {
        if (m_pimpl->m_emulator_running)
            m_pimpl->m_timer.StartSingleShot(bbox::rt::TimeSpan::Seconds(120));

        if (m_pimpl->m_enabled)
        {
            if (!m_pimpl->m_aquired)
            {
                if (AcquireVJD(DEVICE_ID))
                    m_pimpl->m_aquired = true;
            }

            if (m_pimpl->m_aquired)
            {
                JOYSTICK_POSITION_V2 pos;
                memset(&pos, 0, sizeof(JOYSTICK_POSITION_V2));

                pos.bDevice = 1;
                pos.wAxisX = 0x4000;
                pos.wAxisY = 0x4000;
                pos.wAxisZ = 0x4000;

                for (size_t i = 0; i < states.button_states.size(); ++i)
                {
                    if (states.button_states[i])
                        pos.lButtons |= (1 << i);
                }

                if (states.back_state
                    || (m_pimpl->m_emulator_running && m_pimpl->m_timer_expired))
                {
                    pos.lButtons |= (1 << 6);
                }

                if (states.play_state)
                    pos.lButtons |= (1 << 7);

                if (!UpdateVJD(DEVICE_ID, &pos))
                    m_pimpl->m_aquired = false;
            }
        }
    }
}

void EmulatorJoystick::HandleTimerExpired()
{
    if (GetLocalRunLevel() == bbox::rt::RunLevel::RUNNING)
    {
        m_pimpl->m_timer_expired = true;

        SetStates(msgs::ButtonStates(m_pimpl->m_states));
    }
}

} // namespace mn
