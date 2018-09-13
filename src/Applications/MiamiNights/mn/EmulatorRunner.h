/**
 * @file
 *
 * Header file for the mn::EmulatorRunner class.
 */

#ifndef __MN__EMULATOR_RUNNER_H__
#define __MN__EMULATOR_RUNNER_H__

#include <bbox/rt/Resource.h>

#include <thread>

namespace mn {

// Forward declarations
class ApplicationService;

/**
 * A web-socket that can send and receive messages to a single remote party.
 */
class EmulatorRunner : public bbox::rt::Resource
{
public:
	EmulatorRunner(
		const std::string &name,
		bbox::rt::Service &parent,
		std::function<void()> &&on_complete_handler);
	~EmulatorRunner();

	void Start(const std::string &game);

private:
	void HandleStopping() override;
	void PrintState(bbox::DebugOutput &out) const override;

	void ThreadProc();
	void ProcessCompleted();

	std::function<void()> m_on_complete_handler;

	bool m_running;
	std::string m_game;
	std::thread m_thread;
};

} // namespace mn

#endif // __MN__EMULATOR_RUNNER_H__
