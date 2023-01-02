//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#include "DiffConfig.h"
#include "clDTL.h"
#include "clPluginsFindBar.h"
#include "clToolBar.h"
#include "wxcrafter_plugin.h"

#include <vector>
#include <wx/filename.h>

class WXDLLIMPEXP_SDK DiffSideBySidePanel : public DiffSideBySidePanelBase
{
    enum {
        ID_COPY_LEFT_TO_RIGHT = wxID_HIGHEST + 1,
        ID_COPY_LEFT_TO_RIGHT_AND_MOVE,
        ID_COPY_RIGHT_TO_LEFT,
        ID_COPY_RIGHT_TO_LEFT_AND_MOVE,
    };

    typedef std::vector<int> Markers_t;

public:
    struct FileInfo {
        wxFileName filename;
        wxString title;
        bool readOnly;
        bool deleteOnExit;

        FileInfo(const wxFileName& fn, const wxString& caption, bool ro)
            : filename(fn)
            , title(caption)
            , readOnly(ro)
            , deleteOnExit(false)
        {
        }
        FileInfo()
            : readOnly(true)
            , deleteOnExit(false)
        {
        }
    };

    enum {
        kDeleteLeftOnExit = (1 << 0),
        kDeleteRightOnExit = (1 << 1),
        kLeftReadOnly = (1 << 2),
        kRightReadOnly = (1 << 3),
        kOriginSourceControl = (1 << 4),
        kSavePaths = (1 << 5),
    };

protected:
    Markers_t m_leftRedMarkers;
    Markers_t m_leftGreenMarkers;
    Markers_t m_leftPlaceholdersMarkers;

    Markers_t m_rightGreenMarkers;
    Markers_t m_rightRedMarkers;
    Markers_t m_rightPlaceholdersMarkers;

    wxArrayInt m_overviewPanelMarkers;

    bool m_darkTheme;

    std::vector<std::pair<int, int>> m_sequences; // start-line - end-line pairs
    int m_cur_sequence;

    size_t m_flags;
    DiffConfig m_config;
    bool m_storeFilepaths;
    clToolBar* m_toolbar;
    clPluginsFindBar* m_findBar = nullptr;

protected:
    virtual void OnBrowseLeftFile(wxCommandEvent& event);
    virtual void OnBrowseRightFile(wxCommandEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);
    virtual void OnSingleUI(wxUpdateUIEvent& event);
    virtual void OnSingleView(wxCommandEvent& event);
    virtual void OnLeftPickerUI(wxUpdateUIEvent& event);
    virtual void OnRightPickerUI(wxUpdateUIEvent& event);
    virtual void OnPaneloverviewLeftDown(wxMouseEvent& event);

    void OnMenuCopyLeft2Right(wxCommandEvent& event);
    void OnMenuCopyRight2Left(wxCommandEvent& event);
    void OnCopyAllMenu(wxCommandEvent& event);
    void OnViewMenu(wxCommandEvent& event);
    void OnPreferences(wxCommandEvent& event);

protected:
    wxString DoGetContentNoPlaceholders(wxStyledTextCtrl* stc) const;
    bool IsLeftReadOnly() const { return m_flags & kLeftReadOnly; }
    bool IsRightReadOnly() const { return m_flags & kRightReadOnly; }
    bool IsDeleteLeftOnExit() const { return m_flags & kDeleteLeftOnExit; }
    bool IsDeleteRightOnExit() const { return m_flags & kDeleteRightOnExit; }
    bool IsOriginSourceControl() const { return m_flags & kOriginSourceControl; }

public:
    virtual void OnRefreshDiffUI(wxUpdateUIEvent& event);
    virtual void OnHorizontal(wxCommandEvent& event);
    virtual void OnHorizontalUI(wxUpdateUIEvent& event);
    virtual void OnVertical(wxCommandEvent& event);
    virtual void OnVerticalUI(wxUpdateUIEvent& event);
    virtual void OnCopyFileFromRight(wxCommandEvent& event);
    virtual void OnCopyFileLeftToRight(wxCommandEvent& event);
    virtual void OnSaveChanges(wxCommandEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnSaveChangesUI(wxUpdateUIEvent& event);
    virtual void OnCopyLeftToRight(wxCommandEvent& event);
    virtual void OnCopyRightToLeft(wxCommandEvent& event);
    virtual void OnCopyLeftToRightUI(wxUpdateUIEvent& event);
    virtual void OnCopyRightToLeftUI(wxUpdateUIEvent& event);
    virtual void OnNextDiffUI(wxUpdateUIEvent& event);
    virtual void OnPrevDiffUI(wxUpdateUIEvent& event);
    virtual void OnNextDiffSequence(wxCommandEvent& event);
    virtual void OnPrevDiffSequence(wxCommandEvent& event);
    virtual void OnRefreshDiff(wxCommandEvent& event);
    virtual void OnLeftStcPainted(wxStyledTextEvent& event);
    virtual void OnRightStcPainted(wxStyledTextEvent& event);
    virtual void OnLeftStcUpdateUI(wxStyledTextEvent& event);
    virtual void OnIgnoreWhitespaceClicked(wxCommandEvent& event);
    virtual void OnIgnoreWhitespaceUI(wxUpdateUIEvent& event);
    virtual void OnShowLinenosClicked(wxCommandEvent& event);
    virtual void OnShowLinenosUI(wxUpdateUIEvent& event);
    virtual void OnShowOverviewBarClicked(wxCommandEvent& event);
    virtual void OnShowOverviewBarUI(wxUpdateUIEvent& event);
    virtual void OnPaneloverviewEraseBackground(wxEraseEvent& event);
    void OnPageClosing(wxNotifyEvent& event);

    void PrepareViews();
    void UpdateViews(const wxString& left, const wxString& right);
    void DoClean();
    void DoDrawSequenceMarkers(int firstLine, int lastLine, wxStyledTextCtrl* ctrl);
    void DoCopyCurrentSequence(wxStyledTextCtrl* from, wxStyledTextCtrl* to);
    void DoCopyFileContent(wxStyledTextCtrl* from, wxStyledTextCtrl* to);
    void DoGetPositionsToCopy(wxStyledTextCtrl* stc, int& startPos, int& endPos, int& placeHolderMarkerFirstLine,
                              int& placeHolderMarkerLastLine);
    void DoSave(wxStyledTextCtrl* stc, const wxFileName& fn);

    bool CanNextDiff();
    bool CanPrevDiff();
    void DefineMarkers(wxStyledTextCtrl* ctrl);

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
    void SetOriginSourceControl() { m_flags |= kOriginSourceControl; }

    /**
     * @brief start a new empty diff
     */
    void DiffNew();

    /**
     * @brief start a new diff for two input files
     */
    void DiffNew(const wxFileName& left, const wxFileName& right);

    /**
     * @brief set the initial files to diff
     * Once set, you should call Diff() function
     */
    void SetFilesDetails(const DiffSideBySidePanel::FileInfo& leftFile, const DiffSideBySidePanel::FileInfo& rightFile);

    /**
     * @brief set whether to store the diff's filepaths for later reload
     */
    void SetSaveFilepaths(bool save) { m_storeFilepaths = save; }

    /**
     * @brief returns whether find bar has focus
     */
    bool HasFindBarFocus() const { return m_findBar->HasFocus(); }
};
#endif // DIFFSIDEBYSIDEPANEL_H
