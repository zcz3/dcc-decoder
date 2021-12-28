#ifndef CBOR_H
#define CBOR_H

#include <stdint.h>

enum cbor_type {
    CBOR_INVALID,
    CBOR_INT,
    CBOR_BA,
    CBOR_STRING,
    CBOR_ARRAY,
    CBOR_MAP,
    CBOR_TAG,
    CBOR_FLOAT,
    CBOR_BOOL,
    CBOR_NULL,
    CBOR_UNDEFINED,
};

struct cbor_node {
    enum cbor_type type;
    uint32_t length;

    union {
        int integer;
        uint8_t *string;
        double fp;
        int boolean;
    } v;

    struct cbor_node *child;
    struct cbor_node *next;
};

struct cbor_node *cbor_walk(uint8_t *data, uint32_t len);

void cbor_free(struct cbor_node *node);

void cbor_debug(struct cbor_node *node);

#endif