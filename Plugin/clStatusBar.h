//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clStatusBar.h
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

#ifndef CLSTATUSBAR_H
#define CLSTATUSBAR_H

#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wxCustomStatusBar.h" // Base class: wxCustomStatusBar

#include <wx/bitmap.h>

class IManager;
class WXDLLIMPEXP_SDK clStatusBar : public wxCustomStatusBar
{
    IManager* m_mgr;
    wxBitmap m_bmpBuildError;
    wxBitmap m_bmpBuildWarnings;
    wxBitmap m_bmpSourceControl;
    wxString m_sourceControlTabName;
    wxStringMap_t m_gotoAnythingTableThemes;
    wxStringMap_t m_gotoAnythingTableSyntax;

protected:
    void OnPageChanged(wxCommandEvent& event);
    void OnThemeChanged(wxCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void OnBuildStarted(clBuildEvent& event);
    void OnBuildEnded(clBuildEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnFieldClicked(clCommandEvent& event);
    void OnEditorSettingsChanged(wxCommandEvent& event);
    void OnGotoAnythingShowing(clGotoEvent& e);
    void OnActionSelected(clGotoEvent& e);
    void DoUpdateColour();
    void DoSetLinePosColumn(const wxString& message);

    void SetBuildBitmap(const wxBitmap& bmp, const wxString& tooltip);

    void DoUpdateView();
    void DoFieldClicked(int fieldIndex);
    int GetTextWidth(const wxString& text) const;

public:
    clStatusBar(wxWindow* parent, IManager* mgr);
    virtual ~clStatusBar();

    /**
     * @brief clear all text fields from the status bar
     */
    void Clear();

    /**
     * @brief set a status bar message
     */
    void SetMessage(const wxString& message, int secondsToLive = wxNOT_FOUND);

    /**
     * @brief set the whitespace information (Tabs vs Spaces)
     */
    void SetWhitespaceInfo();

    /**
     * @brief clear the whitespace info fields
     */
    void ClearWhitespaceInfo();

    /**
     * @brief update the language field
     */
    void SetLanguage(const wxString& lang);

    /**
     * @brief Update the encoding field
     */
    void SetEncoding(const wxString& enc);

    /**
     * @brief update the line / column / pos field
     */
    void SetLinePosColumn(const wxString& lineCol);

    /**
     * @brief start the animation
     * @param refreshRate refresh rate in milliseconds
     * @param tooltip set the tooltip for this field
     */
    void StartAnimation(long refreshRate, const wxString& tooltip);

    /**
     * @brief stop the animation
     */
    void StopAnimation();

    /**
     * @brief set a bitmap (16x16) in the source control section
     * and optionally, provide an output tab name to toggle when the bitmap is clicked
     */
    void SetSourceControlBitmap(const wxBitmap& bmp, const wxString& outputTabName, const wxString& tooltip);
};

#endif // CLSTATUSBAR_H
