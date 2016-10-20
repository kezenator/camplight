/**
 * @file
 *
 * Implementation file for the camplight::pattern::TestPattern class.
 */

#include <camplight/pattern/TestPattern.h>

namespace camplight
{
    namespace pattern
    {

        TestPattern::TestPattern()
        {
        }

        TestPattern::~TestPattern()
        {
        }

        std::string TestPattern::GetDescription() const
        {
            return "TestPattern";
        }

        void TestPattern::RenderTo(render::ColorGrid &dest, render::Timestamp timestamp)
        {
            render::Color color;

            switch ((timestamp.ToMilliseconds() / 2000) % 4)
            {
            case 0:
                color = render::Color(0, 0, 0);
                break;
            case 1:
                color = render::Color(255, 0, 0);
                break;
            case 2:
                color = render::Color(0, 255, 0);
                break;
            case 3:
                color = render::Color(0, 0, 255);
                break;
            }

            for (size_t i = 0; i < GRID_SIZE; ++i)
            {
                for (size_t j = 0; j < GRID_SIZE; ++j)
                {
                    dest[i][j] = color;
                }
            }
        }

    } // namespace camplight::pattern
} // namespace camplight
