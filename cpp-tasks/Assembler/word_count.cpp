#include <iostream>
#include <emmintrin.h>

size_t count_simple(const char * str, size_t size) {
    uint32_t ret = 0;
    bool prev_is_space = true;
    for(size_t i = 0; i < size; i++) {
        if(str[i] != ' ' && prev_is_space) {
            ret++;
            prev_is_space = false;
        } else if(str[i] == ' ') {
            prev_is_space = true;
        }
    }
    return ret;
}

size_t count_asm(std::string input) {
    bool ws_seq = false;
    const char *str = input.c_str();
    size_t size = input.length();
    
    int ans = (size == 0 || str[0] == 32) ? 0 : 1;
    size_t offset = (size_t)str % 16;
    if (offset != 0) {
        offset = 16 - offset;
        ans += count_simple(str, offset);
        str += offset;
        size -= offset;
    }
    
    __m128i spaces = _mm_set_epi8(32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32);
    __m128i curr, next, tmp;
    __asm__ volatile(
                     "movdqa     (%0), %1\n"
                     "pcmpeqb    %2, %1\n"
                     : "=r" (str),"=x" (next), "=x" (spaces)
                     : "0" (str), "1" (next), "2" (spaces)
                     : "memory", "cc"
                     );
    
    for (; size >= 32; size -= 16) {
        int32_t a;
        __asm__ volatile(
                         "add        $16,  %0\n"
                         "movdqa     %3, %1\n"
                         "movdqa     (%0), %2\n"
                         
                         "pcmpeqb    %4, %2\n"
                         "movdqa     %2, %3\n"
                         
                         "palignr    $1, %1, %2\n"
                         "pandn      %1, %2\n"
                         "pmovmskb   %2, %5\n"
                         : "=r" (str), "=x" (curr), "=x" (tmp), "=x" (next), "=x" (spaces), "=r" (a)
                         : "0" (str), "1" (curr), "2" (tmp), "3" (next), "4" (spaces), "5" (a)
                         : "memory", "cc"
                         );
        
    
        ans += __builtin_popcount(a);
    }
    ws_seq = false;
    return ans + count_simple(str, size);
}

int main() {
    std::string str = "hthpfc ethth  nmwhthqc yrqnd  wzlz d  qlnvmk  w g   iv  ic qa sertyuiopoiuytretyuiop[oiuytryuiopoiuytryuiop[]p oiytretyuiop[ptryuiop[poiuytuiop[oiuytuiopiuythth tuioiuyghjkopiuyujiokijughbo  d e a v                                                  ";
    auto begin = std::clock();
    std::cout << count_simple(str.c_str(), str.length()) << " time: " << std::clock() - begin << std::endl;
    begin = std::clock();
    std::cout << count_asm(str) << " time: " << std::clock() - begin << std::endl;
    return 0;
}
