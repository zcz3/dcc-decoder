#include "cbor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static struct cbor_node *cbor_new()
{
    struct cbor_node *node = malloc(sizeof(struct cbor_node));
    memset(node, 0, sizeof(struct cbor_node));
    return node;
}

void cbor_delete(struct cbor_node *node)
{
    if(!node)
        return;

    cbor_delete(node->child);
    cbor_delete(node->next);

    if(node->type == CBOR_BA || node->type == CBOR_STRING)
        if(node->v.string)
            free(node->v.string);

    free(node);
}

static struct cbor_node *cbor_next(uint8_t *data, uint32_t len, uint32_t *consumed)
{
    uint32_t c = 0;

    if(len < 1)
    {
        printf("Not enough bytes\n");
        return NULL;
    }

    int type = (data[0] >> 5) & 0b111;
    int arg = data[0] & 0b11111;
    uint32_t argx = 0;
    int argx_l = 0;
    c++;

    if(arg < 24)
    {
        argx = arg;
    }
    else if(arg == 24)
    {
        argx_l = 1;
    }
    else if(arg == 25)
    {
        argx_l = 2;
    }
    else if(arg == 26)
    {
        argx_l = 4;
    }
    else
    {
        printf("Can't parse arg > 26\n");
        return NULL;
    }
    
    if(argx_l)
    {
        if(len <= argx_l)
        {
            printf("Not enough bytes for arg\n");
            return NULL;
        }

        for(int i = 0; i < argx_l; i++)
            argx = (argx << 8) + data[i+1];
        
        c += argx_l;
    }

    struct cbor_node *node = cbor_new();
    node->length = argx;

    if(type == 0 || type == 1)
    {
        if(argx & (1 << 31))
        {
            printf("Integer too large\n");
            goto error;
        }

        node->type = CBOR_INT;
        node->v.integer = (type == 0) ? argx : -1-(int)argx;
    }

    if(type == 2 || type == 3)
    {
        if((len - c) < argx)
        {
            printf("Not enough bytes for string or ba\n");
            goto error;
        }

        if(argx > 1024)
        {
            printf("String too big\n");
            goto error;
        }

        node->type = (type == 2) ? CBOR_BA : CBOR_STRING;
        node->v.string = malloc(argx+1);
        memcpy(node->v.string, &data[c], argx);
        node->v.string[argx] = '\0';

        c += argx;
    }

    if(type == 4)
        node->type = CBOR_ARRAY;

    if(type == 5)
        node->type = CBOR_MAP;

    if(type == 6)
    {
        node->type = CBOR_TAG;

        if(argx > 0xFFFF)
        {
            printf("Can't parse tag > 0xFFFF\n");
            goto error;
        }

        node->v.integer = argx;
    }

    if(type == 7)
    {
        if(argx == 20)
        {
            node->type = CBOR_BOOL;
            node->v.boolean = 0;
        }
        else if(argx == 21)
        {
            node->type = CBOR_BOOL;
            node->v.boolean = 1;
        }
        else if(argx == 22)
            node->type = CBOR_NULL;
        else if(argx == 23)
            node->type = CBOR_UNDEFINED;
        else
        {
            printf("Can't parse float\n");
            goto error;
        }
    }

    *consumed += c;
    return node;

error:
    cbor_delete(node);
    return NULL;
}

static struct cbor_node *cbor_next_recurse(uint8_t *data, uint32_t len, uint32_t *consumed)
{
    uint32_t c = 0;

    struct cbor_node *node = cbor_next(data, len, &c);

    if(!node)
        return NULL;
    
    if(node->type == CBOR_ARRAY)
    {
        struct cbor_node **n = &node->child;

        for(int i = 0; i < node->length; i++)
        {
            *n = cbor_next_recurse(&data[c], len-c, &c);
            if(!*n)
            {
                printf("Could not get all array elements\n");
                goto error;
            }

            n = &(*n)->next;
        }
    }

    if(node->type == CBOR_MAP)
    {
        struct cbor_node **n = &node->child;

        for(int i = 0; i < node->length; i++)
        {
            struct cbor_node *k = cbor_next(&data[c], len-c, &c);

            if(!k)
            {
                printf("Could not get key\n");
                goto error;
            }

            if(k->type != CBOR_INT && k->type != CBOR_STRING)
            {
                printf("Key must be integer or string\n");
                cbor_delete(k);
                goto error;
            }

            struct cbor_node *v = cbor_next_recurse(&data[c], len-c, &c);

            if(!v)
            {
                printf("Could not get value\n");
                cbor_delete(k);
                goto error;
            }

            k->child = v;
            *n = k;
            n = &k->next;
        }
    }

    if(node->type == CBOR_TAG)
    {
        node->child = cbor_next_recurse(&data[c], len-c, &c);

        if(!node->child)
        {
            printf("Could not get tag content\n");
            goto error;
        }
    }

    *consumed += c;
    return node;

error:
    cbor_delete(node);
    return NULL;

}

struct cbor_node *cbor_walk(uint8_t *data, uint32_t len)
{
    uint32_t c = 0;
    struct cbor_node *node = cbor_next_recurse(data, len, &c);

    if(node && c != len)
    {
        printf("Did not consume all bytes\n");
        cbor_delete(node);
    }

    return node;
}

static void cbor_debug_inner(struct cbor_node *node, int indent, int is_map)
{
    if(!node)
        return;

    for(int i = 0; i < indent; i++)
        printf("  ");

    switch(node->type)
    {
        case CBOR_INVALID: printf("(invalid)\n"); break;
        case CBOR_INT: printf("(int) %d\n", node->v.integer); break;
        case CBOR_BA: printf("(ba) %d\n", node->length); break;
        case CBOR_STRING: printf("(string) \"%s\"\n", node->v.string); break;
        case CBOR_ARRAY: printf("(array) %d\n", node->length); break;
        case CBOR_MAP: printf("(map) %d\n", node->length); break;
        case CBOR_TAG: printf("(tag) %d\n", node->v.integer); break;
        case CBOR_FLOAT: printf("(float) %lf\n", node->v.fp); break;
        case CBOR_BOOL: printf("(bool) %s\n", node->v.boolean ? "true" : "false"); break;
        case CBOR_NULL: printf("(null)\n"); break;
        case CBOR_UNDEFINED: printf("(undefined)\n"); break;
    }

    if(is_map)
    {
        cbor_debug_inner(node->child, indent+1, 0);
    }
    else
    {
        if(node->type == CBOR_ARRAY)
            cbor_debug_inner(node->child, indent+1, 0);
        
        if(node->type == CBOR_MAP)
            cbor_debug_inner(node->child, indent+1, 1);

        if(node->type == CBOR_TAG)
            cbor_debug_inner(node->child, indent+1, 0);
    }

    cbor_debug_inner(node->next, indent, is_map);
}

void cbor_debug(struct cbor_node *node)
{
    cbor_debug_inner(node, 0, 0);
}
