//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : swGlobals.h              
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

static const wxString noEditor = _("There is no active editor\n");
static const wxString codeLite = wxT("CodeLite");
