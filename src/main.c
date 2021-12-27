#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "base45.h"
#include "comp.h"
#include "cbor.h"

#define MAX_CERT 1024
#define HC_PREFIX "HC1:"

static void get_line(FILE *f, char *line, int max)
{
    memset(line, 0, max);

    for(int i = 0; i < (max-1); i++)
    {
        int c = fgetc(f);
        if(c < 0 || c > 0xFF || c == '\r' || c == '\n')
            return;
        line[i] = c;
    }
}

static int parse_line(char *line)
{
    const char *PREFIX = HC_PREFIX;
    const int PREFIX_LEN = strlen(PREFIX);

    if(strlen(line) < 10 || strncmp(line, PREFIX, PREFIX_LEN))
    {
        printf("invlid string\n");
        return 1;
    }

    uint8_t bin[MAX_CERT];
    int bin_len = b45_decode(&line[PREFIX_LEN], bin);

    if(bin_len < 10)
    {
        printf("base45 decode error\n");
        return 1;
    }

    uint8_t cbor[MAX_CERT];
    memset(cbor, 0, MAX_CERT);
    int cbor_len = decomp(bin, bin_len, cbor, MAX_CERT);

    if(cbor_len < 10)
    {
        printf("inflate error\n");
        FILE *t = fopen("test", "w");
        fwrite(bin, bin_len, 1, t);
        fclose(t);
        return 1;
    }

    struct cbor_node *node = cbor_walk(cbor, cbor_len);

    printf("CWT\n---\n");
    cbor_debug(node);

    if(node &&
      node->type == CBOR_TAG &&
      node->child->type == CBOR_ARRAY &&
      node->child->length == 4)
    {
        struct cbor_node *header = node->child->child;
        if(header->type == CBOR_BA)
        {
            printf("CWT header\n---\n");
            cbor_debug(cbor_walk(header->v.string, header->length));
        }

        struct cbor_node *payload = node->child->child->next->next;
        if(payload->type == CBOR_BA)
        {
            printf("Payload\n---\n");
            cbor_debug(cbor_walk(payload->v.string, payload->length));
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s file\n", argv[0]);
        return 1;
    }
    
    const char *file = argv[1];
    FILE *f = fopen(file, "r");
    if(!f)
    {
        printf("Cannot open file\n");
        return 1;
    }

    while(!feof(f))
    {
        char line[MAX_CERT];
        get_line(f, line, sizeof(line));

        if(strlen(line))
            parse_line(line);
        
        printf("\n\n\n\n");
    }

    fclose(f);
    f = NULL;

    return 0;
}
