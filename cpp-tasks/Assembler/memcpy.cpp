#include <ctime>
#include <iostream>
#include <emmintrin.h>

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
                          "movdqu (%1), %0\n"
                          "movntdq %0, (%2)\n"
                          :"=x"(reg)
                          :"r"((const char *)from + i), "r"((char *)to + i)
                          :"memory");
    }
    
    for (size_t i = (sz - aligned_sz) ; i < sz ; i++)
        *((char *)to + i) = *((char *)from + i);
    _mm_sfence();
}

bool check(void const* a, void const* b, size_t size) {
    for(size_t i = 0; i < size; i++)
        if(*((char*) a + i) != *((char*) b + i))
            return false;
    return true;
}

int main() {
    char *from = new char[N];
    char *to = new char[N];
    
    auto begin = std::clock();
    memcpy_simple(to, from, N);
    std::cout << (double)(std::clock() - begin) / CLOCKS_PER_SEC << std::endl;
    if(!check(from, to, N))
        std::cout << "Memory isn't equal" << ::std::endl;
    
    from = new char[N];
    to = new char[N];
    begin = std::clock();
    memcpy_asm(to, from, N);
    std::cout << (double)(std::clock() - begin) / CLOCKS_PER_SEC << std::endl;
    if(!check(from, to, N))
        std::cout << "Memory isn't equal" << ::std::endl;
    return 0;
}
