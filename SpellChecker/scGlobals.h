//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : scGlobals.h
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
// Name:        scGlobals.h
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: scGlobals.h 31 2014-02-22 15:19:28Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------
static const wxString s_plugName( wxT( "SpellCheck" ) );
static const wxString s_spOptions( wxT( "SpellCheckOptions" ) );
static const wxString s_noEditor( wxT( "There is no active editor\n" ) );
static const wxString s_codeLite( wxT( "CodeLite" ) );
static const wxString s_userDict( wxT("userwords.dict") );
static const wxString s_cppDelimiters( wxT( " \t\r\n.,?!@#$%^&*()-=+[]{}\\|_;:\"\'<>/~0123456789" ) );
static const wxString s_commentDelimiters( wxT( " \t\r\n.,?!@#$%^&*()-=+[]{}\\|;:\"\'<>/" ) );
static const wxString s_defDelimiters( wxT( " \t\r\n.,?!@#$%^&*()-=+[]{}\\|;:\"\'<>/~0123456789" ) );

static const wxString s_include( wxT( "#include" ) );
static const wxString s_wsRegEx( wxT( "(\\\\[^\\\\])" ) );

static const wxString s_doCheckID(wxT("do_spell_check"));
static const wxString s_contCheckID(wxT("do_continuous_check"));

static const wxString s_PLACE_HOLDER     = "@#)(";
static const wxString s_DOUBLE_BACKSLASH = "\\\\";
        
//------------------------------------------------------------

