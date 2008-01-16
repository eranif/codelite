///////////////////////////////////////////////////////////////////////////////
// Name:        codec.cpp
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2006-12-06
// RCS-ID:      $$
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence + RSA Data Security license
///////////////////////////////////////////////////////////////////////////////

/// \file codec.cpp Implementation of MD5, RC4 and AES algorithms

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */

#include "codec.h"

// ----------------
// MD5 by RSA
// ----------------

// C headers for MD5
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MD5_HASHBYTES 16

/// Structure representing an MD5 context while ecrypting. (For internal use only)
typedef struct MD5Context
{
  unsigned int buf[4];
  unsigned int bits[2];
  unsigned char in[64];
} MD5_CTX;

static void  MD5Init(MD5_CTX *context);
static void  MD5Update(MD5_CTX *context, unsigned char const *buf, unsigned len);
static void  MD5Final(unsigned char digest[MD5_HASHBYTES], MD5_CTX *context);
static void  MD5Transform(unsigned int buf[4], unsigned int const in[16]);

#if 0
static char* MD5End(MD5_CTX *, char *);

static char* MD5End(MD5_CTX *ctx, char *buf)
{
  int i;
  unsigned char digest[MD5_HASHBYTES];
  char hex[]="0123456789abcdef";

  if (!buf)
  {
    buf = (char *)malloc(33);
  }
    
  if (!buf)
  {
    return 0;
  }
    
  MD5Final(digest,ctx);
  for (i=0;i<MD5_HASHBYTES;i++)
  {
    buf[i+i] = hex[digest[i] >> 4];
    buf[i+i+1] = hex[digest[i] & 0x0f];
  }
  buf[i+i] = '\0';
  return buf;
}
#endif

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static void MD5Final(unsigned char digest[16], MD5_CTX *ctx)
{
  unsigned count;
  unsigned char *p;

  /* Compute number of bytes mod 64 */
  count = (ctx->bits[0] >> 3) & 0x3F; 

  /* Set the first char of padding to 0x80.  This is safe since there is
     always at least one byte free */
  p = ctx->in + count;
  *p++ = 0x80;

  /* Bytes of padding needed to make 64 bytes */
  count = 64 - 1 - count;

  /* Pad out to 56 mod 64 */
  if (count < 8)
  {
    /* Two lots of padding:  Pad the first block to 64 bytes */
    memset(p, 0, count);
    MD5Transform(ctx->buf, (unsigned int *) ctx->in);

    /* Now fill the next block with 56 bytes */
    memset(ctx->in, 0, 56);
  }
  else
  {
    /* Pad block to 56 bytes */
    memset(p, 0, count - 8);   
  }

  /* Append length in bits and transform */
  ((unsigned int *) ctx->in)[14] = ctx->bits[0];
  ((unsigned int *) ctx->in)[15] = ctx->bits[1];

  MD5Transform(ctx->buf, (unsigned int *) ctx->in);
  memcpy(digest, ctx->buf, 16);
  memset((char *) ctx, 0, sizeof(ctx));       /* In case it's sensitive */
}

static void MD5Init(MD5_CTX *ctx)
{
  ctx->buf[0] = 0x67452301;
  ctx->buf[1] = 0xefcdab89;
  ctx->buf[2] = 0x98badcfe;
  ctx->buf[3] = 0x10325476;

  ctx->bits[0] = 0;
  ctx->bits[1] = 0;
}

static void MD5Update(MD5_CTX *ctx, unsigned char const *buf, unsigned len)
{
  unsigned int t;

  /* Update bitcount */

  t = ctx->bits[0];
  if ((ctx->bits[0] = t + ((unsigned int) len << 3)) < t)
  {
        ctx->bits[1]++;         /* Carry from low to high */
  }
  ctx->bits[1] += len >> 29;

  t = (t >> 3) & 0x3f;        /* Bytes already in shsInfo->data */

  /* Handle any leading odd-sized chunks */

  if (t)
  {
    unsigned char *p = (unsigned char *) ctx->in + t;

    t = 64 - t;
    if (len < t)
    {
      memcpy(p, buf, len);
      return;
    }
    memcpy(p, buf, t);
    MD5Transform(ctx->buf, (unsigned int *) ctx->in);
    buf += t;
    len -= t;
  }
  /* Process data in 64-byte chunks */

  while (len >= 64)
  {
    memcpy(ctx->in, buf, 64);
    MD5Transform(ctx->buf, (unsigned int *) ctx->in);
    buf += 64;
    len -= 64;
  }

  /* Handle any remaining bytes of data. */

  memcpy(ctx->in, buf, len);
}


/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))   
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
        ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void MD5Transform(unsigned int buf[4], unsigned int const in[16])
{
  register unsigned int a, b, c, d;

  a = buf[0];
  b = buf[1];
  c = buf[2];
  d = buf[3];

  MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7); 
  MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
  MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
  MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
  MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7); 
  MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
  MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
  MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22); 
  MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);  
  MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12); 
  MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
  MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
  MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7); 
  MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
  MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
  MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

  MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);  
  MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);  
  MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
  MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20); 
  MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);  
  MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9); 
  MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
  MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20); 
  MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);  
  MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9); 
  MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14); 
  MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20); 
  MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
  MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);  
  MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
  MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

  MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
  MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
  MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
  MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
  MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);  
  MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11); 
  MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16); 
  MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
  MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4); 
  MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11); 
  MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16); 
  MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23); 
  MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);  
  MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
  MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
  MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23); 

  MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
  MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
  MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
  MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21); 
  MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6); 
  MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10); 
  MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
  MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21); 
  MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);  
  MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
  MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15); 
  MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
  MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);  
  MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
  MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15); 
  MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21); 

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}
 

#ifndef REVERSEBYTE
#define byteReverse(buf, len)   /* Nothing */
#else
void byteReverse(unsigned char *buf, unsigned longs);

/*
 * Note: this code is harmless on little-endian machines.
 */
static void byteReverse(unsigned char *buf, unsigned longs)
{
  unsigned int t;
  do
  {
    t = (unsigned int) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
        ((unsigned) buf[1] << 8 | buf[0]);
    *(unsigned int *) buf = t;  
    buf += 4;
  }
  while (--longs);
}
#endif

// ---------------------------
// RC4 implementation
// ---------------------------

/**
* RC4 is the standard encryption algorithm used in PDF format
*/

void
Codec::RC4(unsigned char* key, int keylen,
           unsigned char* textin, int textlen,
           unsigned char* textout)
{
  int i;
  int j;
  int t;
  unsigned char rc4[256];

  for (i = 0; i < 256; i++)
  {
    rc4[i] = i;
  }
  j = 0;
  for (i = 0; i < 256; i++)
  {
    t = rc4[i];
    j = (j + t + key[i % keylen]) % 256;
    rc4[i] = rc4[j];
    rc4[j] = t;
  }

  int a = 0;
  int b = 0;
  unsigned char k;
  for (i = 0; i < textlen; i++)
  {
    a = (a + 1) % 256;
    t = rc4[a];
    b = (b + t) % 256;
    rc4[a] = rc4[b];
    rc4[b] = t;
    k = rc4[(rc4[a] + rc4[b]) % 256];
    textout[i] = textin[i] ^ k;
  }
}

void
Codec::GetMD5Binary(const unsigned char* data, int length, unsigned char* digest)
{
  MD5_CTX ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, data, length);
  MD5Final(digest,&ctx);
}

#define MODMULT(a, b, c, m, s) q = s / a; s = b * (s - a * q) - c * q; if (s < 0) s += m

void
Codec::GenerateInitialVector(int seed, unsigned char iv[16])
{
  unsigned char initkey[MD5_HASHBYTES];
  int j, q;
  int z = seed + 1;
  for (j = 0; j < 4; j++)
  {
    MODMULT(52774, 40692,  3791, 2147483399L, z);
    initkey[4*j+0] = 0xff &  z;
    initkey[4*j+1] = 0xff & (z >>  8);
    initkey[4*j+2] = 0xff & (z >> 16);
    initkey[4*j+3] = 0xff & (z >> 24);
  }
  GetMD5Binary((const unsigned char*) initkey, 16, iv);
}

void
Codec::AES(int page, bool encrypt, unsigned char encryptionKey[MD5_HASHBYTES],
           unsigned char* datain, int datalen, unsigned char* dataout)
{
  unsigned char initial[MD5_HASHBYTES];
  unsigned char pagekey[MD5_HASHBYTES];
  unsigned char nkey[MD5_HASHBYTES+4+4];
  int keyLength = MD5_HASHBYTES;
  int nkeylen = keyLength + 4 + 4;
  int j;
  for (j = 0; j < keyLength; j++)
  {
    nkey[j] = encryptionKey[j];
  }
  nkey[keyLength+0] = 0xff &  page;
  nkey[keyLength+1] = 0xff & (page >>  8);
  nkey[keyLength+2] = 0xff & (page >> 16);
  nkey[keyLength+3] = 0xff & (page >> 24);

  // AES encryption needs some 'salt'
  nkey[keyLength+4] = 0x73;
  nkey[keyLength+5] = 0x41;
  nkey[keyLength+6] = 0x6c;
  nkey[keyLength+7] = 0x54;

  GetMD5Binary(nkey, nkeylen, pagekey);
  GenerateInitialVector(page, initial);

  Rijndael::Direction direction = (encrypt) ? Rijndael::Encrypt : Rijndael::Decrypt;
  m_aes.init(Rijndael::CBC, direction, pagekey, Rijndael::Key16Bytes, initial);
  int len = 0;
  if (encrypt)
  {
    len = m_aes.blockEncrypt(datain, datalen*8, dataout);
  }
  else
  {
    len = m_aes.blockDecrypt(datain, datalen*8, dataout);
  }
  
  // It is a good idea to check the error code
  if (len < 0)
  {
    // AES: Error on encrypting.
  }
}

static unsigned char padding[] =
  "\x28\xBF\x4E\x5E\x4E\x75\x8A\x41\x64\x00\x4E\x56\xFF\xFA\x01\x08\x2E\x2E\x00\xB6\xD0\x68\x3E\x80\x2F\x0C\xA9\xFE\x64\x53\x69\x7A";

Codec::Codec()
{
  m_isEncrypted = false;
  m_hasReadKey  = false;
  m_hasWriteKey = false;
}

Codec::~Codec()
{
}

void
Codec::Copy(Codec& other)
{
  int j;
  m_isEncrypted = other.m_isEncrypted;
  m_hasReadKey  = other.m_hasReadKey;
  m_hasWriteKey = other.m_hasWriteKey;
  for (j = 0; j < 16; j++)
  {
    m_readKey[j]  = other.m_readKey[j];
    m_writeKey[j] = other.m_writeKey[j];
  }
  m_bt = other.m_bt;
  m_aes.invalidate();
}

void
Codec::CopyKey(bool read2write)
{
  int j;
  if (read2write)
  {
    for (j = 0; j < 16; j++)
    {
      m_writeKey[j] = m_readKey[j];
    }
  }
  else
  {
    for (j = 0; j < 16; j++)
    {
      m_readKey[j] = m_writeKey[j];
    }
  }
}

void
Codec::PadPassword(const char* password, int pswdlen, unsigned char pswd[32])
{
  int m = pswdlen;
  if (m > 32) m = 32;

  int j;
  int p = 0;
  for (j = 0; j < m; j++)
  {
    pswd[p++] = (unsigned char) password[j];
  }
  for (j = 0; p < 32 && j < 32; j++)
  {
    pswd[p++] = padding[j];
  }
}

void
Codec::GenerateReadKey(const char* userPassword, int passwordLength)
{
  GenerateEncryptionKey(userPassword, passwordLength, m_readKey);
}

void
Codec::GenerateWriteKey(const char* userPassword, int passwordLength)
{
  GenerateEncryptionKey(userPassword, passwordLength, m_writeKey);
}

void
Codec::GenerateEncryptionKey(const char* userPassword, int passwordLength, unsigned char encryptionKey[KEYLENGTH])
{
  unsigned char userPad[32];
  unsigned char ownerPad[32];
  unsigned char ownerKey[32];

  unsigned char mkey[MD5_HASHBYTES];
  unsigned char digest[MD5_HASHBYTES];
  int keyLength = MD5_HASHBYTES;
  int i, j, k;

  // Pad passwords
  PadPassword(userPassword, passwordLength, userPad);
  PadPassword("", 0, ownerPad);

  // Compute owner key

  MD5_CTX ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, ownerPad, 32);
  MD5Final(digest,&ctx);

  // only use for the input as many bit as the key consists of
  for (k = 0; k < 50; ++k)
  {
    MD5Init(&ctx);
    MD5Update(&ctx, digest, keyLength);
    MD5Final(digest,&ctx);
  }
  memcpy(ownerKey, userPad, 32);
  for (i = 0; i < 20; ++i)
  {
    for (j = 0; j < keyLength ; ++j)
    {
      mkey[j] = (digest[j] ^ i);
    }
    RC4(mkey, keyLength, ownerKey, 32, ownerKey);
  }

  // Compute encryption key

  MD5Init(&ctx);
  MD5Update(&ctx, userPad, 32);
  MD5Update(&ctx, ownerKey, 32);
  MD5Final(digest,&ctx);

  // only use the really needed bits as input for the hash
  for (k = 0; k < 50; ++k)
  {
    MD5Init(&ctx);
    MD5Update(&ctx, digest, keyLength);
    MD5Final(digest, &ctx);
  }
  memcpy(encryptionKey, digest, keyLength);
}

void
Codec::Encrypt(int page, unsigned char* data, int len, bool useWriteKey)
{
  unsigned char* key = (useWriteKey) ? m_writeKey : m_readKey;
  AES(page, true, key, data, len, data);

}

void
Codec::Decrypt(int page, unsigned char* data, int len)
{
  AES(page, false, m_readKey, data, len, data);
}

