#include "base64.h"

int b64_encode_size(int len)
{
    int l = (len / 3) * 4;
    if(len % 3) l++;
    return l + 1;
}

static char b64_char(int index)
{
    if(index < 0)
        return '!';

    if(index <= 25)
        return 'A' + index;

    index -= 26;

    if(index <= 25)
        return 'a' + index;

    index -= 26;

    if(index <= 9)
        return '0' + index;

    index -= 10;

    if(index == 0)
        return '+';

    if(index == 2)
        return '/';

    return '!';
}

int b64_encode(uint8_t * const in, int len, char *out)
{
    int olen = 0;

    for(int i = 0; i < len; )
    {
        int pad = 0;
        uint8_t b1 = in[i++];
        uint8_t b2 = 0;
        uint8_t b3 = 0;

        if(i < len)
        {
            b2 = in[i++];
            if(i < len)
                b3 = in[i++];
            else
                pad = 1;
        }
        else
        {
            pad = 2;
        }

        uint32_t n = (b1 << 16) + (b2 << 8) + b3;
        int c1 = (n >> 18) & 0b111111;
        int c2 = (n >> 12) & 0b111111;
        int c3 = (n >> 6)  & 0b111111;
        int c4 = n         & 0b111111;

        out[olen++] = b64_char(c1);
        out[olen++] = b64_char(c2);
        out[olen++] = pad > 1 ? '=' : b64_char(c3);
        out[olen++] = pad > 0 ? '=' : b64_char(c4);
    }

    out[olen++] = '\0';

    return olen;
}
