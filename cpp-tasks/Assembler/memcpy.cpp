#include <ctime>
#include <iostream>
#include <emmintrin.h>
#include <vector>

const size_t N = (1 << 30);

void memcpy_simple(void *to, const void *from, size_t sz) {
    for (size_t i = 0; i < sz; i++)
        *((char *) (to) + i) = *((const char *) (from) + i);
}

void memcpy_asm(void *to, const void *from, size_t sz) {
    size_t cur_pos = 0;
    while(((size_t)to + cur_pos) % 16 != 0 && cur_pos < sz) {
        *((char *)to + cur_pos) = *((char *)from + cur_pos);
        cur_pos++;
    }
    
    size_t aligned_sz = (sz - cur_pos) % 16;
    for (size_t i = cur_pos; i < sz - aligned_sz; i += 16) {
        __m128i reg;
        __asm__ volatile (
                          "movdqu (%1, %2), %0\n"
                          "movntdq %0, (%2, %3)\n"
                          :"=x"(reg)
                          :"r"((const char *)from), "r"(i), "r"((char*)to)
                          :"memory");
    }
    
    for (size_t i = (sz - aligned_sz) ; i < sz ; i++)
        *((char *)to + i) = *((char *)from + i);
    _mm_sfence();
}

bool check(std::vector<char> & a, std::vector<char> & b) {
    for(size_t i = 0; i < a.size(); i++)
        if(a[i] != b[i])
            return false;
    return true;
}

int main() {
    //char *from = new char[N];
   // char *to = new char[N];
    std::vector<char> from(N);
    std::vector<char> to(N);
    
    for(;;) {
    auto begin = std::clock();
    memcpy_simple(to.data(), from.data(), N);
    std::cout << (double)(std::clock() - begin) / CLOCKS_PER_SEC << std::endl;
    if(!check(from, to))
        std::cout << "Memory isn't equal" << ::std::endl;
    
    begin = std::clock();
    memcpy_asm(to.data(), from.data(), N);
    std::cout << (double)(std::clock() - begin) / CLOCKS_PER_SEC << std::endl;
    if(!check(from, to))
        std::cout << "Memory isn't equal" << ::std::endl;
    }
    return 0;
}
