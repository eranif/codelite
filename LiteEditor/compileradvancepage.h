//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : compileradvancepage.h
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

#ifndef __compileradvancepage__
#define __compileradvancepage__

#include "compiler_pages.h"
#include "advanced_settings.h"
#include "compiler.h"

class CompilerAdvancePage : public CompilerAdvanceBase, public ICompilerSubPage
{
    wxString m_cmpname;
public:
    CompilerAdvancePage( wxWindow* parent, const wxString &cmpname );
    virtual void Save(CompilerPtr cmp);
    
protected:
    virtual void OnEditIncludePaths(wxCommandEvent& event);
    virtual void OnEditLibraryPaths(wxCommandEvent& event);
};

#endif // __compileradvancepage__
