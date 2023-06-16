#include "clBuiltinTerminalPane.hpp"

#include "ColoursAndFontsManager.h"
#include "FontUtils.hpp"
#include "Platform.hpp"
#include "TextView.h"
#include "clWorkspaceManager.h"
#include "event_notifier.h"
#include "ssh/ssh_account_info.h"

#include <wx/sizer.h>

clBuiltinTerminalPane::clBuiltinTerminalPane(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_toolbar = new clToolBar(this);
    m_terminal = new wxTerminalCtrl(this);
    GetSizer()->Add(m_toolbar, wxSizerFlags().Expand().Proportion(0));
    GetSizer()->Add(m_terminal, wxSizerFlags().Expand().Proportion(1));

    auto image_list = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_NEW, _("New"), image_list->Add("file_new"), wxEmptyString, wxITEM_DROPDOWN);
    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &clBuiltinTerminalPane::OnNewDropdown, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &clBuiltinTerminalPane::OnNew, this, wxID_NEW);

    GetSizer()->Fit(this);
    UpdateTextAttributes();
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
}

clBuiltinTerminalPane::~clBuiltinTerminalPane()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
}

void clBuiltinTerminalPane::UpdateTextAttributes()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    auto default_style = lexer->GetProperty(0);
    wxColour bg_colour = default_style.GetBgColour();
    wxColour fg_colour = default_style.GetFgColour();

    wxFont text_font;
    if(default_style.GetFontInfoDesc().empty()) {
        text_font = FontUtils::GetDefaultMonospacedFont();
    } else {
        text_font.SetNativeFontInfo(default_style.GetFontInfoDesc());
    }
    m_terminal->GetView()->SetAttributes(bg_colour, fg_colour, text_font);
    m_terminal->GetView()->ReloadSettings();
}

void clBuiltinTerminalPane::OnWorkspaceLoaded(clWorkspaceEvent& event) { event.Skip(); }

void clBuiltinTerminalPane::Focus() { m_terminal->GetView()->GetCtrl()->SetFocus(); }
wxTerminalCtrl* clBuiltinTerminalPane::GetActiveTerminal()
{
    // when we add tabs, return the active selected tab's terminal
    return m_terminal;
}

void clBuiltinTerminalPane::OnNewDropdown(wxCommandEvent& event)
{
    wxMenu menu;
    wxString default_path;          // contains the wd for the terminal
    wxString workspace_ssh_account; // if remote workspace is loaded, this variable will contains its account
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if(workspace) {
        if(workspace->IsRemote()) {
            workspace_ssh_account = workspace->GetSshAccount();
            default_path = wxFileName(workspace->GetFileName()).GetPath(false, wxPATH_UNIX);
        } else {
            default_path = wxFileName(workspace->GetFileName()).GetPath();
        }
    }

    if(!default_path.empty()) {
        wxString label;
        label << "Workspace: " << default_path;
        if(!workspace_ssh_account.empty()) {
            label << "@" << workspace_ssh_account;
        }

        auto item = menu.Append(wxID_ANY, label);
        menu.Bind(
            wxEVT_MENU,
            [this, default_path, workspace_ssh_account](wxCommandEvent& event) {
                if(workspace_ssh_account.empty()) {
                    GetActiveTerminal()->SetTerminalWorkingDirectory(default_path);
                } else {
                    GetActiveTerminal()->SSHAndSetWorkingDirectory(workspace_ssh_account, default_path);
                }
            },
            item->GetId());
        menu.AppendSeparator();
    }

    auto all_accounts = SSHAccountInfo::Load();
    for(const auto& account : all_accounts) {
        auto item = menu.Append(wxID_ANY, "SSH: " + account.GetAccountName());
        menu.Bind(
            wxEVT_MENU,
            [this, account](wxCommandEvent& event) {
                GetActiveTerminal()->SSHAndSetWorkingDirectory(account.GetAccountName(), wxEmptyString);
            },
            item->GetId());
    }
    m_toolbar->ShowMenuForButton(wxID_NEW, &menu);
}

void clBuiltinTerminalPane::OnNew(wxCommandEvent& event) { OnNewDropdown(event); }