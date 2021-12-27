#include "base45.h"

static int b45_val(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';

    if(c >= 'A' && c <= 'Z')
        return c - 'A' + 10;

    int i = 36;
    char syms[] = {' ', '$', '%', '*', '+', '-', '.', '/', ':', 0};

    for(char *s = syms;
        *s;
        s++, i++)
        if(c == *s) return i;

    return -1;
}

int b45_decode(char * const in, uint8_t *out)
{
    int len = 0;

    for(int i = 0; in[i]; )
    {
        // Need to have 3 chars available
        // or 2 chars then end of string
        if(!in[i+1]) return -1;
        int end = !in[i+2];

        int c = b45_val(in[i++]);
        int d = b45_val(in[i++]);
        int e = in[i] ? b45_val(in[i++]) : 0;

        if(c < 0 || d < 0 || e < 0) return -1;

        uint32_t n = c + d * 45 + e * 45 * 45;
        if(!end) out[len++] = (n >> 8) & 0xFF;
        out[len++] = n & 0xFF;
    }

    return len;
}
