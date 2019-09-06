#include "EnvironmentVariablesDlg.h"
#include <wx/arrstr.h>
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include <wx/regex.h>
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"
#include "window_locker.h"
#include "evnvarlist.h"
#include "environmentconfig.h"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include <wx/textdlg.h>
#include "globals.h"

// EnvironmentVariablesDlg* EnvironmentVariablesDlg::m_dlg = nullptr;

EnvironmentVariablesDlg::EnvironmentVariablesDlg(wxWindow* parent)
    : EnvVarsTableDlgBase(parent)
    , m_editEventsHander(m_textCtrlDefault)
{
    EvnVarList vars;
    EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
    wxStringMap_t envSets = vars.GetEnvVarSets();
    wxString activePage = vars.GetActiveSet();

    wxStyledTextCtrl* sci = m_textCtrlDefault;
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(sci);
    }

    SetName("EnvVarsTableDlg");
    wxStringMap_t::iterator iter = envSets.begin();
    for(; iter != envSets.end(); iter++) {
        wxString name = iter->first;
        wxString content = iter->second;

        if(name == wxT("Default")) {
            m_textCtrlDefault->SetText(content);
            m_textCtrlDefault->SetSavePoint();
            m_textCtrlDefault->EmptyUndoBuffer();
        } else {
            DoAddPage(name, content, false);
        }
    }

    m_book->SetSelection(0);
    for(size_t i = 0; i < m_book->GetPageCount(); i++) {
        if(m_book->GetPageText(i) == activePage) {
            m_book->GetPage(i)->SetFocus();
            m_book->SetSelection(i);
        }
        wxWindow* page = m_book->GetPage(i);
        wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(page);
        if(ctrl) {
            ctrl->SetSavePoint();
            ctrl->EmptyUndoBuffer();
        }
    }
    ::clSetSmallDialogBestSizeAndPosition(this);
}

EnvironmentVariablesDlg::~EnvironmentVariablesDlg() {}

void EnvironmentVariablesDlg::DoAddPage(const wxString& name, const wxString& content, bool select)
{
    wxStyledTextCtrl* page = new wxStyledTextCtrl(m_book, wxID_ANY, wxDefaultPosition, wxSize(0, 0));
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(page);
    }

    page->SetText(content);
    m_book->AddPage(page, name, select);
}

void EnvironmentVariablesDlg::OnButtonOk(wxCommandEvent& event)
{
    EvnVarList vars;

    wxStringMap_t envSets;

    wxString content = m_textCtrlDefault->GetText();
    wxString name = wxT("Default");
    content.Trim().Trim(false);
    envSets[name] = content;

    for(size_t i = 1; i < m_book->GetPageCount(); i++) {
        if(i == (size_t)m_book->GetSelection()) {
            vars.SetActiveSet(m_book->GetPageText(i));
        }

        wxStyledTextCtrl* page = (wxStyledTextCtrl*)m_book->GetPage(i);
        wxString content = page->GetText();
        wxString name = m_book->GetPageText(i);
        content.Trim().Trim(false);
        envSets[name] = content;
    }
    vars.SetEnvVarSets(envSets);
    EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);

    // Notify that the environment variables were modified
    clCommandEvent eventSave(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED);
    EventNotifier::Get()->AddPendingEvent(eventSave);
    event.Skip();
}

void EnvironmentVariablesDlg::OnDeleteSet(wxCommandEvent& event)
{
    wxUnusedVar(event);

    int selection = m_book->GetSelection();
    if(selection == wxNOT_FOUND) return;

    wxString name = m_book->GetPageText((size_t)selection);
    if(wxMessageBox(wxString::Format(wxT("Delete environment variables set\n'%s' ?"), name.c_str()), wxT("Confirm"),
                    wxYES_NO | wxICON_QUESTION, this) != wxYES)
        return;
    m_book->DeletePage((size_t)selection);
}

void EnvironmentVariablesDlg::OnDeleteSetUI(wxUpdateUIEvent& event)
{
    int i = m_book->GetSelection();
    event.Enable(i != wxNOT_FOUND && m_book->GetPageText(i) != wxT("Default"));
}

void EnvironmentVariablesDlg::OnExport(wxCommandEvent& event)
{
    int selection = m_book->GetSelection();
    if(selection == wxNOT_FOUND) return;

#ifdef __WXMSW__
    bool isWindows = true;
#else
    bool isWindows = false;
#endif

    wxString text;
    if(selection == 0) {
        text = m_textCtrlDefault->GetText();
    } else {
        wxStyledTextCtrl* page = dynamic_cast<wxStyledTextCtrl*>(m_book->GetPage((size_t)selection));
        if(page) {
            text = page->GetText();
        }
    }

    if(text.IsEmpty()) return;

    wxArrayString lines = wxStringTokenize(text, wxT("\r\n"), wxTOKEN_STRTOK);
    wxString envfile;
    if(isWindows) {
        envfile << wxT("environment.bat");
    } else {
        envfile << wxT("environment");
    }

    wxFileName fn(wxGetCwd(), envfile);
    wxFFile fp(fn.GetFullPath(), wxT("w+b"));
    if(fp.IsOpened() == false) {
        wxMessageBox(wxString::Format(_("Failed to open file: '%s' for write"), fn.GetFullPath().c_str()),
                     wxT("CodeLite"), wxOK | wxCENTER | wxICON_WARNING, this);
        return;
    }

    for(size_t i = 0; i < lines.GetCount(); i++) {

        wxString sLine = lines.Item(i).Trim().Trim(false);
        if(sLine.IsEmpty()) continue;

        static wxRegEx reVarPattern(wxT("\\$\\(( *)([a-zA-Z0-9_]+)( *)\\)"));
        if(isWindows) {
            while(reVarPattern.Matches(sLine)) {
                wxString varName = reVarPattern.GetMatch(sLine, 2);
                wxString text = reVarPattern.GetMatch(sLine);
                sLine.Replace(text, wxString::Format(wxT("%%%s%%"), varName.c_str()));
            }
            sLine.Prepend(wxT("set "));
            sLine.Append(wxT("\r\n"));

        } else {
            while(reVarPattern.Matches(sLine)) {
                wxString varName = reVarPattern.GetMatch(sLine, 2);
                wxString text = reVarPattern.GetMatch(sLine);
                sLine.Replace(text, wxString::Format(wxT("$%s"), varName.c_str()));
            }
            sLine.Prepend(wxT("export "));
            sLine.Append(wxT("\n"));
        }
        fp.Write(sLine);
    }

    wxMessageBox(wxString::Format(_("Environment exported to: '%s' successfully"), fn.GetFullPath()), wxT("CodeLite"),
                 wxOK | wxCENTRE, this);
}

void EnvironmentVariablesDlg::OnNewSet(wxCommandEvent& event) { CallAfter(&EnvironmentVariablesDlg::DoAddNewSet); }

void EnvironmentVariablesDlg::OnClose(wxCloseEvent& event) { event.Skip(); }

void EnvironmentVariablesDlg::OnCancel(wxCommandEvent& event)
{
    event.Skip();
    EndModal(wxID_CANCEL);
}

void EnvironmentVariablesDlg::DoAddNewSet()
{
    wxTextEntryDialog dlg(this, _("Name:"), wxT("Create a new set"), "My New Set");
    if(dlg.ShowModal() == wxID_OK) {
        wxString name = dlg.GetValue();
        if(name.IsEmpty()) return;
        DoAddPage(name, wxT(""), false);
    }
}
