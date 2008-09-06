/////////////////////////////////////////////////////////////////////////////
// Name:        swGlobals.h
// Purpose:     
// Author:      Frank Lichtner
// Modified by: 
// Created:     09/02/08
// RCS-ID:      
// Copyright:   2008 Frank Lichtner
// Licence:		GNU General Public Licence    
/////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------

#define FRLSNIPWIZ_VERSION 1000
#define FRLSNIPWIZ_HEADER _T("Code snippet wizard file")
//------------------------------------------------------------
static const wxString snippetSet = wxT( "Snippets" );
static const wxString surroundSet = wxT( "Surrounds" );

static const wxString eol[3] = { wxT( "\r\n" ), wxT( "\r" ), wxT( "\n" ) };
static const wxString defaultFile = wxT( "SnipWiz.snip" );
static const wxString defaultTmplFile = wxT( "SnipWiz.tmpl" );
static const wxString plugName = wxT( "SnipWiz" );

static const wxString noEditor = wxT( "There is no active editor\n" );
static const wxString codeLite = wxT( "CodeLite" );
