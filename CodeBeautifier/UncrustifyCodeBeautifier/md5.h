//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : md5.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * @file md5.h
 * A simple class for MD5 calculation
 *
 * @author  Ben Gardner
 * @license GPL v2+
 */
#ifndef MD5_H_INCLUDED
#define MD5_H_INCLUDED

#include "base_types.h"

class MD5
{
public:
   MD5();
   ~MD5() { }

   void Init();
   void Update(const void *data, UINT32 len);

   void Final(UINT8 digest[16]);

   /* internal function */
   static void Transform(UINT32 buf[4], UINT32 in_data[16]);

   static void Calc(const void *data, UINT32 length, UINT8 digest[16]);

private:
   UINT32 m_buf[4];
   UINT32 m_bits[2];
   UINT8  m_in[64];
   bool   m_need_byteswap;
   bool   m_big_endian;

   void reverse_u32(UINT8 *buf, int n_u32);
};

#endif /* MD5_H_INCLUDED */
