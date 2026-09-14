#ifndef CLBUILTINTERMINALPANE_HPP
#define CLBUILTINTERMINALPANE_HPP

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

class WXDLLIMPEXP_SDK clBuiltinTerminalPane : public wxPanel
{
public:
    clBuiltinTerminalPane(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clBuiltinTerminalPane();

    /**
     * @brief Open a new terminal tab with the specified working directory
     * @param workingDirectory The working directory for the terminal
     * @param sshAccount Optional SSH account for remote connections
     * @param tabTitle Optional custom title for the tab (defaults to the shell name)
     * @param makeVisible If true, shows the Terminal pane and selects the new tab
     * @return pointer to the created terminal control, or nullptr on failure
     */
    wxTerminalViewCtrl* OpenNewTerminalTab(const wxString& workingDirectory,
                                           const std::optional<SSHAccountInfo>& sshAccount = std::nullopt,
                                           const wxString& tabTitle = wxEmptyString,
                                           bool makeVisible = true,
                                           std::optional<wxString> terminal_cmd = std::nullopt,
                                           wxBookCtrlBase* book = nullptr);

    /**
     * @brief Open new terminal, this is similar to the user clicking the "+" button
     * on the toolbar.
     */
    void NewTerminal();

    /**
     * @brief Open a new terminal tab using the platform default shell.
     * @param workingDirectory The working directory for the terminal.
     * @param sshAccount Optional SSH account for remote connections.
     * @param tabTitle Optional custom title for the tab (defaults to the shell name).
     * @return pointer to the created terminal control, or nullptr on failure.
     */
    wxTerminalViewCtrl* OpenNewDefaultTerminalTab(const wxString& workingDirectory,
                                                  const std::optional<SSHAccountInfo>& sshAccount = std::nullopt,
                                                  const wxString& tabTitle = wxEmptyString)
    {
        return OpenNewTerminalTab(workingDirectory, sshAccount, tabTitle, true, kTerminalCommand);
    }

    /**
     * @brief Find an existing terminal tab by its title
     * @param tabTitle The title of the tab to find
     * @param makeVisible If true and tab is found, shows the Terminal pane and selects the tab
     * @return pointer to the terminal control if found, or nullptr if not found
     */
    wxTerminalViewCtrl* FindTerminalByTitle(const wxString& tabTitle, bool makeVisible = false);

    /**
     * @brief Closes the first terminal tab whose title matches the given text.
     *
     * Searches the associated notebook control for a page with a matching title and
     * deletes that page if found. Only the first matching tab is removed; if no
     * matching title exists, the method does nothing.
     *
     * @param tabTitle const wxString& The title of the terminal tab to close.
     *
     * @return void This function does not return a value.
     */
    void CloseTerminalByTitle(const wxString& tabTitle);
    wxTerminalViewCtrl* GetActiveTerminal();

    /**
     * Creates and initializes a new terminal control instance within a notebook pane.
     *
     * This method handles the instantiation of the wxTerminalViewCtrl, configures its
     * visual settings (buffer size, theme, and delimiters), adds it as a page to the
     * provided book control, and sets up keyboard accelerators and event bindings.
     *
     * @param book The notebook control (wxBookCtrlBase) where the terminal page will be added.
     * @param shellCommand The command string used to launch the terminal shell.
     * @param tabTitle The initial text to display on the notebook tab.
     * @param makeActive Whether the newly created terminal tab should be selected/active.
     * @param persistTabTitle If true, prevents the tab title from being automatically updated by the shell.
     * @param bindEvents Whether to bind terminal events (title changes, termination, and links) to handlers.
     * @param workingDirectory An optional string specifying the directory where the shell should start.
     *
     * @return A pointer to the newly created wxTerminalViewCtrl instance.
     */
    wxTerminalViewCtrl* CreateTerminal(wxBookCtrlBase* book,
                                       const wxString& shellCommand,
                                       const wxString& tabTitle,
                                       bool makeActive,
                                       bool persistTabTitle,
                                       bool bindEvents,
                                       std::optional<wxString> workingDirectory);

    std::optional<wxString> PromptForTerminal();

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnInitDone(wxCommandEvent& e);
    void OnPageChanged(wxBookCtrlEvent& event);
    void OnLinkClicked(wxTerminalEvent& event);
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
    void OnPaste(wxCommandEvent& e);
#ifdef __WXMAC__
    void OnCopy(wxCommandEvent& e);
#endif

    void OnThemeChanged(clCommandEvent& event);
    void ThemesUpdated();
    void OnChoiceTheme(wxCommandEvent& event);
    void ApplyThemeChanges();
    void ApplySettings();
    void UpdateFont();
    void OnSettings(wxCommandEvent& event);
    void DoOpenLink(const wxString& linkText);

private:
    static std::optional<wxTerminalTheme> FromTOML(const wxFileName& filepath);
    wxAuiToolBar* m_toolbar = nullptr;
    wxAuiNotebook* m_book = nullptr;
    wxChoice* m_choice_themes = nullptr;
    std::vector<std::pair<EventFilterCallbackToken, wxEventType>> m_tokens;
    wxMutex m_themes_mutex;
    std::map<wxString, wxTerminalTheme> m_themes;
    std::optional<wxTerminalTheme> m_activeTheme{std::nullopt};
    wxFont m_activeFont;
    TerminalSettings m_terminalSettings;
};

#endif // CLBUILTINTERMINALPANE_HPP
