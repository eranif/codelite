#ifndef CLBUILTINTERMINALPANE_HPP
#define CLBUILTINTERMINALPANE_HPP

#include "clAuiBook.hpp"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "ssh/ssh_account_info.h"
#include "terminal_event.h"
#include "terminal_theme.h"

#include <map>
#include <optional>
#include <vector>
#include <wx/aui/auibar.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/thread.h>

class wxTerminalViewCtrl;

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
                                           std::optional<wxString> terminal_cmd = std::nullopt);

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

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnInitDone(wxCommandEvent& e);
    void OnPageChanged(wxBookCtrlEvent& event);
    void OnSetTitle(wxTerminalEvent& event);
    void OnLinkClicked(wxTerminalEvent& event);
    void OnNew(wxCommandEvent& event);
    void DetectTerminals(std::vector<std::pair<wxString, wxString>>& terminals);
    bool ReadTerminalOptionsFromDisk(std::vector<std::pair<wxString, wxString>>& terminals);
    void WriteTerminalOptionsToDisk(const std::vector<std::pair<wxString, wxString>>& terminals);
    std::vector<std::pair<wxString, wxString>> GetTerminalsOptions(bool scan = false);
    void OnScanForTerminals(wxCommandEvent& event);
    void UpdateTerminalsChoice(bool scan);
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
    void UpdateFont();
    void OnSettingsMenu(wxCommandEvent& event);
    void DoOpenLink(const wxString& linkText);

private:
    static std::optional<wxTerminalTheme> FromTOML(const wxFileName& filepath);
    /**
     * @brief Helper method to create and setup a terminal control
     * @param shellCommand The shell command to run
     * @param tabTitle The title for the terminal tab
     * @param makeActive If true, make this tab the active tab
     * @param persistTabTitle If true, the initial tab title is persisted and the control will ignore any attempt to
     * change it using ANSI escape sequences.
     */
    wxTerminalViewCtrl*
    DoCreateTerminal(const wxString& shellCommand, const wxString& tabTitle, bool makeActive, bool persistTabTitle);

    wxAuiToolBar* m_toolbar = nullptr;
    wxAuiNotebook* m_book = nullptr;
    wxChoice* m_terminal_types = nullptr;
    wxChoice* m_choice_themes = nullptr;
    std::vector<std::pair<EventFilterCallbackToken, wxEventType>> m_tokens;
    wxMutex m_themes_mutex;
    std::map<wxString, wxTerminalTheme> m_themes;
    std::optional<wxTerminalTheme> m_activeTheme{std::nullopt};
    wxFont m_activeFont;
    bool m_safeDrawingEnabled = false;
};

#endif // CLBUILTINTERMINALPANE_HPP
