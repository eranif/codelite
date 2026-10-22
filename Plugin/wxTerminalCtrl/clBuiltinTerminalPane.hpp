#ifndef CLBUILTINTERMINALPANE_HPP
#define CLBUILTINTERMINALPANE_HPP

#include "Keyboard/clKeyboardManager.h"
#include "clAuiBook.hpp"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "ssh/ssh_account_info.h"
#include "terminal_event.h"
#include "terminal_theme.h"
#include "wxTerminalCtrl/TerminalSettingsDlg.hpp"

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <wx/aui/auibar.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/thread.h>

class wxTerminalViewCtrl;

#ifdef __WXMSW__
inline constexpr const char kTerminalCommand[] = "cmd.exe";
#else
inline constexpr const char kTerminalCommand[] = "/bin/bash --login -i";
#endif

// Translate wxTerminalViewCtrl events into exported events (these events can be used from outside the DLL)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BUILTIN_TERMINAL_TERMINATED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BUILTIN_TERMINAL_TITLE_CHANGED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BUILTIN_TERMINAL_TEXT_LINK_CLICKED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BUILTIN_TERMINAL_BELL, clCommandEvent);

/// The parent of a terminal control: a panel with a vertical sizer, so the terminal can show its progress bar above
/// itself. The events of the terminal are fired with the terminal control (not the host) as the event object.
class WXDLLIMPEXP_SDK TerminalHost : public wxPanel
{
public:
    explicit TerminalHost(wxWindow* parent);

    /// The terminal control, or nullptr if it was not set
    wxTerminalViewCtrl* GetCtrl() { return m_terminal; }

private:
    friend class clBuiltinTerminalPane;
    /// Set the terminal control (which must be a child of this panel). Can be called once.
    void SetCtrl(wxTerminalViewCtrl* ctrl);

    wxTerminalViewCtrl* m_terminal{nullptr};
};

class WXDLLIMPEXP_SDK clBuiltinTerminalPane : public wxPanel
{
public:
    clBuiltinTerminalPane(wxWindow* parent, wxWindowID id = wxID_ANY);
    ~clBuiltinTerminalPane() override;

    /**
     * @brief Create a new terminal host that runs `shellCmd`, with `parent` as its parent window. The host is
     * detached: it is not added to the notebook of this pane, the caller must add it to its own layout.
     * @param parent The parent window of the host
     * @param shellCmd The command that launches the shell
     * @param workingDirectory The working directory (empty: the workspace directory, if it is a local one)
     * @param sshAccount Optional SSH account: connect to it from the new shell
     * @param persistTabTitle If true, the title of the shell does not change the title of the tab
     * @return the new host, or nullptr on failure
     */
    TerminalHost* NewTerminalHost(wxWindow* parent,
                                  const wxString& shellCmd,
                                  const wxString& workingDirectory,
                                  const std::optional<SSHAccountInfo>& sshAccount = std::nullopt,
                                  bool persistTabTitle = false);

    /**
     * @brief Create a new terminal, add it as a tab of this pane, select it and show the Terminal pane
     * @param tabTitle Optional title for the tab (defaults to the shell command). A title set by the caller is kept:
     * the title of the shell does not change it.
     * @return the new host, or nullptr on failure
     */
    TerminalHost* AddTerminal(const wxString& shellCmd,
                              const wxString& workingDirectory,
                              const std::optional<SSHAccountInfo>& sshAccount = std::nullopt,
                              const wxString& tabTitle = wxEmptyString);

    /// Find a tab of this pane by its title. If `makeVisible` is true and the tab is found, select it and show the
    /// Terminal pane. Returns nullptr if there is no such tab.
    TerminalHost* FindTerminalByTitle(const wxString& tabTitle, bool makeVisible = false);
    /// Close the first tab whose title is `tabTitle` (does nothing if there is none)
    void CloseTerminalByTitle(const wxString& tabTitle);
    /// The terminal of the selected tab, or nullptr if there are no tabs
    TerminalHost* GetActiveTerminal();

    wxString GetDefaultShell() const { return kTerminalCommand; }

    std::optional<wxString> PromptForTerminal();
    bool OpenLink(const wxString& linkText);

protected:
    void PromptForNewTerminal();
    TerminalHost*
    DoCreateTerminal(wxWindow* parent, const wxString& shellCommand, std::optional<wxString> workingDirectory);
    void BindTerminalEvents(TerminalHost* host, bool persistTabTitle);
    void DoOpenLink(const wxString& linkText);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnInitDone(wxCommandEvent& e);
    void OnPageChanged(wxBookCtrlEvent& event);
    void OnLinkClicked(wxTerminalEvent& event);
    void OnTerminalBell(wxTerminalEvent& event);
    void OnNew(wxCommandEvent& event);
    void DetectTerminals(std::vector<std::pair<wxString, wxString>>& terminals);
    bool ReadTerminalOptionsFromDisk(std::vector<std::pair<wxString, wxString>>& terminals);
    void WriteTerminalOptionsToDisk(const std::vector<std::pair<wxString, wxString>>& terminals);
    std::vector<std::pair<wxString, wxString>> GetTerminalsOptions(bool scan = false);
    void OnCtrlR(wxCommandEvent& e);
    void OnCtrlU(wxCommandEvent& e);
    void OnCtrlL(wxCommandEvent& e);
    void OnCtrlD(wxCommandEvent& e);
    void OnCtrlC(wxCommandEvent& e);
    void OnCtrlW(wxCommandEvent& e);
    void OnCtrlZ(wxCommandEvent& e);
    void OnAltF(wxCommandEvent& e);
    void OnAltB(wxCommandEvent& e);
    void OnCtrlA(wxCommandEvent& e);
    void OnCtrlE(wxCommandEvent& e);
    void OnCtrlLetter(wxCommandEvent& e);
    void OnTerminalShortcutCharHook(wxKeyEvent& e);
    /// The terminal consumes its special keys (function keys, Ctrl-<key>, ...), so the shortcuts of the main frame do
    /// not work while it has the focus. The terminal calls this filter for these keys: if the key is the shortcut of
    /// one of the frame's actions (for example F6: "show_terminal_pane"), fire the action and return true, so the
    /// terminal ignores the key. Otherwise return false.
    bool HandleTerminalShortcut(const wxKeyEvent& event);
    /// The actions (command IDs) of the main frame that can be triggered from a terminal, with their shortcuts. Built
    /// on first use, and rebuilt after the user changes the shortcuts
    const std::vector<std::pair<int, clKeyboardShortcut>>& GetFrameActions();
    void OnKeyboardShortcutsUpdated(clCommandEvent& event);
    void OnPaste(wxCommandEvent& e);
#ifdef __WXMAC__
    void OnCopy(wxCommandEvent& e);
#endif

    void OnThemeChanged(clCommandEvent& event);
    void ThemesUpdated();
    /// Called on the main thread with the (name, content) of the theme files read by the background thread
    void OnTOMLFilesLoaded(const std::vector<std::pair<std::string, std::string>>& files);
    void OnChoiceTheme(wxCommandEvent& event);
    void ApplyThemeChanges();
    void ApplySettings();
    void UpdateFont();
    void OnSettings(wxCommandEvent& event);

private:
    /// Create a theme from the content of an Alacritty theme (TOML) file. `name` is used for logging only
    static std::optional<wxTerminalTheme> FromTOML(const wxString& name, const wxString& content);
    wxAuiToolBar* m_toolbar = nullptr;
    wxAuiNotebook* m_book = nullptr;
    wxChoice* m_choice_themes = nullptr;
    std::vector<std::pair<EventFilterCallbackToken, wxEventType>> m_tokens;
    std::map<wxString, wxTerminalTheme> m_themes; // accessed from the main thread only
    std::optional<wxTerminalTheme> m_activeTheme{std::nullopt};
    wxFont m_activeFont;
    TerminalSettings m_terminalSettings;
    std::optional<std::vector<std::pair<int, clKeyboardShortcut>>> m_frameActions{std::nullopt};
};

#endif // CLBUILTINTERMINALPANE_HPP
