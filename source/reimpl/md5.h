//
// Created by Lisen on 2025/11/2.
//

#ifndef ACRODEA_MD5_H
#define ACRODEA_MD5_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int MD5_u32;

struct MD5_CTX {
    MD5_u32 lo, hi;
    MD5_u32 a, b, c, d;
    unsigned char buffer[64];
    MD5_u32 block[16];
};

void MD5_Init(struct MD5_CTX *ctx);
void MD5_Update(struct MD5_CTX *ctx, const void *buf, unsigned long len);
void MD5_Final(unsigned char digest[16], struct MD5_CTX *ctx);

#ifdef __cplusplus
}
#endif


#endif //ACRODEA_MD5_H
