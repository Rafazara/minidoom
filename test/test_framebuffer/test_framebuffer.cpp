#include <string.h>
#include <unity.h>
#include "ssd1306_framebuffer.h"

using namespace SSD1306Framebuffer;

void test_page_layout_and_bit_order() {
    uint8_t buffer[SIZE];
    memset(buffer, 0, sizeof(buffer));

    TEST_ASSERT_EQUAL_UINT16(0, byteIndex(0, 0));
    TEST_ASSERT_EQUAL_UINT16(127, byteIndex(127, 0));
    TEST_ASSERT_EQUAL_UINT16(128, byteIndex(0, 8));
    TEST_ASSERT_EQUAL_UINT16(1023, byteIndex(127, 63));
    TEST_ASSERT_EQUAL_UINT8(0x01, bitMask(0));
    TEST_ASSERT_EQUAL_UINT8(0x80, bitMask(7));
    TEST_ASSERT_EQUAL_UINT8(0x01, bitMask(8));

    setPixel(buffer, 0, 0, 1);
    setPixel(buffer, 127, 63, 1);
    setPixel(buffer, 5, 15, 1);
    TEST_ASSERT_EQUAL_UINT8(0x01, buffer[0]);
    TEST_ASSERT_EQUAL_UINT8(0x80, buffer[1023]);
    TEST_ASSERT_EQUAL_UINT8(0x80, buffer[133]);
    TEST_ASSERT_EQUAL_UINT8(1, getPixel(buffer, 5, 15));
}

void test_clear_and_out_of_bounds_access() {
    uint8_t guarded[SIZE + 2];
    memset(guarded, 0xA5, sizeof(guarded));
    uint8_t* buffer = guarded + 1;

    setPixel(buffer, 10, 10, 1);
    TEST_ASSERT_EQUAL_UINT8(1, getPixel(buffer, 10, 10));
    setPixel(buffer, 10, 10, 0);
    TEST_ASSERT_EQUAL_UINT8(0, getPixel(buffer, 10, 10));

    setPixel(buffer, 128, 0, 1);
    setPixel(buffer, 0, 64, 1);
    setPixel(buffer, 255, 255, 1);
    TEST_ASSERT_EQUAL_UINT8(0, getPixel(buffer, 128, 0));
    TEST_ASSERT_EQUAL_UINT8(0, getPixel(buffer, 0, 64));
    TEST_ASSERT_EQUAL_UINT8(0xA5, guarded[0]);
    TEST_ASSERT_EQUAL_UINT8(0xA5, guarded[SIZE + 1]);

    setPixel(nullptr, 0, 0, 1);
    TEST_ASSERT_EQUAL_UINT8(0, getPixel(nullptr, 0, 0));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_page_layout_and_bit_order);
    RUN_TEST(test_clear_and_out_of_bounds_access);
    return UNITY_END();
}
