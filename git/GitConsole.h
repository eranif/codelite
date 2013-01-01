#ifndef GITCONSOLE_H
#define GITCONSOLE_H
#include "gitui.h"
#include "bitmap_loader.h"
#include <wx/dataview.h>

class GitPlugin;
class GitConsole : public GitConsoleBase
{
    GitPlugin* m_git;
    bool       m_isVerbose;
    wxDataViewItem m_itemModified;
    wxDataViewItem m_itemUntracked;
    wxDataViewItem m_itemNew;
    BitmapLoader   *m_bitmapLoader;
    BitmapLoader::BitmapMap_t m_bitmaps;
    wxBitmap m_modifiedBmp;
    wxBitmap m_untrackedBmp;
    wxBitmap m_folderBmp;
    wxBitmap m_newBmp;

public:
    GitConsole(wxWindow* parent, GitPlugin* git);
    virtual ~GitConsole();
    void AddText(const wxString &text);
    void AddRawText(const wxString &text);
    bool IsVerbose() const;
    void UpdateTreeView(const wxString &output);
protected:
    virtual void OnAddFile(wxCommandEvent& event);
    virtual void OnResetFile(wxCommandEvent& event);
    virtual void OnContextMenu(wxDataViewEvent& event);
    virtual void OnStopGitProcessUI(wxUpdateUIEvent& event);
    virtual void OnClearGitLogUI(wxUpdateUIEvent& event);
    virtual void OnClearGitLog(wxCommandEvent& event);
    virtual void OnStopGitProcess(wxCommandEvent& event);
    void OnWorkspaceClosed(wxCommandEvent &e);
    void OnConfigurationChanged(wxCommandEvent &e);

};
#endif // GITCONSOLE_H
