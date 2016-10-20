/**
* @file
*
* Implementation for the bbox::rt::TimeSpan class.
*/

#include <bbox/rt/TimeSpan.h>
#include <bbox/Format.h>

namespace bbox {
    namespace rt {

        std::string TimeSpan::ToString() const
        {
			std::string frac_seconds_str;

			if ((std::chrono::duration_cast<std::chrono::microseconds>(m_duration).count() % 1000000) != 0)
			{
				frac_seconds_str = Format(".%06d", std::chrono::duration_cast<std::chrono::microseconds>(m_duration).count());
			}

			auto seconds = std::chrono::duration_cast<std::chrono::seconds>(m_duration).count();

			if (seconds < 60)
			{
				return Format("%d%s s",
					seconds,
					frac_seconds_str);
			}
			else
			{
				auto hours = seconds / 3600;
				auto mins = (seconds / 60) % 60;
				seconds = seconds % 60;

				return Format("%d h %d min %d%s s",
					hours, mins, seconds,
					frac_seconds_str);
			}
        }

    } // namespace bbox::rt
} // namespace bbox
