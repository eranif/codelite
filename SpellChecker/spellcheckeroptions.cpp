//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : spellcheckeroptions.cpp
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
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: spellcheckeroptions.cpp 35 2014-02-22 18:18:49Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
#include "spellcheckeroptions.h"
#include <cl_standard_paths.h>
// ------------------------------------------------------------
SpellCheckerOptions::SpellCheckerOptions()
{
    m_dictionary.Empty();
    m_dictionaryPath.Empty();
    m_scanStr = true;
    m_scanCPP = false;
    m_scanC   = false;
    m_scanD1  = false;
    m_scanD2  = false;
    m_checkContinuous = false;
    m_caseSensitiveUserDictionary = true;
    m_ignoreSymbolsInTagsDatabase = false;

    wxString defaultDicsDir;
    defaultDicsDir << clStandardPaths::Get().GetDataDir() << wxFILE_SEP_PATH << "dics";
    m_dictionaryPath = defaultDicsDir;
}

// ------------------------------------------------------------
SpellCheckerOptions::~SpellCheckerOptions()
{
}

// ------------------------------------------------------------
void SpellCheckerOptions::DeSerialize( Archive& arch )
{
    arch.Read( wxT( "m_dictionary" ), m_dictionary );
    arch.Read( wxT( "m_dictionaryPath" ), m_dictionaryPath );
    arch.Read( wxT( "m_scanStr" ), m_scanStr );
    arch.Read( wxT( "m_scanCPP" ), m_scanCPP );
    arch.Read( wxT( "m_scanC" ), m_scanC );
    arch.Read( wxT( "m_scanD1" ), m_scanD1 );
    arch.Read( wxT( "m_scanD2" ), m_scanD2 );
    arch.Read( wxT( "m_checkContinuous" ), m_checkContinuous );
    arch.Read( wxT( "m_caseSensitiveUserDictionary" ), m_caseSensitiveUserDictionary );
    arch.Read( wxT( "m_ignoreSymbolsInTagsDatabase" ), m_ignoreSymbolsInTagsDatabase );
}

// ------------------------------------------------------------
void SpellCheckerOptions::Serialize( Archive& arch )
{
    arch.Write( wxT( "m_dictionary" ), m_dictionary );
    arch.Write( wxT( "m_dictionaryPath" ), m_dictionaryPath );
    arch.Write( wxT( "m_scanStr" ), m_scanStr );
    arch.Write( wxT( "m_scanCPP" ), m_scanCPP );
    arch.Write( wxT( "m_scanC" ), m_scanC );
    arch.Write( wxT( "m_scanD1" ), m_scanD1 );
    arch.Write( wxT( "m_scanD2" ), m_scanD2 );
    arch.Write( wxT( "m_checkContinuous" ), m_checkContinuous );
    arch.Write( wxT( "m_caseSensitiveUserDictionary" ), m_caseSensitiveUserDictionary );
    arch.Write( wxT( "m_ignoreSymbolsInTagsDatabase" ), m_ignoreSymbolsInTagsDatabase );
}
// ------------------------------------------------------------
