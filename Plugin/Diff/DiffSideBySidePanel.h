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
#include "DiffUI.hpp"
#include "clDTL.h"
#include "clPluginsFindBar.h"
#include "clToolBar.h"

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

    using Markers_t = std::vector<int>;

public:
    struct FileInfo {
        wxFileName filename;
        wxString title;
        bool readOnly = true;
        bool deleteOnExit = false;
        wxString remoteAccount;
        wxString remotePath;

        FileInfo(const wxFileName& fn, const wxString& caption, bool ro)
            : filename(fn)
            , title(caption)
            , readOnly(ro)
            , deleteOnExit(false)
        {
        }

        FileInfo(const wxFileName& fn)
            : filename(fn)
            , title(fn.GetFullPath())
        {
        }

        FileInfo()
            : readOnly(true)
            , deleteOnExit(false)
        {
        }

        bool is_remote() const { return !remoteAccount.empty() && !remotePath.empty(); }
        void clear() { *this = {}; }
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

    size_t m_flags = 0;
    DiffConfig m_config;
    bool m_storeFilepaths;
    clToolBar* m_toolbar;
    clPluginsFindBar* m_findBar = nullptr;
    FileInfo m_left;
    FileInfo m_right;

protected:
    void OnBrowseLeftFile(wxCommandEvent& event) override;
    void OnBrowseRightFile(wxCommandEvent& event) override;
    void OnMouseWheel(wxMouseEvent& event) override;
    void OnLeftPickerUI(wxUpdateUIEvent& event) override;
    void OnRightPickerUI(wxUpdateUIEvent& event) override;
    void OnPanelOverviewEraseBackground(wxEraseEvent& event) override;
    void OnPanelOverviewLeftDown(wxMouseEvent& event) override;
    void OnLeftStcPainted(wxStyledTextEvent& event) override;
    void OnRightStcPainted(wxStyledTextEvent& event) override;

    void OnSingleUI(wxUpdateUIEvent& event);
    void OnSingleView(wxCommandEvent& event);
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
    void OnRefreshDiffUI(wxUpdateUIEvent& event);
    void OnHorizontal(wxCommandEvent& event);
    void OnHorizontalUI(wxUpdateUIEvent& event);
    void OnVertical(wxCommandEvent& event);
    void OnVerticalUI(wxUpdateUIEvent& event);
    void OnCopyFileFromRight(wxCommandEvent& event);
    void OnCopyFileLeftToRight(wxCommandEvent& event);
    void OnSaveChanges(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    void OnSaveChangesUI(wxUpdateUIEvent& event);
    void OnCopyLeftToRight(wxCommandEvent& event);
    void OnCopyRightToLeft(wxCommandEvent& event);
    void OnCopyLeftToRightUI(wxUpdateUIEvent& event);
    void OnCopyRightToLeftUI(wxUpdateUIEvent& event);
    void OnNextDiffUI(wxUpdateUIEvent& event);
    void OnPrevDiffUI(wxUpdateUIEvent& event);
    void OnNextDiffSequence(wxCommandEvent& event);
    void OnPrevDiffSequence(wxCommandEvent& event);
    void OnRefreshDiff(wxCommandEvent& event);
    void OnLeftStcUpdateUI(wxStyledTextEvent& event);
    void OnIgnoreWhitespaceClicked(wxCommandEvent& event);
    void OnIgnoreWhitespaceUI(wxUpdateUIEvent& event);
    void OnShowLinenosClicked(wxCommandEvent& event);
    void OnShowLinenosUI(wxUpdateUIEvent& event);
    void OnShowOverviewBarClicked(wxCommandEvent& event);
    void OnShowOverviewBarUI(wxUpdateUIEvent& event);
    void OnPageClosing(wxNotifyEvent& event);

    void PrepareViews();
    void UpdateViews(const wxString& left, const wxString& right);
    void DoClean();
    void DoDrawSequenceMarkers(int firstLine, int lastLine, wxStyledTextCtrl* ctrl);
    void DoCopyCurrentSequence(wxStyledTextCtrl* from, wxStyledTextCtrl* to);
    void DoCopyFileContent(wxStyledTextCtrl* from, wxStyledTextCtrl* to);
    void DoGetPositionsToCopy(wxStyledTextCtrl* stc,
                              int& startPos,
                              int& endPos,
                              int& placeHolderMarkerFirstLine,
                              int& placeHolderMarkerLastLine);
    void DoSave(wxStyledTextCtrl* stc, const wxFileName& fn);

    bool CanNextDiff();
    bool CanPrevDiff();
    void DefineMarkers(wxStyledTextCtrl* ctrl);

public:
    explicit DiffSideBySidePanel(wxWindow* parent);
    ~DiffSideBySidePanel() override;

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
