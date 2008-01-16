///////////////////////////////////////////////////////////////////////////////
// Name:        codec.h
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2006-12-06
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file codec.h Interface of the codec class

#ifndef _CODEC_H_
#define _CODEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../sqliteInt.h"

// ATTENTION: Macro similar to that in pager.c
// TODO: Check in case of new version of SQLite
#include "../os.h"
#define PAGER_MJ_PGNO(x) ((PENDING_BYTE/(x))+1)

#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif

#include "rijndael.h"

#define KEYLENGTH 16

class Codec
{
public:
  Codec();
  ~Codec();
  void Copy(Codec& other);

  void GenerateReadKey(const char* userPassword, int passwordLength);

  void GenerateWriteKey(const char* userPassword, int passwordLength);

  /// Encrypt data
  void Encrypt(int page, unsigned char* data, int len, bool useWriteKey = false);

  /// Decrypt data
  void Decrypt(int page, unsigned char* data, int len);

  void CopyKey(bool read2write);

  void SetIsEncrypted(bool isEncrypted) { m_isEncrypted = isEncrypted; }
  void SetHasReadKey(bool hasReadKey) { m_hasReadKey = hasReadKey; }
  void SetHasWriteKey(bool hasWriteKey) { m_hasWriteKey = hasWriteKey; }
  void SetBtree(Btree* bt) { m_bt = bt; }

  bool IsEncrypted() { return m_isEncrypted; }
  bool HasReadKey() { return m_hasReadKey; }
  bool HasWriteKey() { return m_hasWriteKey; }
  Btree* GetBtree() { return m_bt; }
  unsigned char* GetPageBuffer() { return &m_page[4]; }

protected:
  /// Generate encryption key
  void GenerateEncryptionKey(const char* userPassword, int passwordLength, unsigned char encryptionKey[KEYLENGTH]);

  void PadPassword(const char* password, int pswdlen, unsigned char pswd[32]);

  void RC4(unsigned char* key, int keylen,
           unsigned char* textin, int textlen,
           unsigned char* textout);

  void GetMD5Binary(const unsigned char* data, int length, unsigned char* digest);
  
  void GenerateInitialVector(int seed, unsigned char iv[16]);

  void AES(int page, bool encrypt, unsigned char encryptionKey[KEYLENGTH],
           unsigned char* datain, int datalen, unsigned char* dataout);

private:
  bool          m_isEncrypted;
  bool          m_hasReadKey;
  unsigned char m_readKey[16];
  bool          m_hasWriteKey;
  unsigned char m_writeKey[16];
  Rijndael      m_aes;

	Btree*        m_bt; // Pointer to B-tree used by DB
  unsigned char m_page[SQLITE_MAX_PAGE_SIZE+8];
};

#endif
