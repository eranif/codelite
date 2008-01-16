#ifndef SQLITE_OMIT_DISKIO
#ifdef SQLITE_HAS_CODEC

#include "codec.h"

extern "C"
void sqlite3_activate_see(const char *info)
{
}

// Free the encryption data structure associated with a pager instance.
// (called from the modified code in pager.c) 
extern "C"
void sqlite3pager_free_codecarg(void *pCodecArg)
{
  if (pCodecArg)
  {
    delete (Codec*) pCodecArg;
  }
}

// Encrypt/Decrypt functionality, called by pager.c
extern "C"
void* sqlite3Codec(void* pCodecArg, void* data, Pgno nPageNum, int nMode)
{
  if (pCodecArg == NULL)
  {
    return data;
  }
  Codec* codec = (Codec*) pCodecArg;
  if (!codec->IsEncrypted())
  {
    return data;
  }
  
  int pageSize = sqlite3BtreeGetPageSize(codec->GetBtree());

  switch(nMode)
  {
    case 0: // Undo a "case 7" journal file encryption
    case 2: // Reload a page
    case 3: // Load a page
      if (codec->HasReadKey())
      {
        codec->Decrypt(nPageNum, (unsigned char*) data, pageSize);
      }
      break;

    case 6: // Encrypt a page for the main database file
      if (codec->HasWriteKey())
      {
        unsigned char* pageBuffer = codec->GetPageBuffer();
        memcpy(pageBuffer, data, pageSize);
        data = pageBuffer;
        codec->Encrypt(nPageNum, (unsigned char*) data, pageSize, true);
      }
      break;

    case 7: // Encrypt a page for the journal file
      /* Under normal circumstances, the readkey is the same as the writekey.  However,
         when the database is being rekeyed, the readkey is not the same as the writekey.
         The rollback journal must be written using the original key for the
         database file because it is, by nature, a rollback journal.
         Therefore, for case 7, when the rollback is being written, always encrypt using
         the database's readkey, which is guaranteed to be the same key that was used to
         read the original data.
      */
      if (codec->HasReadKey())
      {
        unsigned char* pageBuffer = codec->GetPageBuffer();
        memcpy(pageBuffer, data, pageSize);
        data = pageBuffer;
        codec->Encrypt(nPageNum, (unsigned char*) data, pageSize);
      }
      break;
  }
  return data;
}

extern "C" void* sqlite3pager_get_codecarg(Pager *pPager);

extern "C"
int sqlite3CodecAttach(sqlite3* db, int nDb, const void* zKey, int nKey)
{
  // Attach a key to a database.
  Codec* codec = new Codec();

  // No key specified, could mean either use the main db's encryption or no encryption
  if (zKey == NULL || nKey <= 0)
  {
    // No key specified
    if (nDb != 0 && nKey < 0)
    {
      Codec* mainCodec = (Codec*) sqlite3pager_get_codecarg(sqlite3BtreePager(db->aDb[0].pBt));
      // Attached database, therefore use the key of main database, if main database is encrypted
      if (mainCodec != NULL && mainCodec->IsEncrypted())
      {
        codec->Copy(*mainCodec);
        codec->SetBtree(db->aDb[nDb].pBt);
        sqlite3pager_set_codec(sqlite3BtreePager(db->aDb[nDb].pBt), sqlite3Codec, codec);
      }
      else
      {
        codec->SetIsEncrypted(false);
        delete codec;
      }
    }
  }
  else
  {
    // Key specified, setup encryption key for database
    codec->SetIsEncrypted(true);
    codec->SetHasReadKey(true);
    codec->SetHasWriteKey(true);
    codec->GenerateReadKey((const char*) zKey, nKey);
    codec->CopyKey(true);
    codec->SetBtree(db->aDb[nDb].pBt);
    sqlite3pager_set_codec(sqlite3BtreePager(db->aDb[nDb].pBt), sqlite3Codec, codec);
  }
  return SQLITE_OK;
}

extern "C"
void sqlite3CodecGetKey(sqlite3* db, int nDb, void** zKey, int* nKey)
{
  // The unencrypted password is not stored for security reasons
  // therefore always return NULL
  *zKey = NULL;
  *nKey = -1;
}

extern "C"
int sqlite3_key(sqlite3 *db, const void *zKey, int nKey)
{
  // The key is only set for the main database, not the temp database
  return sqlite3CodecAttach(db, 0, zKey, nKey);
}

extern "C"
int sqlite3_rekey(sqlite3 *db, const void *zKey, int nKey)
{
  // Changes the encryption key for an existing database.
  int rc = SQLITE_ERROR;
  Btree* pbt = db->aDb[0].pBt;
  Pager* pPager = sqlite3BtreePager(pbt);
  Codec* codec = (Codec*) sqlite3pager_get_codecarg(pPager);

  if ((zKey == NULL || nKey == 0) && (codec == NULL || !codec->IsEncrypted()))
  {
    // Database not encrypted and key not specified
    // therefore do nothing
    return SQLITE_OK;
  }

  if (codec == NULL || !codec->IsEncrypted())
  {
    // Database not encrypted, but key specified
    // therefore encrypt database
    if (codec == NULL) codec = new Codec();

    codec->SetIsEncrypted(true);
    codec->SetHasReadKey(false); // Original database is not encrypted
    codec->SetHasWriteKey(true);
    codec->GenerateWriteKey((const char*) zKey, nKey);
    codec->SetBtree(pbt);
    sqlite3pager_set_codec(pPager, sqlite3Codec, codec);
  }
  else if (zKey == NULL || nKey == 0)
  {
    // Database encrypted, but key not specified
    // therefore decrypt database
    // Keep read key, drop write key
    codec->SetHasWriteKey(false);
  }
  else
  {
    // Database encrypted and key specified
    // therefore re-encrypt database with new key
    // Keep read key, change write key to new key
    codec->GenerateWriteKey((const char*) zKey, nKey);
    codec->SetHasWriteKey(true);
  }

  // Start transaction
  rc = sqlite3BtreeBeginTrans(pbt, 1);
  if (!rc)
  {
    // Rewrite all pages using the new encryption key (if specified)
    Pgno nPage = sqlite3pager_pagecount(pPager);
    int pageSize = sqlite3BtreeGetPageSize(pbt);
    Pgno nSkip = PAGER_MJ_PGNO(pageSize);
    void *pPage;
    Pgno n;

    for (n = 1; rc == SQLITE_OK && n <= nPage; n++)
    {
      if (n == nSkip) continue;
      rc = sqlite3pager_get(pPager, n, &pPage);
      if (!rc)
      {
        rc = sqlite3pager_write(pPage);
        sqlite3pager_unref(pPage);
      }
    }
  }

  if (rc == SQLITE_OK)
  {
    // Commit transaction if all pages could be rewritten
    rc = sqlite3BtreeCommit(pbt);
  }
  if (rc != SQLITE_OK)
  {
    // Rollback in case of error
    sqlite3BtreeRollback(pbt);
  }

  if (rc == SQLITE_OK)
  {
    // Set read key equal to write key if necessary
    if (codec->HasWriteKey())
    {
      codec->CopyKey(false);
      codec->SetHasReadKey(true);
    }
    else
    {
      codec->SetIsEncrypted(false);
    }
  }
  else
  {
    // Restore write key if necessary
    if (codec->HasReadKey())
    {
      codec->CopyKey(true);
    }
    else
    {
      codec->SetIsEncrypted(false);
    }
  }

  if (!codec->IsEncrypted())
  {
    // Remove codec for unencrypted database
    sqlite3pager_set_codec(pPager, NULL, NULL);
    delete codec;
  }
  return rc;
}

#endif // SQLITE_HAS_CODEC

#endif // SQLITE_OMIT_DISKIO
