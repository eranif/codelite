//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SpellCheckerSettings.h
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
// Name:        SpellCheckerSettings.h
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: SpellCheckerSettings.h 35 2014-02-22 18:18:49Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
#ifndef __SpellCheckerSettings__
#define __SpellCheckerSettings__
// ------------------------------------------------------------
#include "wxcrafter.h"
// ------------------------------------------------------------
class IHunSpell;
// ------------------------------------------------------------
/** Implementing SpellCheckerSettings_base */
class SpellCheckerSettings : public SpellCheckerSettings_base
{
protected:
    // Handlers for SpellCheckerSettings_base events.
    void OnInitDialog( wxInitDialogEvent& event );
    void OnLanguageSelected( wxCommandEvent& event );
    void OnUpdateOk( wxUpdateUIEvent& event );
    void OnOk( wxCommandEvent& event );
    void OnDirChanged( wxFileDirPickerEvent& event );
    void OnClearIgnoreList( wxCommandEvent& event );

    void FillLanguageList();

    IHunSpell* m_pHunspell;
    wxString   m_dictionaryFileName;
    wxString   m_dictionaryPath;
    bool       m_scanStrings;
    bool       m_scanCPP;
    bool       m_scanC;
    bool       m_scanD1;
    bool       m_scanD2;
    bool       m_caseSensitiveUserDictionary;
    bool       m_ignoreSymbolsInTagsDatabase;

public:
    const wxString& GetDictionaryPath() const {
        return m_dictionaryPath;
    }
    const wxString& GetDictionaryFileName() const {
        return m_dictionaryFileName;
    }
    void SetDictionaryFileName( const wxString& dictionaryFileName ) {
        this->m_dictionaryFileName = dictionaryFileName;
    }
    void SetDictionaryPath( const wxString& dictionaryPath );
    void            SetHunspell( IHunSpell* pHunspell ) {
        this->m_pHunspell = pHunspell;
    }
    void            SetScanC( const bool& scanC ) {
        this->m_scanC = scanC;
    }
    void            SetScanCPP( const bool& scanCPP ) {
        this->m_scanCPP = scanCPP;
    }
    void            SetScanD1( const bool& scanD1 ) {
        this->m_scanD1 = scanD1;
    }
    void            SetScanD2( const bool& scanD2 ) {
        this->m_scanD2 = scanD2;
    }
    void            SetScanStrings( const bool& scanStrings ) {
        this->m_scanStrings = scanStrings;
    }
    void            SetCaseSensitiveUserDictionary( const bool& caseSensitiveUserDictionary ) {
        this->m_caseSensitiveUserDictionary = caseSensitiveUserDictionary;
	}
    void            SetIgnoreSymbolsInTagsDatabase( const bool& ignoreSymbolsInTagsDatabase ) {
        this->m_ignoreSymbolsInTagsDatabase = ignoreSymbolsInTagsDatabase;
    }

    bool GetScanC() const {
        return m_scanC;
    }
    bool GetScanCPP() const {
        return m_scanCPP;
    }
    bool GetScanD1() const {
        return m_scanD1;
    }
    bool GetScanD2() const {
        return m_scanD2;
    }
    bool GetScanStrings() const {
        return m_scanStrings;
    }
    bool GetCaseSensitiveUserDictionary() const {
        return m_caseSensitiveUserDictionary;
	}
    bool GetIgnoreSymbolsInTagsDatabase() const {
        return m_ignoreSymbolsInTagsDatabase;
    }
    /** Constructor */
    SpellCheckerSettings( wxWindow* parent );
    virtual ~SpellCheckerSettings();
};

#endif // __SpellCheckerSettings__
