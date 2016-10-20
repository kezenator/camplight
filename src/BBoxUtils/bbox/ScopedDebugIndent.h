/**
 * @file
 * 
 * Header file for the bbox::ScopedDebugIndent class.
 */

#ifndef __BBOX__SCOPED_DEBUG_INDENT_H__
#define __BBOX__SCOPED_DEBUG_INDENT_H__

#include <bbox/DebugOutput.h>

namespace bbox
{
	/**
	 * Increases the indent of a debug output for the
	 * lifetime of this variable.
	 */
	class ScopedDebugIndent
	{
		ScopedDebugIndent() = delete;
		ScopedDebugIndent(const ScopedDebugIndent &other) = delete;
		ScopedDebugIndent &operator =(const ScopedDebugIndent &other) = delete;

	public:
		ScopedDebugIndent(bbox::DebugOutput &out, size_t indent)
			: m_out(out)
			, m_indent(indent)
		{
			m_out.IncIndent(m_indent);
		}

		~ScopedDebugIndent()
		{
			m_out.DecIndent(m_indent);
		}

	private:
		bbox::DebugOutput &m_out;
		const size_t m_indent;
	};

} // namespace bbox

#endif // __BBOX__SCOPED_DEBUG_INDENT_H__
