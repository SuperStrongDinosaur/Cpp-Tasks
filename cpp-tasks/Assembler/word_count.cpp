#include <iostream>
#include <tmmintrin.h>

uint32_t count_simple(std::string const& str) {
    uint32_t ret = 0;
    bool prev_is_space = true;
    for(size_t i = 0; i < str.size(); i++) {
        if(str[i] != ' ' && prev_is_space) {
            ret++;
            prev_is_space = false;
        } else if(str[i] == ' ') {
            prev_is_space = true;
        }
    }
    return ret;
}

uint32_t count_asm(std::string const& str) {
    if(str.length() < 64)
        return count_simple(str);

    uint32_t ans = 0;
    size_t sz = str.length();
    char const *s = str.c_str();
    
    __m128i space_reg = _mm_set_epi8(32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32);
    __m128i cmp_result, cmp_result_sh, store = _mm_set_epi32(0, 0, 0, 0), tmp_reg;
    
    size_t cur_pos = 0;
    bool is_ws = false;
    
    while((size_t) (s + cur_pos) % 16 != 0 && cur_pos < sz) {
        char cur_symbol = *(s + cur_pos);
        if(is_ws && cur_symbol != ' ')
            ans++;
        is_ws = (cur_symbol == ' ');
        cur_pos++;
    }
    
    if ((is_ws && *(s + cur_pos) != ' ' && cur_pos != 0) || (cur_pos == 0 && *s != ' '))
        ans++;
    if(cur_pos != 0 && *s != ' ')
        ans++;
    
    size_t size = sz - (sz - cur_pos) % 16;
    
    __asm__("movdqa (%2), %1\n"
            "pcmpeqb %1, %0"
            :"=x"(cmp_result_sh), "=x"(tmp_reg)
            :"r"(s + cur_pos), "0"(space_reg));
    
    for (size_t i = cur_pos; i < size - 16 ; i += 16) {
        
        cmp_result = cmp_result_sh;
        uint32_t m;
        
        __m128i tmp04, tmp05, tmp06, tmp07;
        __asm__("movdqa (%7), %3\n"
                "pcmpeqb %3, %0\n"
                "movdqa %0, %6\n"
                "palignr $1, %4, %6\n"
                "pandn %4, %6\n"
                "psubsb %6, %5\n"
                "paddusb %5, %1\n"
                "pmovmskb %1, %2"
                :"=x"(cmp_result_sh), "=x"(store), "=r"(m), "=x"(tmp04), "=x"(tmp05), "=x"(tmp06), "=x"(tmp07)
                :"r"(s + i + 16), "0"(space_reg), "1"(store), "4"(cmp_result), "5"(_mm_set_epi32(0, 0, 0, 0)));
        
        if (m != 0) {
            __m128i abs_low, tt;
            uint32_t high, low;
            
            __asm__("psadbw %3, %0\n"
                    "movd %0, %2\n"
                    "movhlps %0, %0\n"
                    "movd %0, %1\n"
                    :"=x" (tt), "=r"(high), "=r"(low), "=x"(abs_low)
                    :"0"(_mm_set_epi32(0, 0, 0, 0)), "3"(store)
                    :"0");
            
            ans += high + low;
            store = _mm_set_epi32(0, 0, 0, 0);
        }
        
    }
    
    __m128i abs_low, tt;
    uint32_t high, low;
    
    __asm__("psadbw %3, %0\n"
            "movd %0, %2\n"
            "movhlps %0, %0\n"
            "movd %0, %1\n"
            :"=x" (tt), "=r"(high), "=r"(low), "=x"(abs_low)
            :"0"(_mm_set_epi32(0, 0, 0, 0)), "3"(store)
            :"0");
    
    ans += high + low;
    store = _mm_set_epi32(0, 0, 0, 0);
    
    cur_pos = size - 16;
    
    if(*(s + cur_pos- 1) == ' ' && *(s + cur_pos) != ' ')
        ans--;
    
    is_ws = *(s + size - 17) == ' ';
    for (size_t i = (size - 16) ; i < sz ; i++)  {
        if (*(s + i) != ' ' && is_ws)
            ans++;
        is_ws = *(s + i) == ' ';
    }
    return ans;
}

int main() {
    std::string str = "      pfc e  nmwqc yrqnd  wzlz d  qlnvmk  w g   iv  ic qa sertyuiopoiuytretyuiop[oiuytryuiopoiuytryuiop[]poiytretyuiop[ptryuiop[poiuytuiop[oiuytuiopiuytuioiuyghjkopiuyujiokijughbo  d e a v                                                  ";
    auto begin = std::clock();
    std::cout << count_simple(str) << " time: " << std::clock() - begin << std::endl;
    begin = std::clock();
    std::cout << count_asm(str) << " time: " << std::clock() - begin << std::endl;
    return 0;
}
