/**
 * @file
 *
 * Header file for the bbox::debug::DebugChildEntry class.
 *
 */

#ifndef __BBOX__DEBUG__DEBUG_CHILD_ENTRY_H__
#define __BBOX__DEBUG__DEBUG_CHILD_ENTRY_H__

#include <string>

namespace bbox
{
	namespace debug
	{

		/**
		 * Represents a single result from a debug query.
		 */
		class DebugChildEntry
		{
		public:
            std::string name;
            std::string path;
            std::string short_description;

            DebugChildEntry() = default;
            DebugChildEntry(const DebugChildEntry &) = default;
            DebugChildEntry(DebugChildEntry &&) = default;
            ~DebugChildEntry() = default;
            DebugChildEntry &operator =(const DebugChildEntry &) = default;
            DebugChildEntry &operator =(DebugChildEntry &&) = default;

            DebugChildEntry(const std::string &_name, const std::string &_path, const std::string &_short_description)
                : name(_name)
                , path(_path)
                , short_description(_short_description)
            {
            }
        };

	} // namespace bbox::debug
} // namespace bbox

#endif // __BBOX__DEBUG__DEBUG_CHILD_ENTRY_H__
