#include "clBuiltinTerminalPane.hpp"

#include "ColoursAndFontsManager.h"
#include "CompilerLocator/CompilerLocatorMSVC.h"
#include "FontUtils.hpp"
#include "Platform/Platform.hpp"
#include "bitmap_loader.h"
#include "clFileName.hpp"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "ssh/ssh_account_info.h"
#include "terminal_view.h"
#include "wxTerminalOutputCtrl.hpp"

#include <wx/app.h>
#include <wx/choicdlg.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/xrc/xmlres.h>

namespace
{
std::map<wxString, wxString> LocateDefaultTerminals()
{
    std::map<wxString, wxString> terminals;
    auto bash = ThePlatform->Which("bash");
    auto cmd = ThePlatform->Which("powershell");
    if (bash.has_value()) {
        terminals.insert(
            {wxString::Format("%s --login -i", bash.value()), wxString::Format("%s --login -i", bash.value())});
    }

    if (cmd.has_value()) {
        terminals.insert({cmd.value(), cmd.value()});
    }
    return terminals;
}
} // namespace

clBuiltinTerminalPane::clBuiltinTerminalPane(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_book = new Notebook(this,
                          wxID_ANY,
                          wxDefaultPosition,
                          wxDefaultSize,
                          kNotebook_CloseButtonOnActiveTab | kNotebook_ShowFileListButton |
                              kNotebook_MouseMiddleClickClosesTab | kNotebook_FixedWidth | kNotebook_AllowDnD);

    m_toolbar = new wxAuiToolBar(this);
    GetSizer()->Add(m_toolbar, wxSizerFlags().Expand().Proportion(0));
    GetSizer()->Add(m_book, wxSizerFlags().Expand().Proportion(1));

    auto image_list = clGetManager()->GetStdIcons();
    m_toolbar->AddTool(wxID_NEW, _("New"), image_list->LoadBitmap("file_new"), wxEmptyString, wxITEM_NORMAL);

    // Get list of terminals
    m_terminal_types = new wxChoice(m_toolbar, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(300), wxNOT_FOUND));
    UpdateTerminalsChoice(false);
    m_toolbar->AddControl(m_terminal_types);

#ifdef __WXMSW__
    m_toolbar->AddTool(
        wxID_REFRESH, _("Scan"), image_list->LoadBitmap("debugger_restart"), wxEmptyString, wxITEM_NORMAL);
#endif

    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL, &clBuiltinTerminalPane::OnNew, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &clBuiltinTerminalPane::OnScanForTerminals, this, wxID_REFRESH);

    GetSizer()->Fit(this);
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &clBuiltinTerminalPane::OnPageChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
}

clBuiltinTerminalPane::~clBuiltinTerminalPane()
{
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &clBuiltinTerminalPane::OnPageChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
    clConfig::Get().Write("terminal/last_used_terminal", m_terminal_types->GetStringSelection());
}

void clBuiltinTerminalPane::OnWorkspaceLoaded(clWorkspaceEvent& event) { event.Skip(); }

TerminalView* clBuiltinTerminalPane::GetActiveTerminal()
{
    // when we add tabs, return the active selected tab's terminal
    if (m_book->GetPageCount() == 0) {
        return nullptr;
    }
    return static_cast<TerminalView*>(m_book->GetPage(m_book->GetSelection()));
}

void clBuiltinTerminalPane::OnNew(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if (m_terminal_types->IsEmpty()) {
        return;
    }

    int selection = m_terminal_types->GetSelection();
    if (selection == wxNOT_FOUND) {
        return;
    }

    wxStringClientData* cd = dynamic_cast<wxStringClientData*>(m_terminal_types->GetClientObject(selection));
    const wxString& cmd = cd->GetData();

    // By default, inherit parent's env.
    EnvSetter env_setter{};
    std::optional<TerminalView::EnvironmentList> env{std::nullopt};
    TerminalView* ctrl = new TerminalView(m_book, cmd, env);
    m_book->AddPage(ctrl, cmd, true);
    m_book->SetPageToolTip(m_book->GetPageCount() - 1, cmd);

    ctrl->Bind(wxEVT_TERMINAL_TITLE_CHANGED, [ctrl, this](wxTerminalEvent& event) {
        int where = m_book->FindPage(ctrl);
        if (where != wxNOT_FOUND) {
            m_book->SetPageText(where, event.GetTitle());
        }
    });
    ctrl->Bind(wxEVT_TERMINAL_TERMINATED, [ctrl, this](wxTerminalEvent& event) {
        wxUnusedVar(event);
        int where = m_book->FindPage(ctrl);
        if (where != wxNOT_FOUND) {
            m_book->DeletePage(where, ctrl);
        }
    });

    // Register standard keyboard shortcuts for the terminal.
    std::vector<wxAcceleratorEntry> V;
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'R', XRCID("Ctrl_ID_command")});
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'U', XRCID("Ctrl_ID_clear_line")});
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'L', XRCID("Ctrl_ID_clear_screen")});
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'D', XRCID("Ctrl_ID_logout")});
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'C', XRCID("Ctrl_ID_ctrl_c")});
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'W', XRCID("Ctrl_ID_delete_word")});
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'Z', XRCID("Ctrl_ID_suspend")});
    V.push_back(wxAcceleratorEntry{wxACCEL_ALT, (int)'B', XRCID("Alt_ID_backward")});
    V.push_back(wxAcceleratorEntry{wxACCEL_ALT, (int)'F', XRCID("Alt_ID_forward")});
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'A', XRCID("Ctrl_ID_start_of_line")});
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'E', XRCID("Ctrl_ID_end_of_line")});

    wxAcceleratorTable accel_table(V.size(), V.data());
    ctrl->SetAcceleratorTable(accel_table);

    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCtrlR, this, XRCID("Ctrl_ID_command"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCtrlU, this, XRCID("Ctrl_ID_clear_line"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCtrlL, this, XRCID("Ctrl_ID_clear_screen"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCtrlD, this, XRCID("Ctrl_ID_logout"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCtrlC, this, XRCID("Ctrl_ID_ctrl_c"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCtrlW, this, XRCID("Ctrl_ID_delete_word"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCtrlZ, this, XRCID("Ctrl_ID_suspend"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnAltB, this, XRCID("Alt_ID_backward"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnAltF, this, XRCID("Alt_ID_forward"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCtrlA, this, XRCID("Ctrl_ID_start_of_line"));
    ctrl->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCtrlE, this, XRCID("Ctrl_ID_end_of_line"));
}

void clBuiltinTerminalPane::OnSetTitle(wxTerminalEvent& event)
{
    event.Skip();
    wxWindow* win = dynamic_cast<wxWindow*>(event.GetEventObject());
    CHECK_PTR_RET(win);

    int index = m_book->FindPage(win);
    if (index != wxNOT_FOUND) {
        m_book->SetPageText(index, event.GetTitle());
    }
}

void clBuiltinTerminalPane::OnPageChanged(wxBookCtrlEvent& event)
{
    event.Skip();
    auto terminal = GetActiveTerminal();
    CHECK_PTR_RET(terminal);
    terminal->CallAfter(&TerminalView::SetFocus);
}

void clBuiltinTerminalPane::DetectTerminals(std::map<wxString, wxString>& terminals)
{
#ifdef __WXMSW__
    terminals = LocateDefaultTerminals();

    CompilerLocatorMSVC locator_msvc{};
    if (locator_msvc.Locate()) {
        // attempt to locate MSVC compilers
        const auto& compilers = locator_msvc.GetCompilers();
        for (auto compiler : compilers) {
            wxString build_tool = compiler->GetTool("MAKE");
            build_tool = build_tool.BeforeLast('>');
            build_tool.Prepend("CMD /K ");
            terminals.insert({"CMD for " + compiler->GetName(), build_tool});
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
        terminals.insert({name, command});
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
    std::map<wxString, wxString> terminals = LocateDefaultTerminals();
    ;
#ifdef __WXMSW__
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
    int initial_value = 0;
    wxString last_selection = clConfig::Get().Read("terminal/last_used_terminal", wxString());

    m_terminal_types->Clear();
    for (const auto& [name, command] : terminals) {
        int item_pos = m_terminal_types->Append(name, new wxStringClientData(command));
        if (!last_selection.empty() && last_selection == name) {
            initial_value = item_pos;
        }
    }

    if (!m_terminal_types->IsEmpty()) {
        m_terminal_types->SetSelection(initial_value);
    }
}

#define CHECK_IF_CAN_HANDLE(event)            \
    auto terminal = GetActiveTerminal();      \
    if (!terminal || !terminal->HasFocus()) { \
        event.Skip();                         \
        return;                               \
    }

void clBuiltinTerminalPane::OnCtrlR(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendCtrlR();
}

void clBuiltinTerminalPane::OnCtrlU(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendCtrlU();
}

void clBuiltinTerminalPane::OnCtrlL(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendCtrlL();
}

void clBuiltinTerminalPane::OnCtrlD(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendCtrlD();
}

void clBuiltinTerminalPane::OnCtrlC(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendCtrlC();
}

void clBuiltinTerminalPane::OnCtrlW(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendCtrlW();
}

void clBuiltinTerminalPane::OnCtrlZ(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendCtrlZ();
}

void clBuiltinTerminalPane::OnAltF(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendAltF();
}

void clBuiltinTerminalPane::OnAltB(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendAltB();
}

void clBuiltinTerminalPane::OnCtrlA(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendCtrlA();
}

void clBuiltinTerminalPane::OnCtrlE(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->SendCtrlE();
}
#undef CHECK_IF_CAN_HANDLE

void clBuiltinTerminalPane::OnInitDone(wxCommandEvent& e) { e.Skip(); }