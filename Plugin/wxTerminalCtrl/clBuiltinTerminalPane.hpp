#ifndef CLBUILTINTERMINALPANE_HPP
#define CLBUILTINTERMINALPANE_HPP

#include "Notebook.h"
#include "clAuiBook.hpp"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "event_notifier.h"
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
    wxTerminalViewCtrl* GetActiveTerminal();

    wxAuiToolBar* m_toolbar = nullptr;
    clAuiBook* m_book = nullptr;
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
