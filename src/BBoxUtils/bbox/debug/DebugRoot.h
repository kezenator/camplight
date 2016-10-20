/**
 * @file
 *
 * Header file for the bbox::debug::DebugRoot class.
 *
 */

#ifndef __BBOX__DEBUG__DEBUG_ROOT_H__
#define __BBOX__DEBUG__DEBUG_ROOT_H__

#include <string>

namespace bbox
{
	namespace debug
	{
		// Forward declarations
		class DebugVisitor;

		/**
		 * Defines a root for visiting items in the debug hierarchy.
		 */
		class DebugRoot
		{
			DebugRoot() = delete;
			DebugRoot(const DebugRoot &) = delete;
			const DebugRoot &operator =(const DebugRoot &) = delete;

		public:
			explicit DebugRoot(std::string &&name);
			virtual ~DebugRoot();

			const std::string &GetName() const { return m_name; }

			virtual void VisitDebugRootEntry(DebugVisitor &&visitor) = 0;

		private:
			const std::string m_name;
		};

	} // namespace bbox::debug
} // namespace bbox

#endif // __BBOX__DEBUG__DEBUG_ROOT_H__
