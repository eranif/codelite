#include "TailPanel.h"
#include <wx/filedlg.h>

TailPanel::TailPanel(wxWindow* parent)
    : TailPanelBase(parent)
{
    m_fileWatcher.reset(new clFileSystemWatcher());
    m_fileWatcher->SetOwner(this);
    Bind(wxEVT_FILE_MODIFIED, &TailPanel::OnFileModified, this);
}

TailPanel::~TailPanel() { Unbind(wxEVT_FILE_MODIFIED, &TailPanel::OnFileModified, this); }

void TailPanel::OnOpenFile(wxCommandEvent& event)
{
    wxString filepath = ::wxFileSelector();
    if(filepath.IsEmpty()) {
        return;
    }

    DoClear();

    m_file = filepath;

    // Stop the current watcher
    m_fileWatcher->SetFile(m_file);
    m_fileWatcher->Start();
}

void TailPanel::OnPause(wxCommandEvent& event) { m_fileWatcher->Stop(); }

void TailPanel::OnPauseUI(wxUpdateUIEvent& event) { event.Enable(m_fileWatcher->IsRunning()); }

void TailPanel::OnPlay(wxCommandEvent& event) { m_fileWatcher->Start(); }

void TailPanel::OnPlayUI(wxUpdateUIEvent& event) { event.Enable(!m_fileWatcher->IsRunning()); }

void TailPanel::DoClear()
{
    m_fileWatcher->Stop();
    m_fileWatcher->Clear();

    m_file.Clear();
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
}

void TailPanel::OnFileModified(clFileSystemEvent& event) {}
