//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
static const wxString s_plugName("SpellCheck");
static const wxString s_spOptions("SpellCheckOptions");
static const wxString s_noEditor(wxTRANSLATE("There is no active editor\n"));
static const wxString s_codeLite("CodeLite");
static const wxString s_userDict("userwords.dict");
static const wxString s_cppDelimiters(" \t\r\n.,?!@#$%^&*()-=+[]{}\\|_;:\"\'<>/~0123456789`");
static const wxString s_commentDelimiters(" \t\r\n.,?!@#$%^&*()-=+[]{}\\|;:\"\'<>/");
static const wxString s_defDelimiters(" \t\r\n.,?!@#$%^&*()-=+[]{}\\|;:\"\'<>/~0123456789`");
static const wxString s_dectHex("^0[xX]([0-9a-f]+|[0-9A-F]+)$");

static const wxString s_include("#include");
static const wxString s_wsRegEx("(\\\\[^\\\\])");

static const wxString s_doCheckID("do_spell_check");
static const wxString s_contCheckID("do_continuous_check");

static const wxString s_PLACE_HOLDER = "@#)(";
static const wxString s_DOUBLE_BACKSLASH = "\\\\";

//------------------------------------------------------------
