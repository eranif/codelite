#include "TailPanel.h"
#include <wx/filedlg.h>
#include "fileutils.h"
#include <wx/ffile.h>
#include "event_notifier.h"
#include "codelite_events.h"
#include "lexer_configuration.h"
#include "ColoursAndFontsManager.h"

TailPanel::TailPanel(wxWindow* parent)
    : TailPanelBase(parent)
    , m_lastPos(0)
{
    m_fileWatcher.reset(new clFileSystemWatcher());
    m_fileWatcher->SetOwner(this);
    Bind(wxEVT_FILE_MODIFIED, &TailPanel::OnFileModified, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &TailPanel::OnThemeChanged, this);
    m_editEvents.Reset(new clEditEventsHandler(m_stc));
}

TailPanel::~TailPanel()
{
    Unbind(wxEVT_FILE_MODIFIED, &TailPanel::OnFileModified, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &TailPanel::OnThemeChanged, this);
}

void TailPanel::OnOpenFile(wxCommandEvent& event)
{
    wxString filepath = ::wxFileSelector();
    if(filepath.IsEmpty()) {
        return;
    }

    DoClear();

    m_file = filepath;
    m_lastPos = FileUtils::GetFileSize(m_file);

    // Stop the current watcher
    m_fileWatcher->SetFile(m_file);
    m_fileWatcher->Start();
}

void TailPanel::OnPause(wxCommandEvent& event) { m_fileWatcher->Stop(); }

void TailPanel::OnPauseUI(wxUpdateUIEvent& event) { event.Enable(m_file.IsOk() && m_fileWatcher->IsRunning()); }

void TailPanel::OnPlay(wxCommandEvent& event) { m_fileWatcher->Start(); }

void TailPanel::OnPlayUI(wxUpdateUIEvent& event) { event.Enable(m_file.IsOk() && !m_fileWatcher->IsRunning()); }

void TailPanel::DoClear()
{
    m_fileWatcher->Stop();
    m_fileWatcher->Clear();

    m_file.Clear();
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
    m_lastPos = 0;
}

void TailPanel::OnFileModified(clFileSystemEvent& event)
{
    wxFileName fn(event.GetPath());
    // Get the current file size
    size_t cursize = FileUtils::GetFileSize(m_file);
    wxFFile fp(m_file.GetFullPath(), "rb");
    if(fp.IsOpened() && fp.Seek(m_lastPos)) {
        wxString content;
        if(fp.ReadAll(&content)) {
            DoAppendText(content);
        }
        m_lastPos = cursize;
    }
}

void TailPanel::DoAppendText(const wxString& text)
{
    m_stc->SetReadOnly(false);
    m_stc->AppendText(text);
    m_stc->SetReadOnly(true);
    m_stc->SetSelectionEnd(m_stc->GetLength());
    m_stc->SetSelectionStart(m_stc->GetLength());
    m_stc->SetCurrentPos(m_stc->GetLength());
    m_stc->EnsureCaretVisible();
}

void TailPanel::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip(); // must call this to allow other handlers to work
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_stc);
    }
}
void TailPanel::OnClear(wxCommandEvent& event)
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
}

void TailPanel::OnClearUI(wxUpdateUIEvent& event) { event.Enable(!m_stc->IsEmpty()); }

void TailPanel::OnClose(wxCommandEvent& event) { DoClear(); }

void TailPanel::OnCloseUI(wxUpdateUIEvent& event) { event.Enable(m_file.IsOk()); }
