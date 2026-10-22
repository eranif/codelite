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
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BUILTIN_TERMINAL_PROGRAM_STATUS, clCommandEvent);

/**
 * @brief A panel that holds one terminal control.
 *
 * The panel has a vertical sizer. This lets the terminal show its progress bar above itself.
 * The terminal fires its events with the terminal control (not this panel) as the event object.
 */
class WXDLLIMPEXP_SDK TerminalHost : public wxPanel
{
public:
    /**
     * @brief Create an empty host. The terminal control is added by clBuiltinTerminalPane.
     * @param parent The parent window
     */
    explicit TerminalHost(wxWindow* parent);

    /**
     * @brief Get the terminal control of this host.
     * @return The terminal control, or nullptr if it was not created yet
     */
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
    /**
     * @brief Create the pane. It has a toolbar and a notebook; every tab of the notebook is a terminal.
     * @param parent The parent window
     * @param id The window ID
     */
    clBuiltinTerminalPane(wxWindow* parent, wxWindowID id = wxID_ANY);
    ~clBuiltinTerminalPane() override;

    /**
     * @brief Create a terminal that is not a tab of this pane.
     *
     * The caller owns the layout: it must place the returned host in its own window, for example in a sizer or a
     * splitter. To get a new tab, use AddTerminal().
     *
     * @param parent The parent window of the new host
     * @param shellCmd The command that starts the shell
     * @param workingDirectory The directory to start in. If empty, the workspace directory is used (local workspaces
     * only)
     * @param sshAccount If set, the shell connects to this SSH account
     * @param persistTabTitle If true, the title that the shell sets does not change the tab title
     * @return The new host, or nullptr if it could not be created
     */
    TerminalHost* NewTerminalHost(wxWindow* parent,
                                  const wxString& shellCmd,
                                  const wxString& workingDirectory,
                                  const std::optional<SSHAccountInfo>& sshAccount = std::nullopt,
                                  bool persistTabTitle = false);

    /**
     * @brief Create a terminal as a new tab of this pane, select the tab and show the Terminal pane.
     * @param shellCmd The command that starts the shell
     * @param workingDirectory The directory to start in. If empty, the workspace directory is used (local workspaces
     * only)
     * @param sshAccount If set, the shell connects to this SSH account
     * @param tabTitle The title of the tab. If empty, the shell command is used, and the shell can change the title.
     * If not empty, the title stays as it is, so you can find the tab later with FindTerminalByTitle().
     * @return The new host, or nullptr if it could not be created
     */
    TerminalHost* AddTerminal(const wxString& shellCmd,
                              const wxString& workingDirectory,
                              const std::optional<SSHAccountInfo>& sshAccount = std::nullopt,
                              const wxString& tabTitle = wxEmptyString);

    /**
     * @brief Find the tab with the given title.
     * @param tabTitle The title to look for
     * @param makeVisible If true and the tab exists, select it and show the Terminal pane
     * @return The host of the first tab with this title, or nullptr if there is none
     */
    TerminalHost* FindTerminalByTitle(const wxString& tabTitle, bool makeVisible = false);

    /**
     * @brief Close the first tab with the given title. Does nothing if there is no such tab.
     * @param tabTitle The title of the tab to close
     */
    void CloseTerminalByTitle(const wxString& tabTitle);

    /**
     * @brief Get the terminal of the selected tab.
     * @return The host of the selected tab, or nullptr if there are no tabs
     */
    TerminalHost* GetActiveTerminal();

    /**
     * @brief Get the command that starts the default shell of the platform.
     * @return `cmd.exe` on Windows, `/bin/bash --login -i` on other platforms
     */
    wxString GetDefaultShell() const { return kTerminalCommand; }

    /**
     * @brief Let the user choose a shell. If only one shell is available, return it without asking.
     *
     * The chosen shell is saved as the default for the next time.
     *
     * @return The command of the shell, or std::nullopt if there is no shell or the user cancelled
     */
    std::optional<wxString> PromptForTerminal();

    /**
     * @brief Open text that the user clicked in a terminal.
     *
     * The text can be: a URL (opens in the browser), a folder (opens in the file explorer), an executable (is
     * launched), or `file:line:column` (fires wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED, so the editor opens the file).
     *
     * @param linkText The text that was clicked
     * @return True if the text was handled
     */
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
    void OnProgramStatus(wxTerminalEvent& event);
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
