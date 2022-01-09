#ifndef BUILDTAB_HPP
#define BUILDTAB_HPP

#include "buildtabsettingsdata.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clTerminalViewCtrl.hpp"
#include "cl_command_event.h"
#include "cl_editor.h"
#include "compiler.h"

#include <wx/panel.h>
#include <wx/stopwatch.h>

class BuildTab : public wxPanel
{
    clTerminalViewCtrl* m_view = nullptr;
    BuildTabSettingsData m_buildTabSettings;
    wxStopWatch m_sw;

    // cleanable properties (between builds)
    bool m_buildInProgress = false;
    wxString m_buffer;
    CompilerPtr m_activeCompiler;
    size_t m_error_count = 0;
    size_t m_warn_count = 0;
    bool m_buildInterrupted = false;
    wxString m_currentProjectName;

protected:
    void OnBuildStarted(clBuildEvent& e);
    void OnBuildAddLine(clBuildEvent& e);
    void OnBuildEnded(clBuildEvent& e);
    void OnLineActivated(wxDataViewEvent& e);
    void OnContextMenu(wxDataViewEvent& e);
    void OnNextBuildError(wxCommandEvent& event);
    void OnNextBuildErrorUI(wxUpdateUIEvent& event);
    size_t GetNextLineWithErrorOrWarning(size_t from, bool errors_only = false) const;
    void SelectFirstErrorOrWarning(size_t from);

    void ProcessBuffer(bool last_line = false);
    void Cleanup();
    void ProcessBuildingProjectLine(const wxString& line);
    const wxString& GetCurrentProjectName() const { return m_currentProjectName; }
    wxString WrapLineInColour(const wxString& line, eAsciiColours colour, bool fold_font = false) const;
    void SaveBuildLog();
    void CopySelections();
    void CopyAll();
    wxString CreateSummaryLine();

public:
    BuildTab(wxWindow* parent);
    ~BuildTab();

    void AppendLine(const wxString& text);
    void ClearView();

    // a synonym to ClearView()
    void Clear() { ClearView(); }

    bool GetBuildEndedSuccessfully() const { return m_error_count == 0 && !m_buildInterrupted; }
    void SetBuildInterrupted(bool b) { m_buildInterrupted = b; }
};

#endif // BUILDTAB_HPP
