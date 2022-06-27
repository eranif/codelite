#ifndef DAPTEXTVIEW_H
#define DAPTEXTVIEW_H

#include "UI.h"
#include "cl_command_event.h"
#include "dap/dap.hpp"

#include <vector>

class DAPTextView : public DAPTextViewBase
{
    wxString m_filepath;
    dap::Source m_current_source;
    wxString m_mimeType;

protected:
    void ApplyTheme();
    void Clear();
    void SetFilePath(const wxString& filepath);
    void UpdateLineNumbersMargin();

    void OnColourChanged(clCommandEvent& event);
    void OnMarginClick(wxStyledTextEvent& event);

public:
    DAPTextView(wxWindow* parent);
    virtual ~DAPTextView();

    wxStyledTextCtrl* GetCtrl() { return m_stcTextView; }

    static void ClearMarker(wxStyledTextCtrl* ctrl);
    void ClearMarker() { ClearMarker(m_stcTextView); }

    static void SetMarker(wxStyledTextCtrl* ctrl, int line_number);
    void SetMarker(int line_number) { SetMarker(m_stcTextView, line_number); }

    void SetText(const dap::Source& source, const wxString& text, const wxString& path, const wxString& mimeType);
    void LoadFile(const dap::Source& source, const wxString& filepath);

    const wxString& GetFilePath() const { return m_filepath; }
    bool IsSame(const dap::Source& source) const { return m_current_source == source; }
    void DeleteBreakpointMarkers(int line_number = wxNOT_FOUND);
    void SetBreakpointMarker(int line_number, const wxString& tooltip);
    bool HasBreakpointMarker(int line_number);
    size_t GetBreakpointMarkers(std::vector<int>* lines);
};

#endif // DAPTEXTVIEW_H
