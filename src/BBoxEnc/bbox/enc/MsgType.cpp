/**
 * @file
 *
 * Implementation file for the bbox::enc::MsgType class.
 *
 */

#include <bbox/enc/MsgType.h>
#include <bbox/enc/MsgAnyPtr.h>

namespace bbox {
namespace enc {

MsgAnyPtr MsgType::CreateNewObject() const
{
	return m_ptr->constructor();
}

} // namespace bbox::enc
} // namespace bbox
