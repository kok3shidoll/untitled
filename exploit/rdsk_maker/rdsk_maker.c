/*
 * rdsk_maker.c - rdsk maker for dra (iBoot-1940)
 * Copyright (c) 2021 - 2023 @ kok3shidoll
 *
 * [BUILD]
 * gcc rdsk_maker.c -o rdsk_maker
 *
 * [How to use]
 * ./rdsk_maker <in> <out> [iboot_p1.bin] [payload.bin]
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "plog.h"
#include "header.h"

int open_file(char *file, size_t *sz, void **buf)
{
    FILE *fd = fopen(file, "r");
    if (!fd)
    {
        ERR("opening %s", file);
        return -1;
    }
    
    fseek(fd, 0, SEEK_END);
    *sz = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    
    *buf = malloc(*sz);
    if (!*buf)
    {
        ERR("allocating file buffer");
        fclose(fd);
        return -1;
    }
    
    fread(*buf, *sz, 1, fd);
    fclose(fd);
    
    return 0;
}

int main(int argc, char **argv)
{
    
    if(argc != 5)
    {
        printf("%s <in> <out> [iboot_p1] [payload]\n", argv[0]);
        return 0;
    }
    
    char *infile = argv[1];
    char *outfile = argv[2];
    
    char *iboot_p1_path = argv[3];
    char *payload_path = argv[4];
    
    void *ramdisk = NULL;
    void *iboot_p1 = NULL;
    void *payload = NULL;
    
    size_t ramdisk_sz = 0;
    size_t iboot_p1_sz = 0;
    size_t payload_sz = 0;
    
    /* ramdisk */
    LOG("reading ramdisk");
    open_file(infile, &ramdisk_sz, &ramdisk);
    assert(ramdisk_sz == 0x80000);
    
    /* iboot_p1 */
    LOG("reading iboot_p1");
    open_file(iboot_p1_path, &iboot_p1_sz, &iboot_p1);
    assert(iboot_p1_sz <= 0x49000);
    
    /* payload */
    LOG("reading payload");
    open_file(payload_path, &payload_sz, &payload);
    assert(payload_sz <= 0x49000);
    
    
    memcpy((ramdisk + RDSK_PD_BASE), (iboot_p1 + EXPLOIT_BASE), EXPLOIT_BASE_SIZE);
    memcpy((ramdisk + RDSK_PD_SECOND_BASE), (iboot_p1 + EXPLOIT_SECOND_BASE), EXPLOIT_SECOND_BASE_SIZE);
    memcpy((ramdisk + RDSK_PD_SWAP_BASE), (iboot_p1 + EXPLOIT_SWAP_BASE), EXPLOIT_SWAP_BASE_SIZE);
    memcpy((ramdisk + RDSK_PD_SWAP_BASE + EXPLOIT_SWAP_BASE_SIZE), (payload + PAYLOAD_BASE), PAYLOAD_BASE_SIZE);
    
#ifdef HAVE_SWAP2
    memcpy((ramdisk + RDSK_PD_SWAP2_BASE), (iboot_p1 + EXPLOIT_SWAP2_BASE), EXPLOIT_SWAP2_BASE_SIZE);
#endif
    
    /* write */
    LOG("writing ramdisk");
    FILE *out = fopen(outfile, "w");
    if (!out)
    {
        ERR("opening %s", outfile);
        goto end;
    }
    
    fwrite(ramdisk, ramdisk_sz, 1, out);
    fflush(out);
    fclose(out);
    
end:
    if(ramdisk) free(ramdisk);
    if(iboot_p1) free(iboot_p1);
    if(payload) free(payload);
    
    return 0;
}
