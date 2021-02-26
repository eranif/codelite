//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SvnCommitDialog.cpp
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

#include "SvnCommitDialog.h"
#include "asyncprocess.h"
#include "bitmap_loader.h"
#include "clSingleChoiceDialog.h"
#include "editor_config.h"
#include "globals.h"
#include "imanager.h"
#include "lexer_configuration.h"
#include "processreaderthread.h"
#include "subversion2.h"
#include "svn_local_properties.h"
#include "svnsettingsdata.h"
#include "windowattrmanager.h"
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

class CommitMessageStringData : public wxClientData
{
    wxString m_data;

public:
    CommitMessageStringData(const wxString& data)
        : m_data(data.c_str())
    {
    }
    virtual ~CommitMessageStringData() {}

    const wxString& GetData() const { return m_data; }
};

SvnCommitDialog::SvnCommitDialog(wxWindow* parent, Subversion2* plugin)
    : SvnCommitDialogBaseClass(parent)
    , m_plugin(plugin)
    , m_process(NULL)
{
    // Hide the bug tracker ID
    m_textCtrlBugID->Clear();
    m_textCtrlBugID->Hide();
    m_staticTextBugID->Hide();

    m_textCtrlFrID->Clear();
    m_staticText32->Hide();
    m_textCtrlFrID->Hide();

    m_checkListFiles->Disable();
    m_panel1->Disable();
    DoCommonInit();
}

SvnCommitDialog::SvnCommitDialog(wxWindow* parent, const wxArrayString& paths, const wxString& url, Subversion2* plugin,
                                 const wxString& repoPath)
    : SvnCommitDialogBaseClass(parent)
    , m_plugin(plugin)
    , m_url(url)
    , m_repoPath(repoPath)
    , m_process(NULL)
{
    wxString title = GetTitle();
    if(!url.empty()) {
        title << wxT(" - ") << url;
    }
    SetTitle(title);
    DoCommonInit();

    if(!paths.empty()) {
        for(size_t i = 0; i < paths.GetCount(); ++i) {
            int index = m_checkListFiles->Append(paths.Item(i));
            m_checkListFiles->Check((unsigned int)index);
        }

        m_checkListFiles->Select(0);
        DoShowDiff(0);
    }
}

void SvnCommitDialog::DoCommonInit()
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &SvnCommitDialog::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &SvnCommitDialog::OnProcessTerminatd, this);
    m_stcDiff->SetReadOnly(true);
    m_checkListFiles->Clear();

    // These two classes will allow copy / paste etc using the keyboard on the STC classes
    m_stcMessageHelper.Reset(new clEditEventsHandler(m_stcMessage));
    m_stcDiffHelper.Reset(new clEditEventsHandler(m_stcDiff));
    DoCreateToolbar();
    int sashPos = m_plugin->GetSettings().GetCommitDlgSashPos();
    if(sashPos != wxNOT_FOUND) {
        m_splitterH->SetSashPosition(sashPos);
    }

    int sashHPos = m_plugin->GetSettings().GetCommitDlgHSashPos();
    if(sashHPos != wxNOT_FOUND) {
        m_splitterV->SetSashPosition(sashHPos);
    }

    LexerConf::Ptr_t diffLexer = EditorConfigST::Get()->GetLexer("Diff");
    if(diffLexer) {
        m_stcDiff->SetLexer(wxSTC_LEX_DIFF);
        diffLexer->Apply(m_stcDiff);
    }

    LexerConf::Ptr_t textLexer = EditorConfigST::Get()->GetLexer("text");
    if(textLexer) {
        textLexer->Apply(m_stcMessage);
    }
    ::clSetTLWindowBestSizeAndPosition(this);
}

SvnCommitDialog::~SvnCommitDialog()
{
    wxDELETE(m_process);

    wxString message = m_stcMessage->GetText();
    m_plugin->GetCommitMessagesCache().AddMessage(message);

    int sashPos = m_splitterH->GetSashPosition();
    int sashPosH = m_splitterV->GetSashPosition();
    SvnSettingsData ssd = m_plugin->GetSettings();
    ssd.SetCommitDlgSashPos(sashPos);
    ssd.SetCommitDlgHSashPos(sashPosH);
    m_plugin->SetSettings(ssd);
}

wxString SvnCommitDialog::GetMesasge()
{
    SubversionLocalProperties props(m_url);
    wxString msg = NormalizeMessage(m_stcMessage->GetText());
    msg << wxT("\n");

    // Append any bug URLs to the commit message
    if(m_textCtrlBugID->IsShown()) {
        wxString bugTrackerMsg = props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_MESSAGE);
        wxString bugTrackerUrl = props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_URL);
        wxString bugId = m_textCtrlBugID->GetValue();

        bugId.Trim().Trim(false);
        if(bugId.IsEmpty() == false) {
            // Loop over the bug IDs and append message for each bug
            wxArrayString bugs = wxStringTokenize(bugId, wxT(","), wxTOKEN_STRTOK);
            for(size_t i = 0; i < bugs.size(); i++) {

                bugs[i].Trim().Trim(false);
                if(bugs[i].IsEmpty())
                    continue;

                wxString tmpMsg = bugTrackerMsg;
                wxString tmpUrl = bugTrackerUrl;

                tmpUrl.Replace(wxT("$(BUGID)"), bugs[i]);
                tmpMsg.Replace(wxT("$(BUG_URL)"), tmpUrl);
                tmpMsg.Replace(wxT("$(BUGID)"), bugs[i]);
                msg << tmpMsg << wxT("\n");
            }
        }
    }

    // Append any FR URLs to the commit message
    if(m_textCtrlFrID->IsShown()) {
        wxString frTrackerMsg = props.ReadProperty(SubversionLocalProperties::FR_TRACKER_MESSAGE);
        wxString frTrackerUrl = props.ReadProperty(SubversionLocalProperties::FR_TRACKER_URL);
        wxString frId = m_textCtrlFrID->GetValue();

        frId.Trim().Trim(false);
        if(frId.IsEmpty() == false) {
            // Loop over the bug IDs and append message for each bug
            wxArrayString frs = wxStringTokenize(frId, wxT(","), wxTOKEN_STRTOK);
            for(size_t i = 0; i < frs.size(); i++) {

                frs[i].Trim().Trim(false);
                if(frs[i].IsEmpty())
                    continue;

                wxString tmpMsg = frTrackerMsg;
                wxString tmpUrl = frTrackerUrl;

                tmpUrl.Replace(wxT("$(FRID)"), frs[i]);
                tmpMsg.Replace(wxT("$(FR_URL)"), tmpUrl);
                tmpMsg.Replace(wxT("$(FRID)"), frs[i]);
                msg << tmpMsg << wxT("\n");
            }
        }
    }

    msg.Trim().Trim(false);
    return msg;
}

wxString SvnCommitDialog::NormalizeMessage(const wxString& message)
{
    wxString normalizedStr = message;
    normalizedStr.Trim().Trim(false);

    // SVN does not like any quotation marks in the comment -> escape them
    normalizedStr.Replace(wxT("\""), wxT("\\\""));
    return normalizedStr;
}

wxArrayString SvnCommitDialog::GetPaths()
{
    wxArrayString paths;
    for(size_t i = 0; i < m_checkListFiles->GetCount(); i++) {
        if(m_checkListFiles->IsChecked(i)) {
            paths.Add(m_checkListFiles->GetString(i));
        }
    }
    return paths;
}

void SvnCommitDialog::OnFileSelected(wxCommandEvent& event) { DoShowDiff(event.GetSelection()); }

void SvnCommitDialog::OnProcessOutput(clProcessEvent& e) { m_output << e.GetOutput(); }

void SvnCommitDialog::OnProcessTerminatd(clProcessEvent& e)
{
    m_cache.insert(std::make_pair(m_currentFile, m_output));
    m_stcDiff->SetReadOnly(false);
    m_stcDiff->SetText(m_output);
    m_stcDiff->SetReadOnly(true);

    // m_checkListFiles->Enable(true);
    m_currentFile.Clear();
    wxDELETE(m_process);
}

void SvnCommitDialog::DoShowDiff(int selection)
{
    if(m_repoPath.IsEmpty())
        return;

    wxString filename = m_checkListFiles->GetString(selection);

    if(filename.Contains(" ")) {
        filename.Prepend("\"").Append("\"");
    }

    if(m_cache.count(filename)) {
        m_stcDiff->SetReadOnly(false);
        m_stcDiff->SetText(m_cache[filename]);
        m_stcDiff->SetReadOnly(true);
        return;
    }

    // m_checkListFiles->Enable(false); // disable user interaction with this control until the diff process will
    // terminate
    wxString cmd;
    cmd << m_plugin->GetSvnExeNameNoConfigDir() << " diff " << filename;
    clDEBUG() << "Creating diff:" << cmd << clEndl;
    m_currentFile = filename;
    m_output.Clear();
    m_process = ::CreateAsyncProcess(this, cmd, IProcessCreateDefault, m_repoPath);
}

void SvnCommitDialog::OnShowCommitHistory(wxCommandEvent& event)
{
    wxArrayString lastMessages, previews;
    m_plugin->GetCommitMessagesCache().GetMessages(lastMessages, previews);
    clSingleChoiceDialog dlg(this, lastMessages);
    dlg.SetLabel(_("Choose a commit"));
    if(dlg.ShowModal() != wxID_OK)
        return;
    m_stcMessage->SetText(dlg.GetSelection());
}

void SvnCommitDialog::OnShowCommitHistoryUI(wxUpdateUIEvent& event)
{
    wxArrayString lastMessages, previews;
    m_plugin->GetCommitMessagesCache().GetMessages(lastMessages, previews);
    event.Enable(!lastMessages.IsEmpty());
}
void SvnCommitDialog::OnClearHistory(wxCommandEvent& event)
{
    if(::wxMessageBox(_("This will clear the message history\nContinue?"), "CodeLite",
                      wxICON_WARNING | wxCENTER | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT, this) == wxYES) {
        m_plugin->GetCommitMessagesCache().Clear();
    }
}

void SvnCommitDialog::OnClearHistoryUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_plugin->GetCommitMessagesCache().IsEmpty());
}

void SvnCommitDialog::DoCreateToolbar()
{
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(XRCID("commit-history"), _("Commit History"), images->Add("history"), "", wxITEM_DROPDOWN);
    m_toolbar->AddTool(wxID_CLEAR, _("Clear History"), images->Add("clear"));
    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL, &SvnCommitDialog::OnShowCommitHistory, this, XRCID("commit-history"));
    m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &SvnCommitDialog::OnShowCommitHistoryDropDown, this, XRCID("commit-history"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &SvnCommitDialog::OnShowCommitHistoryUI, this, XRCID("commit-history"));
    m_toolbar->Bind(wxEVT_TOOL, &SvnCommitDialog::OnClearHistory, this, wxID_CLEAR);
    m_toolbar->Bind(wxEVT_UPDATE_UI, &SvnCommitDialog::OnClearHistoryUI, this, wxID_CLEAR);
}

void SvnCommitDialog::OnShowCommitHistoryDropDown(wxCommandEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("commit-history-last-message"), _("Insert Last Message"));
    menu.Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& event) {
            wxArrayString lastMessages, previews;
            m_plugin->GetCommitMessagesCache().GetMessages(lastMessages, previews);
            if(!lastMessages.empty()) {
                m_stcMessage->SetText(lastMessages.Item(0));
            }
        },
        XRCID("commit-history-last-message"));
    m_toolbar->ShowMenuForButton(event.GetId(), &menu);
}
