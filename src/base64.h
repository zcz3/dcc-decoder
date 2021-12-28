#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>

int b64_encode_size(int len);
int b64_encode(uint8_t * const in, int len, char *out);

#endif