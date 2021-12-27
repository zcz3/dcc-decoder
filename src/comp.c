#include "comp.h"
#include <string.h>
#include <zlib.h>

int decomp(uint8_t * const in, int in_len, uint8_t *out, int out_len)
{
    int len = -1;
    z_stream st;

    memset(&st, 0, sizeof(st));
    st.next_in = in;
    st.avail_in = in_len;
    st.next_out = out;
    st.avail_out = out_len;
    st.zalloc = Z_NULL;
    st.zfree = Z_NULL;

    if(inflateInit(&st) != Z_OK)
        return -1;

    int r = inflate(&st, Z_FINISH);

    if(r == Z_STREAM_END)
        // All good
        len = st.total_out;

    inflateEnd(&st);

    return len;
}