/**
 * @file        test_main.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-15>
 * @version     0.0.0
 *
 * @brief       
 *
 * @details
 */

/*****************************************************************************/

/* Standard libraries */

#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

/* Third party libraries */

/* Project libraries */


/*****************************************************************************/

/* Tests */

/**
 * @test 
 * @brief 
 *
 * @details
 */
/*
TEST(ColaTest, KeepMaxBufferSize) {
    
}
*/

/**
 * @test 
 * @brief 
 *
 * @details
 */
/*
class ColaFifoTest : public ::testing::TestWithParam<std::vector<int>> {
   protected:
    Cola<int> cola;
};

TEST_P(ColaFifoTest, ExtractsInOrder) {
    auto values = GetParam();

    // Given: a queue with multiple pushes
    for (auto v : values) {
        cola.push(v);
    }

    // When & Then: pop retrieves elements in the same order
    for (auto expected : values) {
        auto val = cola.pop(std::chrono::seconds(5));
        ASSERT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), expected);
    }
    EXPECT_TRUE(cola.is_empty());
}


// Instanciación con diferentes secuencias
INSTANTIATE_TEST_SUITE_P(MultipleSequences, ColaFifoTest,
                         ::testing::Values(std::vector<int>{10, 20},
                                           std::vector<int>{1, 2, 3, 4, 5}, std::vector<int>{42}));
*/