/**
 * @file   SelfOrganizingMapTest/TrainerCompare.cpp
 * @brief  Unit tests for image processing.
 * @date   Nov 5, 2018
 * @author Bernd Doser, HITS gGmbH
 */

#include <cmath>
#include <gtest/gtest.h>
#include <limits>
#include <omp.h>

#include "SelfOrganizingMapLib/CartesianLayout.h"
#include "SelfOrganizingMapLib/Data.h"
#include "SelfOrganizingMapLib/DataIO.h"
#include "SelfOrganizingMapLib/SOM.h"
#include "SelfOrganizingMapLib/SOMIO.h"
#include "SelfOrganizingMapLib/Trainer.h"
#include "SelfOrganizingMapLib/Trainer_generic.h"
#include "UtilitiesLib/EqualFloatArrays.h"
#include "UtilitiesLib/DistributionFunctor.h"
#include "UtilitiesLib/Filler.h"

using namespace pink;

struct TrainerCompareTestData
{
    TrainerCompareTestData(uint32_t som_dim, uint32_t image_dim, uint32_t neuron_dim, int num_rot, bool use_flip)
     : som_dim(som_dim),
       image_dim(image_dim),
       neuron_dim(neuron_dim),
       num_rot(num_rot),
       use_flip(use_flip)
    {}

    uint32_t som_dim;
    uint32_t image_dim;
    uint32_t neuron_dim;

    uint32_t num_rot;
    bool use_flip;
};

class TrainerCompareTest : public ::testing::TestWithParam<TrainerCompareTestData>
{};

TEST_P(TrainerCompareTest, cartesian_2d_float)
{
    typedef Data<CartesianLayout<2>, float> DataType;
    typedef SOM<CartesianLayout<2>, CartesianLayout<2>, float> SOMType;
    typedef Trainer<CartesianLayout<2>, CartesianLayout<2>, float, false> MyTrainer;
    typedef Trainer_generic<CartesianLayout<2>, CartesianLayout<2>, float, false> MyTrainer_generic;

    DataType data({GetParam().image_dim, GetParam().image_dim});
    fill_random_uniform(data.get_data_pointer(), data.size());

    SOMType som1({GetParam().som_dim, GetParam().som_dim}, {GetParam().neuron_dim, GetParam().neuron_dim}, 0.0);
    SOMType som2({GetParam().som_dim, GetParam().som_dim}, {GetParam().neuron_dim, GetParam().neuron_dim}, 0.0);

    auto&& f = GaussianFunctor(1.1, 0.2);

    MyTrainer trainer1(som1, f, 0, GetParam().num_rot, GetParam().use_flip, 0.0, Interpolation::BILINEAR);
    trainer1(data);

    MyTrainer_generic trainer2(som2, f, 0, GetParam().num_rot, GetParam().use_flip, 0.0, Interpolation::BILINEAR);
    trainer2(data);

    EXPECT_EQ(som1.size(), som2.size());
    EXPECT_TRUE(EqualFloatArrays(som1.get_data_pointer(), som2.get_data_pointer(), som1.size(), 1e-4));
}

INSTANTIATE_TEST_CASE_P(TrainerCompareTest_all, TrainerCompareTest,
    ::testing::Values(
        TrainerCompareTestData(2, 2, 2, 1, false)
       ,TrainerCompareTestData(2, 2, 2, 4, false)
       ,TrainerCompareTestData(2, 2, 2, 8, false)
       ,TrainerCompareTestData(2, 2, 2, 1, true)
       ,TrainerCompareTestData(2, 2, 2, 4, true)
       ,TrainerCompareTestData(2, 2, 2, 8, true)
       ,TrainerCompareTestData(2, 4, 2, 8, true)
));