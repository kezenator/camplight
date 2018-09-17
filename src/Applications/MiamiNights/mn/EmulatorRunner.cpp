/**
 * @file
 *
 * Implementation file for the mn::EmulatorRunner class.
 */

#include <mn/EmulatorRunner.h>

#include <bbox/Error.h>

namespace mn {

EmulatorRunner::EmulatorRunner(const std::string &name, bbox::rt::Service &parent, std::function<void()> &&on_complete_handler)
	: bbox::rt::Resource(name, parent)
	, m_on_complete_handler(std::move(on_complete_handler))
	, m_running(false)
	, m_thread()
{
}

EmulatorRunner::~EmulatorRunner()
{
}

void EmulatorRunner::HandleStopping()
{
	CheckShutdown();
}

void EmulatorRunner::PrintState(bbox::DebugOutput &out) const
{
	out.Format("Running: %s\n", m_running);
	out.Format("Game:    %s\n", m_game);
}

void EmulatorRunner::Start(const std::string &game)
{
	if (!m_running)
	{
		m_running = true;
		m_game = game;
		m_thread = std::thread([this]() { ThreadProc(); });
	}
}

void EmulatorRunner::CheckShutdown()
{
	if ((GetLocalRunLevel() == bbox::rt::RunLevel::STOPPING)
		&& !m_running)
	{
		RequestStopAllChildren();
		NotifyStopped();
	}
}

void EmulatorRunner::ThreadProc()
{
	std::wstring cmd;
	std::wstring current_dir{ L"C:\\mame" };

	if (m_game == "altered-beast")
	{
		cmd = L"C:\\mame\\mame64.exe genesis -cart .\\roms\\AlteredBeast.smd";
	}
	else if (m_game == "super-mario-world")
	{
		cmd = L"C:\\mame\\mame64.exe snes -cart .\\roms\\super_mario_world.smc";
	}
	else // default to Sonic
	{
		cmd = L"C:\\mame\\mame64.exe genesis -cart .\\roms\\sonic3.smd";
	}

	STARTUPINFO startup_info;
	PROCESS_INFORMATION proc_info;

	ZeroMemory(&startup_info, sizeof(STARTUPINFO));
	ZeroMemory(&proc_info, sizeof(PROCESS_INFORMATION));
	startup_info.cb = sizeof(STARTUPINFO);

	BOOL result = ::CreateProcessW(
		nullptr,
		cmd.data(),
		nullptr,
		nullptr,
		false,
		CREATE_NEW_CONSOLE,
		nullptr,
		current_dir.c_str(),
		&startup_info,
		&proc_info);

	if (result == 0)
	{
		// ERROR!

		bbox::Error error = bbox::Error::Win32_GetLastError();

		GetProactor().ThreadSafePost([this, error]()
			{
				bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Error);
				if (out)
				{
					out.Format("ERROR: Could not start emulator: %s\n", error.ToString());
				}
			});
	}
	else
	{
		// Wait for process to terminate...

		::WaitForSingleObject(proc_info.hProcess, INFINITE);

		::CloseHandle(proc_info.hProcess);
		::CloseHandle(proc_info.hThread);
	}

	GetProactor().ThreadSafePost([this]() { ProcessCompleted(); });
}

void EmulatorRunner::ProcessCompleted()
{
	m_thread.join();
	m_running = false;

	m_on_complete_handler();

	CheckShutdown();
}

} // namespace mn
