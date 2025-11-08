#ifndef CLTHEMEDTEXTCTRL_HPP
#define CLTHEMEDTEXTCTRL_HPP

#include "clEditorEditEventsHandler.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/stc/stc.h>

#if defined(__WXMAC__)
#define USE_TEXT_CTRL 1
#else
#define USE_TEXT_CTRL 0
#endif

#if USE_TEXT_CTRL
using clThemedTextCtrlBase = wxTextCtrl;
#else
using clThemedTextCtrlBase = wxStyledTextCtrl;
#endif

class WXDLLIMPEXP_SDK clThemedTextCtrl : public clThemedTextCtrlBase
{
public:
    virtual ~clThemedTextCtrl();

    clThemedTextCtrl() = default;
    clThemedTextCtrl(wxWindow* parent,
                     wxWindowID id,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0);
    void OnSysColours(clCommandEvent& event);
    void SetText(const wxString& value);
    void SetValue(const wxString& value) override;
    void SelectAll() override;

#if USE_TEXT_CTRL
    void SetUseVerticalScrollBar(bool) {}
    void SetWrapMode(int) {}
    wxString GetText() const { return GetValue(); }
    void ClearAll() { Clear(); }
#else
    // wxSTC does not provide "SetHint", what it does, it sets the text to the hint
    // which triggers an event (an unwanted one)
    bool SetHint(const wxString&) override { return false; }
#endif

protected:
    void OnKeyDown(wxKeyEvent& event);
    void OnAddChar(wxStyledTextEvent& event);
    void OnChange(wxStyledTextEvent& event);
    void OnPaste(wxStyledTextEvent& event);

private:
    void ApplySettings();
    wxString TrimText(const wxString& text) const;
    void TrimCurrentText();

    clEditEventsHandler::Ptr_t m_editEventsHandler;
};

#endif // CLTHEMEDTEXTCTRL_HPP
