//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : filechecklist.h
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

#ifndef __filechecklist__
#define __filechecklist__

#include <vector>
#include <wx/filename.h>
#include "filechecklistbase.h"

class FileCheckList : public FileCheckListBase
{
    std::vector<std::pair<wxFileName,bool> > m_files;
    wxFileName m_baseDir;

protected:
    // Handlers for filechecklistbase events.
    void OnCheckAll( wxCommandEvent& event );
    void OnClearAll( wxCommandEvent& event );
    void OnFileSelected( wxCommandEvent& event );
    void OnFileCheckChanged( wxCommandEvent& event );

    void ShowFilePath(size_t n);

public:
    /** Constructor */
    FileCheckList( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    ~FileCheckList() = default;

    void SetCancellable(bool can) {
        m_buttonCancel->Show(can);
    }
    void SetCaption(const wxString &caption);
    void SetBaseDir(const wxFileName &dir);

    void SetFiles(const std::vector<std::pair<wxFileName, bool> > &files);
    const std::vector<std::pair<wxFileName,bool> >  &GetFiles() const {
        return m_files;
    }
};

#endif // __filechecklist__
