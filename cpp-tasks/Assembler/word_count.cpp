#include <iostream>
#include <emmintrin.h>

size_t count_simple(const char * text, size_t size, bool is_ws) {
    size_t result = 0;
    for (size_t i = 0; i < size; i++) {
        if (text[i] == 32) {
            is_ws = true;
        } else if (is_ws) {
            is_ws = false;
            result++;
        }
    }
    return result;
}

size_t count_simple(std::string input) {
    bool is_ws = false;
    return count_simple(input.c_str(), input.size(), is_ws) + ((input.size() == 0 || input[0] == 32) ? 0 : 1);
}

size_t count_asm(std::string& input) {
    bool is_ws = false;
    const char *text = input.c_str();
    size_t size = input.size();
    
    int result = (size == 0 || text[0] == 32) ? 0 : 1;
    size_t offset = (size_t)text % 16;
    if (offset != 0) {
        offset = 16 - offset;
        result += count_simple(text, offset, is_ws);
        text += offset;
        size -= offset;
    }
    
    __m128i spaces = _mm_set_epi8(32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32);
    __m128i curr, next, tmp;
    __asm__ volatile(
                     "movdqa     (%0), %1\n"
                     "pcmpeqb    %2, %1\n"
                     : "=r" (text),"=x" (next), "=x" (spaces)
                     : "0" (text), "1" (next), "2" (spaces)
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
                         : "=r" (text), "=x" (curr), "=x" (tmp), "=x" (next), "=x" (spaces), "=r" (a)
                         : "0" (text), "1" (curr), "2" (tmp), "3" (next), "4" (spaces), "5" (a)
                         : "memory", "cc"
                         );
        result += __builtin_popcount(a);
    }
    return result + count_simple(text, size, false);
}

int main() {
    std::string str = "  hthpfc ethth   d e a v                                fef                 ";
    auto begin = std::clock();
    std::cout << count_simple(str) << " time: " << std::clock() - begin << std::endl;
    begin = std::clock();
    std::cout << count_asm(str) << " time: " << std::clock() - begin << std::endl;
    return 0;
}
