#include "clBuiltinTerminalPane.hpp"

#include "ColoursAndFontsManager.h"
#include "CompilerLocator/CompilerLocatorMSVC.h"
#include "FontUtils.hpp"
#include "Platform/Platform.hpp"
#include "bitmap_loader.h"
#include "clFileName.hpp"
#include "clINIParser.hpp"
#include "clWorkspaceManager.h"
#include "cl_aui_tool_stickness.h"
#include "codelite_events.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "procutils.h"
#include "ssh/ssh_account_info.h"
#include "terminal_view.h"
#include "wxTerminalOutputCtrl.hpp"

#include <unordered_map>
#include <wx/app.h>
#include <wx/choicdlg.h>
#include <wx/dir.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/xrc/xmlres.h>

namespace
{
std::vector<std::pair<wxString, wxString>> LocateDefaultTerminals()
{
    std::vector<std::pair<wxString, wxString>> terminals;

    // Common shells
#ifdef __WXMSW__
    std::vector<std::pair<wxString, wxString>> shells_to_find{
        {"cmd", wxEmptyString},
        {"powershell", wxEmptyString},
        {"bash", " --login -i"},
        {"zsh", " --login -i"},
    };
#else
    std::vector<std::pair<wxString, wxString>> shells_to_find{
        {"bash", " --login -i"},
        {"zsh", " --login -i"},
    };
#endif

    for (const auto& [shell, extra_args] : shells_to_find) {
        auto fullcmd = ThePlatform->Which(shell);
        if (!fullcmd.has_value()) {
            continue;
        }
        terminals.push_back(std::make_pair(wxString::Format("%s%s", fullcmd.value(), extra_args),
                                           wxString::Format("%s%s", fullcmd.value(), extra_args)));
    }

#ifdef __WXMSW__
    // Custom code to handle WSL terminals
    auto wsl = ThePlatform->Which("wsl");
    if (wsl.has_value()) {
        wxString wsl_command = StringUtils::WrapWithDoubleQuotes(wsl.value());
        clDEBUG() << "Found WSL:" << wsl_command << endl;

        // List the available distros we got.
        std::vector<wxString> command = {wsl.value(), "-l"};
        wxString list_output;

        clEnvList_t env;
        env.push_back({"WSL_UTF8", "1"});
        auto process = ::CreateAsyncProcess(nullptr, command, IProcessCreateSync, wxEmptyString, &env, wxEmptyString);
        if (process) {
            process->WaitForTerminate(list_output);
        }
        clDEBUG() << list_output << endl;

        wxArrayString lines = wxStringTokenize(list_output, "\r\n", wxTOKEN_STRTOK);
        for (auto& output_line : lines) {
            output_line.Trim().Trim(false);
            wxString lc_line = output_line.Lower();
            if (lc_line.Contains("windows subsystem for linux distributions") || lc_line.empty()) {
                clDEBUG() << "Ignoring line: [" << lc_line << "]" << endl;
                continue;
            }

            clDEBUG() << "  Checking WSL distro: '" << output_line << "'" << output_line.size() << " bytes" << endl;
            wxString title;
            title << "WSL: " << output_line;
            wxString cmd;
            cmd << wsl_command << " --distribution " << StringUtils::WrapWithDoubleQuotes(output_line) << " --cd ~";
            clDEBUG() << "Adding:" << title << "=>" << cmd << endl;
            terminals.push_back(std::make_pair(title, cmd));
        }
    }
#endif

    return terminals;
}
} // namespace

clBuiltinTerminalPane::clBuiltinTerminalPane(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
{
    // Load saved settings
    m_safeDrawingEnabled = clConfig::Get().Read("terminal/safe_drawing", false);

    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_book = new clAuiBook(this,
                           wxID_ANY,
                           wxDefaultPosition,
                           wxDefaultSize,
                           kNotebook_CloseButtonOnActiveTab | kNotebook_ShowFileListButton |
                               kNotebook_MouseMiddleClickClosesTab | kNotebook_AllowDnD);

    m_toolbar = new wxAuiToolBar(this);
    GetSizer()->Add(m_toolbar, wxSizerFlags().Expand().Proportion(0));
    GetSizer()->Add(m_book, wxSizerFlags().Expand().Proportion(1));

    auto image_list = clGetManager()->GetStdIcons();
    m_toolbar->AddTool(wxID_NEW, _("New"), image_list->LoadBitmap("file_new"), wxEmptyString, wxITEM_NORMAL);

    // Get list of terminals
    m_terminal_types =
        new wxChoice(m_toolbar, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(300), wxNOT_FOUND), wxArrayString{}, 0);
#ifdef __WXMAC__
    m_terminal_types->SetWindowVariant(wxWINDOW_VARIANT_MINI);
#else
    m_terminal_types->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif
    m_terminal_types->SetToolTip(_("Choose shell interpreter"));
    UpdateTerminalsChoice(false);
    m_toolbar->AddControl(m_terminal_types);

#ifdef __WXMSW__
    m_toolbar->AddTool(
        wxID_REFRESH, _("Scan"), image_list->LoadBitmap("debugger_restart"), _("Re-scan for terminals"), wxITEM_NORMAL);

#endif

    m_toolbar->AddSeparator();
    // Get list of terminals
    m_choice_themes =
        new wxChoice(m_toolbar, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(200), wxNOT_FOUND), wxArrayString{}, 0);
#ifdef __WXMAC__
    m_choice_themes->SetWindowVariant(wxWINDOW_VARIANT_MINI);
#else
    m_choice_themes->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

    m_toolbar->AddControl(m_choice_themes);
    m_choice_themes->SetToolTip(_("Choose terminal theme"));
    m_choice_themes->Bind(wxEVT_CHOICE, &clBuiltinTerminalPane::OnChoiceTheme, this);
    UpdateFont();
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(
        wxID_PREFERENCES, _("Settings"), image_list->LoadBitmap("cog"), _("Terminal Settings"), wxITEM_NORMAL);
    m_toolbar->SetToolDropDown(wxID_PREFERENCES, true);
    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL, &clBuiltinTerminalPane::OnNew, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &clBuiltinTerminalPane::OnScanForTerminals, this, wxID_REFRESH);
    m_toolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &clBuiltinTerminalPane::OnSettingsMenu, this, wxID_PREFERENCES);

    GetSizer()->Fit(this);
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &clBuiltinTerminalPane::OnPageChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clBuiltinTerminalPane::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clBuiltinTerminalPane::OnInitDone, this);

#ifdef __WXMAC__
    wxTheApp->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnCopy, this, wxID_COPY);
#endif
    wxTheApp->Bind(wxEVT_MENU, &clBuiltinTerminalPane::OnPaste, this, wxID_PASTE);
}

clBuiltinTerminalPane::~clBuiltinTerminalPane()
{
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &clBuiltinTerminalPane::OnPageChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clBuiltinTerminalPane::OnThemeChanged, this);
    clConfig::Get().Write("terminal/last_used_terminal", m_terminal_types->GetStringSelection());
}

void clBuiltinTerminalPane::OnWorkspaceLoaded(clWorkspaceEvent& event) { event.Skip(); }

wxTerminalViewCtrl* clBuiltinTerminalPane::GetActiveTerminal()
{
    // when we add tabs, return the active selected tab's terminal
    if (m_book->GetPageCount() == 0) {
        return nullptr;
    }
    return static_cast<wxTerminalViewCtrl*>(m_book->GetPage(m_book->GetSelection()));
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
    std::optional<wxTerminalViewCtrl::EnvironmentList> env{std::nullopt};
    wxTerminalViewCtrl* ctrl = new wxTerminalViewCtrl(m_book, cmd, env);
    ctrl->SetSelectionDelimChars(" \t\n\r()[]{}<>,;'\"@|&=*?!");
    ctrl->SetTheme(m_activeTheme.has_value() ? *m_activeTheme : wxTerminalTheme::MakeDarkTheme());
    m_book->AddPage(ctrl, cmd, true);

    // Apply safe drawing setting to the new terminal
    ctrl->EnableSafeDrawing(m_safeDrawingEnabled);
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
    ctrl->Bind(wxEVT_TERMINAL_TEXT_LINK, &clBuiltinTerminalPane::OnLinkClicked, this);

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

#ifdef __WXMAC__
    V.push_back(wxAcceleratorEntry{wxACCEL_CMD, (int)'V', wxID_PASTE});
    V.push_back(wxAcceleratorEntry{wxACCEL_CMD, (int)'C', wxID_COPY});
#else
    V.push_back(wxAcceleratorEntry{wxACCEL_RAW_CTRL, (int)'V', wxID_PASTE});
#endif

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
    terminal->CallAfter(&wxTerminalViewCtrl::SetFocus);
}

void clBuiltinTerminalPane::DetectTerminals(std::vector<std::pair<wxString, wxString>>& terminals)
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
            terminals.push_back({"CMD for " + compiler->GetName(), build_tool});
        }
    }
    WriteTerminalOptionsToDisk(terminals);
#endif
}

bool clBuiltinTerminalPane::ReadTerminalOptionsFromDisk(std::vector<std::pair<wxString, wxString>>& terminals)
{
    wxArrayString results = clConfig::Get().Read("terminal/options", wxArrayString{});
    if (results.empty() || results.size() % 2 != 0) {
        return false;
    }

    terminals.clear();
    // we serialise the map into vectr as pairs of: [key,value,key2,value...]
    for (size_t i = 0; i < results.size() / 2; ++i) {
        wxString name = results[i * 2];
        wxString command = results[i * 2 + 1];
        terminals.push_back(std::make_pair(name, command));
    }
    return true;
}

void clBuiltinTerminalPane::WriteTerminalOptionsToDisk(const std::vector<std::pair<wxString, wxString>>& terminals)
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

std::vector<std::pair<wxString, wxString>> clBuiltinTerminalPane::GetTerminalsOptions(bool scan)
{
    auto terminals = LocateDefaultTerminals();
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

void clBuiltinTerminalPane::OnInitDone(wxCommandEvent& e)
{
    e.Skip();
    std::thread([this]() {
        wxFileName themes_path{clStandardPaths::Get().GetDataDir(), wxEmptyString};
        themes_path.AppendDir("terminal_themes");
        themes_path.AppendDir("themes");
        wxArrayString theme_files;
        wxDir::GetAllFiles(themes_path.GetPath(), &theme_files, "*.toml", wxDIR_FILES);

        std::map<wxString, wxTerminalTheme> themes;
        for (const wxString& toml_file : theme_files) {
            wxFileName fn{toml_file};
            auto theme = clBuiltinTerminalPane::FromTOML(fn);
            if (theme.has_value()) {
                themes.insert({fn.GetName(), *theme});
            }
        }

        {
            wxMutexLocker locker(m_themes_mutex);
            m_themes.swap(themes);
        }
        EventNotifier::Get()->RunOnMain<void>([this]() { ThemesUpdated(); });
    }).detach();
}

#ifdef __WXMAC__
void clBuiltinTerminalPane::OnCopy(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->Copy();
}
#endif

void clBuiltinTerminalPane::OnPaste(wxCommandEvent& e)
{
    CHECK_IF_CAN_HANDLE(e);
    terminal->Paste();
}

void clBuiltinTerminalPane::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    UpdateFont();
    ApplyThemeChanges();
}

std::optional<wxTerminalTheme> clBuiltinTerminalPane::FromTOML(const wxFileName& filepath)
{
    clDEBUG() << "   > Importing Alacritty Theme (TOML) file:" << filepath << endl;
    std::string filename = filepath.GetFullPath().ToStdString(wxConvUTF8);

    clINIParser ini_parser;
    ini_parser.ParseFile(filepath.GetFullPath());

    wxTerminalTheme theme;
    theme.bg = ini_parser["colors.primary"]["background"].GetValue();
    theme.fg = ini_parser["colors.primary"]["foreground"].GetValue();

    if (!theme.bg.IsOk() || !theme.fg.IsOk()) {
        clSYSTEM() << "Can not import theme:" << filepath << endl;
        return std::nullopt;
    }

    bool is_dark = DrawingUtils::IsDark(theme.bg);

    wxString section_name = "colors.normal";
    theme.black = ini_parser[section_name]["black"].GetValue();
    theme.red = ini_parser[section_name]["red"].GetValue();
    theme.green = ini_parser[section_name]["green"].GetValue();
    theme.yellow = ini_parser[section_name]["yellow"].GetValue();
    theme.blue = ini_parser[section_name]["blue"].GetValue();
    theme.magenta = ini_parser[section_name]["magenta"].GetValue();
    theme.cyan = ini_parser[section_name]["cyan"].GetValue();
    theme.white = ini_parser[section_name]["white"].GetValue();

    section_name = "colors.bright";
    theme.brightBlack = ini_parser[section_name]["black"].GetValue();
    theme.brightRed = ini_parser[section_name]["red"].GetValue();
    theme.brightGreen = ini_parser[section_name]["green"].GetValue();
    theme.brightYellow = ini_parser[section_name]["yellow"].GetValue();
    theme.brightBlue = ini_parser[section_name]["blue"].GetValue();
    theme.brightMagenta = ini_parser[section_name]["magenta"].GetValue();
    theme.brightCyan = ini_parser[section_name]["cyan"].GetValue();
    theme.brightWhite = ini_parser[section_name]["white"].GetValue();

    // Optional colours
    theme.cursorColour = ini_parser["colors.cursor"]["cursor"].GetValue();
    theme.selectionBg = ini_parser["colors.selection"]["background"].GetValue();
    theme.selectionFg = ini_parser["colors.selection"]["text"].GetValue();
    theme.highlightBg = ini_parser["colors.search.matches"]["background"].GetValue();
    theme.highlightFg = ini_parser["colors.search.matches"]["foreground"].GetValue();

    if (!theme.cursorColour.IsOk()) {
        theme.cursorColour = is_dark ? *wxYELLOW : *wxBLACK;
    }

    if (!theme.selectionBg.IsOk() || !theme.selectionFg.IsOk()) {
        theme.selectionBg = is_dark ? wxT("#264F78") : wxT("#D1F2EB");
        theme.selectionFg = is_dark ? wxT("#FFFFFF") : wxT("#000000");
    }

    if (!theme.highlightBg.IsOk() || !theme.highlightFg.IsOk()) {
        theme.highlightBg = is_dark ? wxT("#FFF200") : wxT("#8A0885");
        theme.highlightFg = is_dark ? wxT("#1A1917") : wxT("#F7F2F7");
    }

    if (!theme.black.IsOk() || !theme.red.IsOk() || !theme.green.IsOk() || !theme.yellow.IsOk() || !theme.blue.IsOk() ||
        !theme.magenta.IsOk() || !theme.cyan.IsOk() || !theme.white.IsOk() || !theme.brightBlack.IsOk() ||
        !theme.brightRed.IsOk() || !theme.brightGreen.IsOk() || !theme.brightYellow.IsOk() ||
        !theme.brightBlue.IsOk() || !theme.brightMagenta.IsOk() || !theme.brightCyan.IsOk() ||
        !theme.brightWhite.IsOk()) {
        clSYSTEM() << "failed to read basic colour for theme:" << filepath << endl;
        return std::nullopt;
    }
    return theme;
}

void clBuiltinTerminalPane::ThemesUpdated()
{
    m_choice_themes->Clear();
    if (m_themes.empty()) {
        return;
    }
    for (const auto& [theme_name, _] : m_themes) {
        m_choice_themes->Append(theme_name);
    }

    wxString selected_theme = m_themes.begin()->first;
    selected_theme = clConfig::Get().Read("terminal/theme", selected_theme);
    m_choice_themes->SetStringSelection(selected_theme);

    if (m_themes.contains(selected_theme)) {
        m_activeTheme = m_themes[selected_theme];
    } else {
        m_activeTheme = wxTerminalTheme::MakeDarkTheme();
    }
    ApplyThemeChanges();
}

void clBuiltinTerminalPane::OnChoiceTheme(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString selected_theme = m_choice_themes->GetStringSelection();
    if (m_themes.contains(selected_theme)) {
        m_activeTheme = m_themes[selected_theme];
        clConfig::Get().Write("terminal/theme", selected_theme);
        clConfig::Get().Save();
    } else {
        m_activeTheme = wxTerminalTheme::MakeDarkTheme();
    }
    ApplyThemeChanges();
}

void clBuiltinTerminalPane::OnSettingsMenu(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxMenu menu;
    wxMenuItem* safeDrawingItem = menu.AppendCheckItem(wxID_ANY, _("Enable Safe Drawing"));
    safeDrawingItem->Check(m_safeDrawingEnabled);

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            m_safeDrawingEnabled = !m_safeDrawingEnabled;
            clDEBUG() << "Safe Drawing:" << (m_safeDrawingEnabled ? "Enabled" : "Disabled") << endl;

            // Persist the setting
            clConfig::Get().Write("terminal/safe_drawing", m_safeDrawingEnabled);
            clConfig::Get().Save();

            // Apply to all terminals
            for (size_t i = 0; i < m_book->GetPageCount(); ++i) {
                auto terminal = dynamic_cast<wxTerminalViewCtrl*>(m_book->GetPage(i));
                if (terminal) {
                    terminal->EnableSafeDrawing(m_safeDrawingEnabled);
                    terminal->Refresh();
                }
            }
        },
        safeDrawingItem->GetId());

    wxRect rect = m_toolbar->GetToolRect(wxID_PREFERENCES);
    wxPoint pt = m_toolbar->ClientToScreen(rect.GetBottomLeft());
    pt = ScreenToClient(pt);

    clAuiToolStickness st{m_toolbar, event.GetId()};
    PopupMenu(&menu, pt);
}

void clBuiltinTerminalPane::ApplyThemeChanges()
{
    if (!m_activeTheme.has_value()) {
        return;
    }

    m_activeTheme.value().font = m_activeFont;

    for (size_t i = 0; i < m_book->GetPageCount(); ++i) {
        auto terminal = dynamic_cast<wxTerminalViewCtrl*>(m_book->GetPage(i));
        if (terminal) {
            terminal->SetTheme(m_activeTheme.value());
        }
    }
}

void clBuiltinTerminalPane::UpdateFont()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if (lexer) {
        m_activeFont = lexer->GetFontForStyle(0, this);
    }
}

void clBuiltinTerminalPane::OnLinkClicked(wxTerminalEvent& event)
{
    clDEBUG() << "Text clicked inside terminal:" << event.GetClickedText() << endl;
    const wxString& text = event.GetClickedText();
    if (text.StartsWith("http://") || text.StartsWith("https://")) {
        ::wxLaunchDefaultBrowser(text);
        return;
    }

    if (wxFileName::DirExists(text)) {
        CallAfter([text]() { FileUtils::OpenFileExplorer(text); });
        return;
    }

    wxFileName fn{text};
    if (FileUtils::IsBinaryExecutable(fn)) {
        ::wxLaunchDefaultApplication(fn.GetFullPath());
        return;
    }

    auto res = FileUtils::ParseTriplet(text);
    if (res.has_value()) {
        const auto& triplet = res.value();
        clDEBUG() << "Firing event for file:" << triplet.filename << ", line:" << triplet.line_number
                  << ", column:" << triplet.column << endl;
        clBuildEvent event_clicked(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED);
        event_clicked.SetBuildDir(wxEmptyString); // can be empty
        event_clicked.SetFileName(triplet.filename);
        event_clicked.SetLineNumber(triplet.line_number);
        EventNotifier::Get()->AddPendingEvent(event_clicked);
    } else {
        clDEBUG() << "Failed to parse file triplet:" << text << endl;
    }
}
