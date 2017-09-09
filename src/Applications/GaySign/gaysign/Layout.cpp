/**
 * @file
 *
 * Implementation for the gaysign::Layout class.
 *
 */

#include <gaysign/Layout.h>

namespace gaysign
{

    const std::vector<Layout::Entry> Layout::ENTRIES =
    {
        { true,  0.189189189, 0.145945946 },
        { false, 0.162162162, 0.405405405 },
        { false, 0.135135135, 0.648648649 },
        { false, 0.405405405, 0.864864865 },
        { false, 0.524324324, 0.648648649 },
        { true,  0.378378378, 0.497297297 },
        { false, 0.497297297, 0.243243243 },
        { true,  0.594594595, 0.081081081 },
        { true,  0.837837838, 0.216216216 },
        { true,  0.837837838, 0.540540541 },
        { false, 0.756756757, 0.864864865 },
    };

} // namespace gaysign
