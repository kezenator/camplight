/**
 * @file
 *
 * Implementation file for the camplight::transition::Immediate class.
 */

#include <camplight/transition/Immediate.h>

namespace camplight
{
    namespace transition
    {

        Immediate::Immediate()
        {
        }

        Immediate::~Immediate()
        {
        }

        std::string Immediate::GetDescription() const
        {
            return "Immediate";
        }

        render::Transition::Result Immediate::RenderTransition(
            const render::ColorGrid & /*from*/,
            const render::ColorGrid &to,
            render::ColorGrid &dest,
            render::Timestamp /*timestamp*/)
        {
            for (size_t i = 0; i < GRID_SIZE; ++i)
            {
                for (size_t j = 0; j < GRID_SIZE; ++j)
                {
                    dest[i][j] = to[i][j];
                }
            }

            return TransitionComplete;
        }

    } // namespace camplight::transition
} // namespace camplight
