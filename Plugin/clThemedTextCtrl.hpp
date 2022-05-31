#ifndef CLTHEMEDTEXTCTRL_HPP
#define CLTHEMEDTEXTCTRL_HPP

#include "clEditorEditEventsHandler.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clThemedTextCtrl : public wxStyledTextCtrl
{
protected:
    clEditEventsHandler::Ptr_t m_editEventsHandler;

protected:
    void OnKeyDown(wxKeyEvent& event);
    void OnAddChar(wxStyledTextEvent& event);
    void OnChange(wxStyledTextEvent& event);
    void OnPaste(wxStyledTextEvent& event);

private:
    void ApplySettings();
    wxString TrimText(const wxString& text) const;
    void TrimCurrentText();

public:
    virtual ~clThemedTextCtrl();

    clThemedTextCtrl() {}
    clThemedTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    void OnSysColours(clCommandEvent& event);
    void SetText(const wxString& value);
    void SetValue(const wxString& value) override;
    void SelectAll() override;
};

#endif // CLTHEMEDTEXTCTRL_HPP
