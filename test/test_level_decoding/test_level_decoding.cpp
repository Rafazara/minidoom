#include <unity.h>
#include "doom_nano_level.h"

void test_nibbles_and_inverted_rows() {
    const uint8_t level[DoomNanoLevel::SIZE] = {
        0xAB,
        0xCD,
    };

    TEST_ASSERT_EQUAL_UINT8(0xB, DoomNanoLevel::getBlock(level, 0, 56));
    TEST_ASSERT_EQUAL_UINT8(0xA, DoomNanoLevel::getBlock(level, 1, 56));
    TEST_ASSERT_EQUAL_UINT8(0xD, DoomNanoLevel::getBlock(level, 0, 55));
    TEST_ASSERT_EQUAL_UINT8(0xC, DoomNanoLevel::getBlock(level, 1, 55));
    TEST_ASSERT_EQUAL_UINT8(0xB, DoomNanoLevel::getBlock(level, 0.9, 56.9));
}

void test_invalid_coordinates_are_walls() {
    const uint8_t level[DoomNanoLevel::SIZE] = {};

    TEST_ASSERT_EQUAL_UINT8(DoomNanoLevel::WALL, DoomNanoLevel::getBlock(level, -0.1, 0));
    TEST_ASSERT_EQUAL_UINT8(DoomNanoLevel::WALL, DoomNanoLevel::getBlock(level, 0, -0.1));
    TEST_ASSERT_EQUAL_UINT8(DoomNanoLevel::WALL, DoomNanoLevel::getBlock(level, 64, 0));
    TEST_ASSERT_EQUAL_UINT8(DoomNanoLevel::WALL, DoomNanoLevel::getBlock(level, 0, 57));
    TEST_ASSERT_EQUAL_UINT8(DoomNanoLevel::WALL, DoomNanoLevel::getBlock(nullptr, 0, 0));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_nibbles_and_inverted_rows);
    RUN_TEST(test_invalid_coordinates_are_walls);
    return UNITY_END();
}
