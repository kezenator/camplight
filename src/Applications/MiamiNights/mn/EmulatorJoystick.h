/**
 * @file
 *
 * Header file for the mn::EmulatorJoystick class.
 */

#ifndef __MN__EMULATOR_JOYSTICK_H__
#define __MN__EMULATOR_JOYSTICK_H__

#include <memory>

#include <bbox/rt/Resource.h>

#include <mn/msgs/ButtonStates.h>

namespace mn {

/**
 * A class that feeds button presses through to a virtual joystick interface.
 */
class EmulatorJoystick : public bbox::rt::Resource
{
public:
	EmulatorJoystick(
		const std::string &name,
		bbox::rt::Service &parent);
	~EmulatorJoystick();

	void SetStates(const msgs::ButtonStates &states);

private:
	void HandleStopping() override;
	void PrintState(bbox::DebugOutput &out) const override;

	struct Pimpl;
	std::unique_ptr<Pimpl> m_pimpl;
};

} // namespace mn

#endif // __MN__EMULATOR_JOYSTICK_H__
