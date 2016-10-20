/**
 * @file
 *
 * Implementation file for the camplight::transition::LinearFace class.
 */

#include <camplight/transition/LinearFade.h>

namespace camplight
{
    namespace transition
    {

        LinearFade::LinearFade(bbox::rt::TimeSpan duration)
            : m_duration(duration)
        {
        }

        LinearFade::~LinearFade()
        {
        }

        std::string LinearFade::GetDescription() const
        {
            return bbox::Format("LinearFade[duration=%d]", m_duration.ToString());
        }

        render::Transition::Result LinearFade::RenderTransition(
            const render::ColorGrid &from,
            const render::ColorGrid &to,
            render::ColorGrid &dest,
            render::Timestamp timestamp)
        {
            Result result = TransitionComplete;
            uint8_t from_scale = 0;
            uint8_t to_scale = 255;

            size_t duration_milliseconds = m_duration.ToMilliseconds();
            size_t timestamp_milliseconds = timestamp.ToMilliseconds();

            if (timestamp_milliseconds < duration_milliseconds)
            {
                result = TransitionProgressing;
                to_scale = (timestamp_milliseconds * 255) / duration_milliseconds;
                from_scale = 255 - to_scale;
            }

            for (size_t i = 0; i < GRID_SIZE; ++i)
            {
                for (size_t j = 0; j < GRID_SIZE; ++j)
                {
                    dest[i][j] = from[i][j].Scale(from_scale) + to[i][j].Scale(to_scale);
                }
            }

            return result;
        }

    } // namespace camplight::transition
} // namespace camplight
