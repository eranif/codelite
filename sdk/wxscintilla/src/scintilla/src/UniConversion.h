// Scintilla source code edit control
/** @file UniConversion.h
 ** Functions to handle UFT-8 and UCS-2 strings.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#ifdef WXDLLIMPEXP_SCI
#undef WXDLLIMPEXP_SCI
#endif

#include <wx/string.h>
#ifdef WXMAKINGDLL_SCI
	#define WXDLLIMPEXP_SCI WXEXPORT
#elif defined(WXUSINGDLL_SCI)
	#define WXDLLIMPEXP_SCI WXIMPORT
#else // not making nor using DLL
	#define WXDLLIMPEXP_SCI
#endif

WXDLLIMPEXP_SCI unsigned int UTF8Length(const wchar_t *uptr, unsigned int tlen);
void UTF8FromUTF16(const wchar_t *uptr, unsigned int tlen, char *putf, unsigned int len);
unsigned int UTF8CharLength(unsigned char ch);
unsigned int UTF16Length(const char *s, unsigned int len);
unsigned int UTF16FromUTF8(const char *s, unsigned int len, wchar_t *tbuf, unsigned int tlen);

// [CHANGED] BEGIN
void UTF8FromUCS2(const wchar_t *uptr, unsigned int tlen, char *putf, unsigned int len);
unsigned int UCS2FromUTF8(const char *s, unsigned int len, wchar_t *tbuf, unsigned int tlen);
unsigned int UCS2Length(const char *s, unsigned int len);
// [CHANGED] END
