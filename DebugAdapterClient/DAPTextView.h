#ifndef DAPTEXTVIEW_H
#define DAPTEXTVIEW_H

#include "UI.h"
#include "cl_command_event.h"
#include "dap/dap.hpp"

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

public:
    DAPTextView(wxWindow* parent);
    virtual ~DAPTextView();

    wxStyledTextCtrl* GetCtrl() { return m_stcTextView; }
    void ClearMarker();
    void SetMarker(int line_number);
    void SetText(const dap::Source& source, const wxString& text, const wxString& path, const wxString& mimeType);
    void LoadFile(const dap::Source& source, const wxString& filepath);

    const wxString& GetFilePath() const { return m_filepath; }
    bool IsSame(const dap::Source& source) const { return m_current_source == source; }
};

#endif // DAPTEXTVIEW_H
