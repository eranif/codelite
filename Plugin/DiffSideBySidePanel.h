//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : DiffSideBySidePanel.h
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

#ifndef DIFFSIDEBYSIDEPANEL_H
#define DIFFSIDEBYSIDEPANEL_H

#include "wxcrafter_plugin.h"
#include <wx/filename.h>
#include <vector>
#include "clDTL.h"
#include "DiffConfig.h"

class WXDLLIMPEXP_SDK DiffSideBySidePanel : public DiffSideBySidePanelBase
{
    enum {
        ID_COPY_LEFT_TO_RIGHT = wxID_HIGHEST + 1,
        ID_COPY_LEFT_TO_RIGHT_AND_MOVE,
        ID_COPY_RIGHT_TO_LEFT,
        ID_COPY_RIGHT_TO_LEFT_AND_MOVE,
    };

    typedef std::vector< int > Markers_t;
public:
    struct FileInfo {
        wxFileName filename;
        wxString title;
        bool readOnly;
        bool deleteOnExit;

        FileInfo(const wxFileName& fn, const wxString &caption, bool ro) : filename(fn), title(caption), readOnly(ro), deleteOnExit(false) {}
        FileInfo() : readOnly(true), deleteOnExit(false) {}
    };

    enum {
        kDeleteLeftOnExit    = 0x00000001,
        kDeleteRightOnExit   = 0x00000002,
        kLeftReadOnly        = 0x00000004,
        kRightReadOnly       = 0x00000008,
        kOriginSourceControl = 0x00000010,
    };

protected:
    virtual void OnSingleUI(wxUpdateUIEvent& event);
    virtual void OnSingleView(wxRibbonButtonBarEvent& event);
    virtual void OnCopyLeftToRightMenu(wxRibbonButtonBarEvent& event);
    virtual void OnCopyRightToLeftMenu(wxRibbonButtonBarEvent& event);
    virtual void OnLeftPickerUI(wxUpdateUIEvent& event);
    virtual void OnRightPickerUI(wxUpdateUIEvent& event);

    void OnMenuCopyLeft2Right(wxCommandEvent &event);
    void OnMenuCopyRight2Left(wxCommandEvent &event);

    Markers_t m_leftRedMarkers;
    Markers_t m_leftGreenMarkers;
    Markers_t m_leftPlaceholdersMarkers;

    Markers_t m_rightGreenMarkers;
    Markers_t m_rightRedMarkers;
    Markers_t m_rightPlaceholdersMarkers;

    std::vector< std::pair<int, int> > m_sequences; // start-line - end-line pairs
    int m_cur_sequence;

    size_t m_flags;
    wxString m_leftCaption;
    wxString m_rightCaption;
    DiffConfig m_config;
    
protected:
    wxString DoGetContentNoPlaceholders(wxStyledTextCtrl *stc) const;
    bool IsLeftReadOnly() const {
        return m_flags & kLeftReadOnly;
    }
    bool IsRightReadOnly() const {
        return m_flags & kRightReadOnly;
    }
    bool IsDeleteLeftOnExit() const {
        return m_flags & kDeleteLeftOnExit;
    }
    bool IsDeleteRightOnExit() const {
        return m_flags & kDeleteRightOnExit;
    }
    bool IsOriginSourceControl() const {
        return m_flags & kOriginSourceControl;
    }

protected:
    virtual void OnRefreshDiffUI(wxUpdateUIEvent& event);
    virtual void OnHorizontal(wxRibbonButtonBarEvent& event);
    virtual void OnHorizontalUI(wxUpdateUIEvent& event);
    virtual void OnVertical(wxRibbonButtonBarEvent& event);
    virtual void OnVerticalUI(wxUpdateUIEvent& event);
    virtual void OnCopyFileFromRight(wxRibbonButtonBarEvent& event);
    virtual void OnCopyFileLeftToRight(wxRibbonButtonBarEvent& event);
    virtual void OnSaveChanges(wxRibbonButtonBarEvent& event);
    virtual void OnSaveChangesUI(wxUpdateUIEvent& event);
    virtual void OnCopyLeftToRight(wxRibbonButtonBarEvent& event);
    virtual void OnCopyRightToLeft(wxRibbonButtonBarEvent& event);
    virtual void OnCopyLeftToRightUI(wxUpdateUIEvent& event);
    virtual void OnCopyRightToLeftUI(wxUpdateUIEvent& event);
    virtual void OnNextDiffUI(wxUpdateUIEvent& event);
    virtual void OnPrevDiffUI(wxUpdateUIEvent& event);
    virtual void OnNextDiffSequence(wxRibbonButtonBarEvent& event);
    virtual void OnPrevDiffSequence(wxRibbonButtonBarEvent& event);
    virtual void OnRefreshDiff(wxRibbonButtonBarEvent& event);
    virtual void OnLeftStcPainted(wxStyledTextEvent& event);
    virtual void OnRightStcPainted(wxStyledTextEvent& event);
    void OnPageClosing(wxNotifyEvent &event);

    void PrepareViews();
    void UpdateViews(const wxString &left, const wxString &right);
    void DoClean();
    void DoDrawSequenceMarkers(int firstLine, int lastLine, wxStyledTextCtrl* ctrl);
    void DoCopyCurrentSequence(wxStyledTextCtrl* from, wxStyledTextCtrl* to);
    void DoCopyFileContent(wxStyledTextCtrl* from, wxStyledTextCtrl* to);
    void DoGetPositionsToCopy(wxStyledTextCtrl* stc, int& startPos, int& endPos, int& placeHolderMarkerFirstLine, int& placeHolderMarkerLastLine);
    void DoSave(wxStyledTextCtrl* stc, const wxFileName& fn);

    bool CanNextDiff();
    bool CanPrevDiff();
    void DefineMarkers( wxStyledTextCtrl* ctrl );

public:
    DiffSideBySidePanel(wxWindow* parent);
    virtual ~DiffSideBySidePanel();
    
    void DoLayout();
    /**
     * @brief display a diff view for 2 files left and right
     */
    void Diff();

    /**
     * @brief mark the current diff origin from source control
     */
    void SetOriginSourceControl() {
        m_flags |= kOriginSourceControl;
    }

    /**
     * @brief start a new empty diff
     */
    void DiffNew();

    /**
     * @brief set the initial files to diff
     * Once set, you should call Diff() function
     */
    void SetFilesDetails(const DiffSideBySidePanel::FileInfo& leftFile, const DiffSideBySidePanel::FileInfo& rightFile);
};
#endif // DIFFSIDEBYSIDEPANEL_H
