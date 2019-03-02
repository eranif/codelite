#ifndef DIFFFOLDERSFRAME_H
#define DIFFFOLDERSFRAME_H

#include "DiffUI.h"
#include "codelite_exports.h"
#include <thread>

class WXDLLIMPEXP_SDK DiffFoldersFrame : public DiffFoldersBaseDlg
{
    wxString m_leftFolder;
    wxString m_rightFolder;
    bool m_showSimilarItems = false;
    std::thread* m_checksumThread = nullptr;

public:
    DiffFoldersFrame(wxWindow* parent);
    virtual ~DiffFoldersFrame();
    void OnChecksum(int callId, const wxArrayString& checksumArray);

protected:
    void BuildTrees(const wxString& left, const wxString& right);
    void DoOpenDiff(const wxDataViewItem& item);
    void StopChecksumThread();

protected:
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnNewCmparison(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnItemContextMenu(wxDataViewEvent& event);

    void OnMenuDiff(wxCommandEvent& event);
    void OnCopyToRight(wxCommandEvent& event);
    void OnCopyToLeft(wxCommandEvent& event);
    void OnShowSimilarFiles(wxCommandEvent& event);
    void OnShowSimilarFilesUI(wxUpdateUIEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnRefreshUI(wxUpdateUIEvent& event);
};
#endif // DIFFFOLDERSFRAME_H
