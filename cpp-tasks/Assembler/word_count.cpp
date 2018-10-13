#include <iostream>
#include <emmintrin.h>

size_t count_simple(std::string& input) {
    bool is_ws = true;
    size_t result = 0;
    for (size_t i = 0; i < input.size(); i++) {
        if (is_ws && input[i] != ' ') {
            result++;
        }
        is_ws = (input[i] == ' ');
    }
    return result;
}

size_t count_asm(std::string& input) {
    if (input == "")
        return 0;
    const char *text = input.c_str();
    size_t length = input.length();
    
    auto simple_count = [](const char * text, size_t size, bool is_ws) {
        size_t result = 0;
        for (size_t i = 0; i < size; i++) {
            if (is_ws && text[i] != ' ') {
                result++;
            }
            is_ws = (text[i] == ' ');
        }
        return result;
    };
    
    int result = 0;
    if (length >= 32) {
        __m128i spaces = _mm_set_epi8(' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        __m128i curr, next, tmp;
        __asm__ volatile(
                         "movdqa     (%0), %1\n" // next = text
                         "pcmpeqb    %2, %1\n" // next ^= spaces
                         : "=r" (text),"=x" (next), "=x" (spaces)
                         : "0" (text), "1" (next), "2" (spaces)
                         : "memory", "cc"
                         );
        
        for (; length >= 32; length -= 16) {
            int32_t a;
            __asm__ volatile(
                             "add        $16,  %0\n" // сдвиг
                             "movdqa     %3, %1\n" // curr = next
                             "movdqa     (%0), %2\n" // tmp = text
                             
                             "pcmpeqb    %4, %2\n" // tmp ^= spaces
                             "movdqa     %2, %3\n" // next = tmp
                             
                             "palignr    $1, %1, %2\n" // tmp = shift 1 tmp cur
                             "pandn      %1, %2\n" // tmp = !tmp & curr
                             "pmovmskb   %2, %5\n" // a = pack tmp
                             : "=r" (text), "=x" (curr), "=x" (tmp), "=x" (next), "=x" (spaces), "=r" (a)
                             : "0" (text), "1" (curr), "2" (tmp), "3" (next), "4" (spaces), "5" (a)
                             : "memory", "cc"
                             );
            result += __builtin_popcount(a);
        }
    }
    
    std::cout << text << " " << result << std::endl;
    return result + simple_count(text + 1, length - 1, (text[0] == ' '));
}

int main() {
    std::string str = " fff hthpfc ethth   d e a v gergreg        gerge       ferfef    gtgtgr         ";
    auto begin = std::clock();
    std::cout << count_simple(str) << " time: " << std::clock() - begin << std::endl;
    begin = std::clock();
    std::cout << count_asm(str) << " time: " << std::clock() - begin << std::endl;
    return 0;
}
