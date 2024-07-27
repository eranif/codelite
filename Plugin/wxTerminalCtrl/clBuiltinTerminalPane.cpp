#include "clBuiltinTerminalPane.hpp"

#include "ColoursAndFontsManager.h"
#include "CompilerLocator/CompilerLocatorMSVC.h"
#include "FontUtils.hpp"
#include "bitmap_loader.h"
#include "clFileName.hpp"
#include "clWorkspaceManager.h"
#include "event_notifier.h"
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

    // Get list of terminals
    m_terminal_types = new wxChoice(m_toolbar, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(150), wxNOT_FOUND));
    UpdateTerminalsChoice(false);
    m_toolbar->AddControl(m_terminal_types);

#ifdef __WXMSW__
    m_toolbar->AddTool(wxID_REFRESH, _("Scan"), image_list->Add("debugger_restart"), wxEmptyString, wxITEM_NORMAL);
#endif

    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &clBuiltinTerminalPane::OnNewDropdown, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &clBuiltinTerminalPane::OnNew, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &clBuiltinTerminalPane::OnScanForTerminals, this, wxID_REFRESH);

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
    clConfig::Get().Write("terminal/last_used_terminal", m_terminal_types->GetStringSelection());
}

void clBuiltinTerminalPane::UpdateTextAttributes()
{
    for (size_t i = 0; i < m_book->GetPageCount(); ++i) {
        auto terminal = static_cast<wxTerminalCtrl*>(m_book->GetPage(i));
        auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
        auto default_style = lexer->GetProperty(0);
        wxColour bg_colour = default_style.GetBgColour();
        wxColour fg_colour = default_style.GetFgColour();

        wxFont text_font;
        if (default_style.GetFontInfoDesc().empty()) {
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
    if (GetActiveTerminal()) {
        GetActiveTerminal()->GetInputCtrl()->SetFocus();
    }
}

wxTerminalCtrl* clBuiltinTerminalPane::GetActiveTerminal()
{
    // when we add tabs, return the active selected tab's terminal
    if (m_book->GetPageCount() == 0) {
        return nullptr;
    }
    return static_cast<wxTerminalCtrl*>(m_book->GetPage(m_book->GetSelection()));
}

void clBuiltinTerminalPane::OnNewDropdown(wxCommandEvent& event)
{
    // now show the menu for choosing the location for this terminal
    if (!GetActiveTerminal()) {
        // this functionality requires an active terminal running
        return;
    }

    wxMenu menu;
    wxString default_path;          // contains the wd for the terminal
    wxString workspace_ssh_account; // if remote workspace is loaded, this variable will contains its account
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace) {
        if (workspace->IsRemote()) {
            workspace_ssh_account = workspace->GetSshAccount();
        }
        default_path = wxFileName(workspace->GetFileName()).GetPath();
        default_path.Replace("\\", "/");
    }

    if (!default_path.empty()) {
        wxString label;
        label << "Workspace: " << default_path;
        if (!workspace_ssh_account.empty()) {
            label << "@" << workspace_ssh_account;
        }

        auto item = menu.Append(wxID_ANY, label);
        menu.Bind(
            wxEVT_MENU,
            [this, default_path, workspace_ssh_account](wxCommandEvent& event) {
                if (workspace_ssh_account.empty()) {
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
    for (const auto& account : all_accounts) {
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
    if (workspace && !workspace->IsRemote()) {
        wxFileName fn{ workspace->GetFileName() };
        working_directory = clFileName::ToMSYS2(fn.GetPath());
    }

    int selection = m_terminal_types->GetSelection();
    if (selection == wxNOT_FOUND) {
        return;
    }

    wxStringClientData* cd = dynamic_cast<wxStringClientData*>(m_terminal_types->GetClientObject(selection));
    const wxString& cmd = cd->GetData();

    wxTerminalCtrl* ctrl = new wxTerminalCtrl(m_book, wxID_ANY, working_directory);
    static size_t terminal_id = 0;
    ctrl->SetShellCommand(cmd);

    m_book->AddPage(ctrl, wxString::Format("Terminal %d", ++terminal_id), true);
    m_book->SetPageToolTip(m_book->GetPageCount() - 1, cmd);

    Focus();
}

void clBuiltinTerminalPane::OnSetTitle(wxTerminalEvent& event)
{
    event.Skip();
    wxWindow* win = dynamic_cast<wxWindow*>(event.GetEventObject());
    CHECK_PTR_RET(win);

    for (size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if (win == m_book->GetPage(i)) {
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
    if (GetActiveTerminal()) {
        return IsShown() && GetActiveTerminal()->IsFocused();
    } else {
        return IsShown();
    }
}

void clBuiltinTerminalPane::DetectTerminals(std::map<wxString, wxString>& terminals)
{
#ifdef __WXMSW__
    terminals.clear();
    terminals = { { "bash", "bash" }, { "CMD", "CMD" } };
    CompilerLocatorMSVC locator_msvc{};
    if (locator_msvc.Locate()) {
        // attempt to locate MSVC compilers
        const auto& compilers = locator_msvc.GetCompilers();
        for (auto compiler : compilers) {
            wxString build_tool = compiler->GetTool("MAKE");
            build_tool = build_tool.BeforeLast('>');
            build_tool.Prepend("CMD /K ");
            terminals.insert({ "CMD for " + compiler->GetName(), build_tool });
        }
    }
    WriteTerminalOptionsToDisk(terminals);
#endif
}

bool clBuiltinTerminalPane::ReadTerminalOptionsFromDisk(std::map<wxString, wxString>& terminals)
{
    wxArrayString results = clConfig::Get().Read("terminal/options", wxArrayString{});
    if (results.empty() || results.size() % 2 != 0) {
        return false;
    }

    terminals.clear();
    // we serialise the map into array as pairs of: [key,value,key2,value...]
    for (size_t i = 0; i < results.size() / 2; ++i) {
        wxString name = results[i * 2];
        wxString command = results[i * 2 + 1];
        terminals.insert({ name, command });
    }
    return true;
}

void clBuiltinTerminalPane::WriteTerminalOptionsToDisk(const std::map<wxString, wxString>& terminals)
{
    wxArrayString result;
    result.reserve(terminals.size() * 2);

    for (const auto& [name, command] : terminals) {
        result.Add(name);
        result.Add(command);
    }

    // persist the results
    clConfig::Get().Write("terminal/options", result);
}

std::map<wxString, wxString> clBuiltinTerminalPane::GetTerminalsOptions(bool scan)
{
    std::map<wxString, wxString> terminals;
#ifdef __WXMSW__
    terminals.insert({ "bash", "bash" });
    terminals.insert({ "CMD", "CMD" });
    if (scan) {
        terminals.clear();
        DetectTerminals(terminals);

    } else {
        // Try to read the terminals from previous scans. If we never scanned for terminals
        // before, this function does not affect "terminals" variable
        ReadTerminalOptionsFromDisk(terminals);
    }
#else
    wxUnusedVar(scan);
    terminals.insert({ "bash", "bash" });
#endif
    return terminals;
}

void clBuiltinTerminalPane::OnScanForTerminals(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxBusyCursor bc{};
    UpdateTerminalsChoice(true);
}

void clBuiltinTerminalPane::UpdateTerminalsChoice(bool scan)
{
    auto terminals = GetTerminalsOptions(scan);
#ifdef __WXMSW__
    int choiceWidth = 60;
#endif

    int initial_value = 0;
    wxString last_selection = clConfig::Get().Read("terminal/last_used_terminal", wxString());

    m_terminal_types->Clear();
    for (const auto& [name, command] : terminals) {
#ifdef __WXMSW__
        choiceWidth = wxMax(choiceWidth, GetTextExtent(name).GetWidth());
#endif
        int item_pos = m_terminal_types->Append(name, new wxStringClientData(command));
        if (!last_selection.empty() && last_selection == name) {
            initial_value = item_pos;
        }
    }

#ifdef __WXMSW__
    int controlWidth = choiceWidth == wxNOT_FOUND ? wxNOT_FOUND : FromDIP(choiceWidth);
    m_terminal_types->SetSize(controlWidth, wxNOT_FOUND);
    m_terminal_types->SetSizeHints(controlWidth, wxNOT_FOUND);
#endif

    if (!m_terminal_types->IsEmpty()) {
        m_terminal_types->SetSelection(initial_value);
    }
}
