//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : spellcheckeroptions.h
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

/////////////////////////////////////////////////////////////////////////////
// Name:        spellcheckeroptions.h
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     09/11/08
// RCS-ID:
// Copyright:   2008 Frank Lichtner
// Licence:
/////////////////////////////////////////////////////////////////////////////
#ifndef __spellcheckeroptions__
#define __spellcheckeroptions__
//------------------------------------------------------------
#include "serialized_object.h"
// ------------------------------------------------------------
class SpellCheckerOptions : public SerializedObject
{
public:
	SpellCheckerOptions();
	virtual ~SpellCheckerOptions();

	void DeSerialize( Archive& arch );
	void Serialize( Archive& arch );

	void            SetDictionaryPath( const wxString& dictionaryPath ) { this->m_dictionaryPath = dictionaryPath; }
	void            SetDictionaryFileName( const wxString& dictionary ) { this->m_dictionary = dictionary; }
	void            SetScanC( const bool& scanC ) { this->m_scanC = scanC; }
	void            SetScanCPP( const bool& scanCPP ) { this->m_scanCPP = scanCPP; }
	void            SetScanD1( const bool& scanD1 ) { this->m_scanD1 = scanD1; }
	void            SetScanD2( const bool& scanD2 ) { this->m_scanD2 = scanD2; }
	void            SetScanStr( const bool& scanStr ) { this->m_scanStr = scanStr; }
    void            SetCheckContinuous( const bool& checkContinuous ) { this->m_checkContinuous = checkContinuous; }
    void            SetCaseSensitiveUserDictionary( const bool& caseSensitiveUserDictionary ) { this->m_caseSensitiveUserDictionary = caseSensitiveUserDictionary; }
    void            SetIgnoreSymbolsInTagsDatabase( const bool& ignoreSymbolsInTagsDatabase ) { this->m_ignoreSymbolsInTagsDatabase = ignoreSymbolsInTagsDatabase; }
	bool            GetScanC() const { return m_scanC; }
	bool            GetScanCPP() const { return m_scanCPP; }
	bool            GetScanD1() const { return m_scanD1; }
	bool            GetScanD2() const { return m_scanD2; }
	bool            GetScanStr() const { return m_scanStr; }
	const wxString& GetDictionaryPath() const { return m_dictionaryPath; }
	const wxString& GetDictionaryFileName() const { return m_dictionary; }
    bool            GetCheckContinuous() const { return m_checkContinuous; }
    bool            GetCaseSensitiveUserDictionary() const { return m_caseSensitiveUserDictionary; }
    bool            GetIgnoreSymbolsInTagsDatabase() const { return m_ignoreSymbolsInTagsDatabase; }

protected:
	wxString m_dictionary;
	wxString m_dictionaryPath;
	bool     m_scanStr;
	bool     m_scanCPP;
	bool     m_scanC;
	bool     m_scanD1;
	bool     m_scanD2;
    bool     m_checkContinuous;
    bool     m_caseSensitiveUserDictionary;
    bool     m_ignoreSymbolsInTagsDatabase;
};
//------------------------------------------------------------
#endif // __spellcheckeroptions__
