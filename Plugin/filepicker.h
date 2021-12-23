//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : filepicker.h
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
#ifndef FILEPICKER_H
#define FILEPICKER_H

#include "codelite_exports.h"
#include "wx/button.h"
#include "wx/filedlg.h"
#include "wx/panel.h"
#include "wx/textctrl.h"

/**
 * This control allows the user to select a directory. with a text control on its right side
 */
class WXDLLIMPEXP_SDK FilePicker : public wxPanel
{
    wxTextCtrl* m_path;
    wxButton* m_button;
    wxString m_buttonCaption;
    wxString m_dlgCaption;
    long m_dlgStyle;
    wxString m_defaultFile;
    wxString m_wildCard;

private:
    void CreateControls();
    void ConnectEvents();

protected:
    /**
     * Handle button click event
     * \param &event
     */
    virtual void OnButtonClicked(wxCommandEvent& event);

public:
    /**
     * \param parent control parent
     * \param id control id
     * \param message the dialog message
     * \param buttonCaption the browse button caption
     * \param style control style, same as wxFileDialog styles
     */
    FilePicker(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& defaultFile = wxEmptyString,
               const wxString& message = _("Select a file:"), const wxString& wildCard = wxT("*.*"),
               const wxString& buttonCaption = _("Browse"), const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = wxFD_DEFAULT_STYLE);

    virtual ~FilePicker();

    /**
     * \return the path
     */
    wxString GetPath() const { return m_path->GetValue(); }

    /**
     * set a value into the text control
     * \param path path to set
     */
    void SetPath(const wxString& path) { m_path->SetValue(path); }
};

#endif // FILEPICKER_H
