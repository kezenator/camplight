/**
 * @file
 *
 * Implementation file for the camplight::transition::RandomDither class.
 */

#include <camplight/transition/RandomDither.h>

namespace camplight
{
    namespace transition
    {

        RandomDither::RandomDither(bbox::rt::TimeSpan duration)
            : m_duration(duration)
            , m_num_changed(0)
        {
            for (size_t i = 0; i < GRID_SIZE; ++i)
                for (size_t j = 0; j < GRID_SIZE; ++j)
                    m_leds_changed[i][j] = false;
        }

        RandomDither::~RandomDither()
        {
        }

        std::string RandomDither::GetDescription() const
        {
            return bbox::Format("RandomDither[duration=%d]", m_duration.ToString());
        }

        render::Transition::Result RandomDither::RenderTransition(
            const render::ColorGrid &from,
            const render::ColorGrid &to,
            render::ColorGrid &dest,
            render::Timestamp timestamp)
        {
            size_t duration_milliseconds = m_duration.ToMilliseconds();
            size_t timestamp_milliseconds = timestamp.ToMilliseconds();

            size_t num_should_be_changed = GRID_SIZE * GRID_SIZE;

            if (timestamp_milliseconds < duration_milliseconds)
            {
                num_should_be_changed = (timestamp_milliseconds * GRID_SIZE * GRID_SIZE) / duration_milliseconds;
            }

            while (m_num_changed < num_should_be_changed)
            {
                size_t index = rand() % (GRID_SIZE * GRID_SIZE);
                size_t i = index / GRID_SIZE;
                size_t j = index % GRID_SIZE;

                if (!m_leds_changed[i][j])
                {
                    m_leds_changed[i][j] = true;
                    m_num_changed += 1;
                }
            }

            for (size_t i = 0; i < GRID_SIZE; ++i)
            {
                for (size_t j = 0; j < GRID_SIZE; ++j)
                {
                    dest[i][j] = m_leds_changed[i][j]
                        ? to[i][j]
                        : from[i][j];
                }
            }

            return (m_num_changed == (GRID_SIZE*GRID_SIZE))
                ? TransitionComplete
                : TransitionProgressing;
        }

    } // namespace camplight::transition
} // namespace camplight
