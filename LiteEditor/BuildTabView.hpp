#pragma once

#include "compiler.h"

#include <map>
#include <memory>
#include <optional>
#include <wx/stc/stc.h>

struct LineClientData {
    wxString project_name;
    // use this as the root folder for changing relative paths to abs. If empty, use the workspace path
    wxString root_dir;
    Compiler::PatternMatch match_pattern;
    wxString message;
    wxString toolchain;
};

class BuildTabView : public wxStyledTextCtrl
{
public:
    BuildTabView(wxWindow* parent);
    virtual ~BuildTabView();

    /// Append text to the control.
    ///
    /// This function adds complete lines (i.e. line that ends with a line terminator)
    /// to the view and parses them for errors / warnings.
    ///
    /// Returns:
    /// If the last line in the output is not completed (i.e. it does not end with a line terminator)
    /// it is returned for later processing (unless `process_last_line` is `true`)
    wxString Add(const wxString& output, bool process_last_line = false);

    /// Clear the view and all parsed information
    void Clear();

    /// Initialise the view, preparing it for the next build process. This method should be called when a new build
    /// is starting
    void Initialise(CompilerPtr compiler, bool jump_through_errors_only)
    {
        Clear();
        m_activeCompiler = compiler; // maybe null
        m_jumpThroughErrorsOnly = jump_through_errors_only;
    }

    size_t GetErrorCount() const { return m_errorCount; }
    size_t GetWarnCount() const { return m_warnCount; }

    /// Select the first error / warning message starting from line `from`
    void SelectFirstErrorOrWarning(size_t from, bool errors_only);

protected:
    void OnLeftDown(wxMouseEvent& e);
    void OnLeftUp(wxMouseEvent& e);
    void DoPatternClicked(const wxString& pattern, int pattern_line);
    void OnNextBuildError(wxCommandEvent& event);
    void OnNextBuildErrorUI(wxUpdateUIEvent& event);
    std::optional<std::pair<int, std::shared_ptr<LineClientData>>>
    GetNextLineWithErrorOrWarning(size_t from, bool errors_only) const;
    void ClearLineMarker();
    void SetLineMarker(size_t line);
    void OpenEditor(const wxString& filename, int line, int col, const wxString& wd = {});
    void OpenEditor(std::shared_ptr<LineClientData> line_info);
    void InitialiseView();
    void OnThemeChanged(wxCommandEvent& e);

private:
    std::map<size_t, std::shared_ptr<LineClientData>> m_lineInfo;
    CompilerPtr m_activeCompiler;
    bool m_jumpThroughErrorsOnly = false;
    size_t m_errorCount = 0;
    size_t m_warnCount = 0;
    wxString m_currentProject;
    int m_indicatorStartPos = wxNOT_FOUND;
    int m_indicatorEndPos = wxNOT_FOUND;
};
