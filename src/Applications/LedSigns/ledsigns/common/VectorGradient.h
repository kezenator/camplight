/**
 * @file
 *
 * Header file for the ledsigns::common::VectorGradient class.
 */

#ifndef __LEDSIGNS__COMMON__VECTOR_GRADIENT_H__
#define __LEDSIGNS__COMMON__VECTOR_GRADIENT_H__

#include <ledsigns/common/Gradient.h>
#include <map>

namespace ledsigns
{
    namespace common
    {

        /**
         * A gradient that interpolates between a vector of colors.
         */
        class VectorGradient: public Gradient
        {
        public:

            using Map = std::map<double, leds::Color>;

            VectorGradient(
                std::string &&name,
                Map &&color_map);

            ~VectorGradient();

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            leds::Color Convert(double progress) const override;
            
        private:
            const std::string m_name;
            const Map m_color_map;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__VECTOR_GRADIENT_H__
