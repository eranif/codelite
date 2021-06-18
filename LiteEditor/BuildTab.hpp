#ifndef BUILDTAB_HPP
#define BUILDTAB_HPP

#include "buildtabsettingsdata.h"
#include "clAsciiEscapeColourBuilder.hpp"
#include "clDataViewListCtrl.h"
#include "cl_command_event.h"
#include "cl_editor.h"
#include "compiler.h"
#include <wx/panel.h>

class clControlWithItemsRowRenderer;
class BuildTab : public wxPanel
{
    clDataViewListCtrl* m_view = nullptr;
    clControlWithItemsRowRenderer* m_renderer = nullptr;
    BuildTabSettingsData m_buildTabSettings;
    
    // cleanable properties (between builds)
    bool m_buildInProgress = false;
    wxString m_buffer;
    CompilerPtr m_activeCompiler;
    size_t m_error_count = 0;
    size_t m_warn_count = 0;

protected:
    void OnBuildStarted(clBuildEvent& e);
    void OnBuildAddLine(clBuildEvent& e);
    void OnBuildEnded(clBuildEvent& e);
    void OnSysColourChanged(clCommandEvent& e);
    void OnLineActivated(wxDataViewEvent& e);
    void OnContextMenu(wxDataViewEvent& e);

    void ProcessBuffer(bool last_line = false);
    void Cleanup();
    void ApplyStyle();
    wxString WrapLineInColour(const wxString& line, eAsciiColours colour) const;
    void DoCentreErrorLine(Compiler::PatternMatch* match_result, clEditor* editor, bool centerLine);
    void SaveBuildLog();
    void CopySelections();

public:
    BuildTab(wxWindow* parent);
    ~BuildTab();

    void AppendLine(const wxString& text);
    void ClearView();
};

#endif // BUILDTAB_HPP
