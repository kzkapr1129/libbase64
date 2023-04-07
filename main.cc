#include "base64.h"
#include <stdio.h>
#include <string.h>
 
 
 
struct TestData {
    const char* data;
    const unsigned long max;
    unsigned char expect_bin[255];
    const unsigned long expect_return;
};
 
const struct TestData test_patterns[] = {
    // 正常系
    {"FPucA9l+", 255, {0x14, 0xfb, 0x9c, 0x03, 0xd9, 0x7e}, 6},
    {"FPucA9k=", 255, {0x14, 0xfb, 0x9c, 0x03, 0xd9}, 5},
    {"FPucAw==", 255, {0x14, 0xfb, 0x9c, 0x03}, 4},
 
    // 空文字列
    {"", 255, {}, 0},
 
    // maxがnegative
    {"FPucA9l+", -1, {}, 0},
 
    // maxが不足
    {"FPucA9k=", 2, {}, 0},
    {"FPucAw==", 3, {}, 0},
 
    // base64がnull
    {NULL, 255, {}, 0}
};
 
const size_t num_test_patterns = sizeof(test_patterns) / sizeof(struct TestData);
 
int main() {
    unsigned long i, j;
    unsigned char bin[255];
    unsigned long result;
 
    for (i = 0; i < num_test_patterns; i++) {
        const struct TestData td = test_patterns[i];
        printf("test[%ld]: base64=\"%s\" return=%ld\n", i, td.data, td.expect_return);
 
        result = Base64Decode(td.data, bin, td.max);
 
        if (result != td.expect_return) {
            printf("FAIL: [%ld] return=%ld expect=%ld\n", i, result, td.expect_return);
            return -1;
        }
 
        if (0 < result) {
            for (j = 0; j < td.expect_return; j++) {
                if (bin[j] != td.expect_bin[j]) {
                    printf("FAIL: [%ld] bin[%ld]=0x%x expect=0x%x\n", i, j, bin[j], td.expect_bin[j]);
                    return -1;
                }
            }
        }
    }
 
    printf("ALL PASS\n");
 
    return 0;
}