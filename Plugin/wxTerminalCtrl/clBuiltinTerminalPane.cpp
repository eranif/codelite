#include "clBuiltinTerminalPane.hpp"

#include "ColoursAndFontsManager.h"
#include "CompilerLocatorMSVC.h"
#include "FontUtils.hpp"
#include "Platform.hpp"
#include "bitmap_loader.h"
#include "clFileName.hpp"
#include "clWorkspaceManager.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "ssh/ssh_account_info.h"
#include "wxTerminalOutputCtrl.hpp"

#include <wx/app.h>
#include <wx/choicdlg.h>
#include <wx/sizer.h>

clBuiltinTerminalPane::clBuiltinTerminalPane(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                          kNotebook_CloseButtonOnActiveTab | kNotebook_ShowFileListButton |
                              kNotebook_MouseMiddleClickClosesTab | kNotebook_FixedWidth | kNotebook_AllowDnD);

    m_toolbar = new clToolBar(this);

    GetSizer()->Add(m_toolbar, wxSizerFlags().Expand().Proportion(0));
    GetSizer()->Add(m_book, wxSizerFlags().Expand().Proportion(1));

    auto image_list = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_NEW, _("New"), image_list->Add("file_new"), wxEmptyString, wxITEM_DROPDOWN);
    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &clBuiltinTerminalPane::OnNewDropdown, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &clBuiltinTerminalPane::OnNew, this, wxID_NEW);

    GetSizer()->Fit(this);
    UpdateTextAttributes();
    wxTheApp->Bind(wxEVT_TERMINAL_CTRL_SET_TITLE, &clBuiltinTerminalPane::OnSetTitle, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &clBuiltinTerminalPane::OnPageChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
}

clBuiltinTerminalPane::~clBuiltinTerminalPane()
{
    wxTheApp->Unbind(wxEVT_TERMINAL_CTRL_SET_TITLE, &clBuiltinTerminalPane::OnSetTitle, this);
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &clBuiltinTerminalPane::OnPageChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
}

void clBuiltinTerminalPane::UpdateTextAttributes()
{
    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        auto terminal = static_cast<wxTerminalCtrl*>(m_book->GetPage(i));
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
        terminal->GetView()->SetAttributes(bg_colour, fg_colour, text_font);
        terminal->GetView()->ReloadSettings();
    }
}

void clBuiltinTerminalPane::OnWorkspaceLoaded(clWorkspaceEvent& event) { event.Skip(); }

void clBuiltinTerminalPane::Focus()
{
    if(GetActiveTerminal()) {
        GetActiveTerminal()->GetInputCtrl()->SetFocus();
    }
}

wxTerminalCtrl* clBuiltinTerminalPane::GetActiveTerminal()
{
    // when we add tabs, return the active selected tab's terminal
    if(m_book->GetPageCount() == 0) {
        return nullptr;
    }
    return static_cast<wxTerminalCtrl*>(m_book->GetPage(m_book->GetSelection()));
}

void clBuiltinTerminalPane::OnNewDropdown(wxCommandEvent& event)
{
    // now show the menu for choosing the location for this terminal
    if(!GetActiveTerminal()) {
        // this functionality requires an active terminal running
        return;
    }

    wxMenu menu;
    wxString default_path;          // contains the wd for the terminal
    wxString workspace_ssh_account; // if remote workspace is loaded, this variable will contains its account
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if(workspace) {
        if(workspace->IsRemote()) {
            workspace_ssh_account = workspace->GetSshAccount();
        }
        default_path = wxFileName(workspace->GetFileName()).GetPath();
        default_path.Replace("\\", "/");
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
                Focus();
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
                Focus();
            },
            item->GetId());
    }
    m_toolbar->ShowMenuForButton(wxID_NEW, &menu);
}

void clBuiltinTerminalPane::OnNew(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString working_directory;
    wxString shell;
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if(workspace && !workspace->IsRemote()) {
        wxFileName fn{ workspace->GetFileName() };
        working_directory = clFileName::ToMSYS2(fn.GetPath());
    }

#ifdef __WXMSW__
    static std::map<wxString, wxString> options_map = { { "bash", "bash" }, { "CMD", "CMD" } };
    static bool once = true;
    if(once) {
        once = false;
        wxBusyCursor bc{};
        CompilerLocatorMSVC locator_msvc{};
        if(locator_msvc.Locate()) {
            const auto& compilers = locator_msvc.GetCompilers();
            for(auto compiler : compilers) {
                wxString build_tool = compiler->GetTool("MAKE");
                build_tool = build_tool.BeforeLast('>');
                build_tool.Prepend("CMD /K ");
                options_map.insert({ "CMD for " + compiler->GetName(), build_tool });
            }
        }
    }
    wxArrayString options;
    for(const auto& [name, _] : options_map) {
        options.Add(name);
    }

    wxString selected_shell = ::wxGetSingleChoice(_("Choose a shell:"), _("New Terminal"), options, 0);
    if(selected_shell.empty()) {
        return;
    }
    shell = options_map[selected_shell];

#else
    shell = "bash";
#endif

    wxTerminalCtrl* ctrl = new wxTerminalCtrl(m_book, wxID_ANY, working_directory);
    ctrl->SetShellCommand(shell);
    m_book->AddPage(ctrl, _("Terminal"), true);
    Focus();
}

void clBuiltinTerminalPane::OnSetTitle(wxTerminalEvent& event)
{
    event.Skip();
    wxWindow* win = dynamic_cast<wxWindow*>(event.GetEventObject());
    CHECK_PTR_RET(win);

    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if(win == m_book->GetPage(i)) {
            m_book->SetPageText(i, event.GetString());
            break;
        }
    }
}

void clBuiltinTerminalPane::OnPageChanged(wxBookCtrlEvent& event)
{
    event.Skip();
    CallAfter(&clBuiltinTerminalPane::Focus);
}

bool clBuiltinTerminalPane::IsFocused()
{
    if(GetActiveTerminal()) {
        return IsShown() && GetActiveTerminal()->IsFocused();
    } else {
        return IsShown();
    }
}