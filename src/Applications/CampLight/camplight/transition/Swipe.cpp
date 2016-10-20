/**
 * @file
 *
 * Implementation file for the camplight::transition::Swipe class.
 */

#include <camplight/transition/Swipe.h>

namespace camplight
{
    namespace transition
    {

        Swipe::Swipe(bbox::rt::TimeSpan duration)
            : m_duration(duration)
            , m_direction(rand() % 8)
        {
        }

        Swipe::~Swipe()
        {
        }

        std::string Swipe::GetDescription() const
        {
            return bbox::Format("Swipe[direction=%d,duration=%d]", m_direction, m_duration.ToString());
        }

        render::Transition::Result Swipe::RenderTransition(
            const render::ColorGrid &from,
            const render::ColorGrid &to,
            render::ColorGrid &dest,
            render::Timestamp timestamp)
        {
            size_t num_steps = GRID_SIZE;
            if (m_direction >= 4)
                num_steps = GRID_SIZE + GRID_SIZE - 1;

            Result result = TransitionComplete;
            size_t cur_step = num_steps;

            size_t duration_milliseconds = m_duration.ToMilliseconds();
            size_t timestamp_milliseconds = timestamp.ToMilliseconds();

            if (timestamp_milliseconds < duration_milliseconds)
            {
                result = TransitionProgressing;
                cur_step = (timestamp_milliseconds * num_steps) / duration_milliseconds;
            }

            for (size_t i = 0; i < GRID_SIZE; ++i)
            {
                for (size_t j = 0; j < GRID_SIZE; ++j)
                {
                    size_t pixel_step = 0;
                    switch (m_direction)
                    {
                    case 0: pixel_step = i; break;
                    case 1: pixel_step = j; break;
                    case 2: pixel_step = GRID_SIZE - 1 - i; break;
                    case 3: pixel_step = GRID_SIZE - 1 - j; break;

                    case 4: pixel_step = i + j; break;
                    case 5: pixel_step = i + (GRID_SIZE - 1 - j); break;
                    case 6: pixel_step = (GRID_SIZE - 1 - i) + j; break;
                    case 7: pixel_step = (GRID_SIZE - 1 - i) + (GRID_SIZE - 1 - j); break;
                    }

                    dest[i][j] = (pixel_step >= cur_step)
                        ? from[i][j]
                        : to[i][j];
                }
            }

            return result;
        }

    } // namespace camplight::transition
} // namespace camplight
