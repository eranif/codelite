#ifndef FLOATINGTEXTCTRL_H
#define FLOATINGTEXTCTRL_H
#include "sftp_ui.h"

class FloatingTextCtrl : public FloatingTextCtrlBase
{
public:
    FloatingTextCtrl(wxWindow* parent);
    virtual ~FloatingTextCtrl();

    virtual bool Show( bool show = true );

    wxString GetValue() const {
        return m_textCtrlInput->GetValue();
    }
protected:
    virtual void OnFocusLost(wxFocusEvent& event);
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnTextUpdated(wxCommandEvent& event);
};
#endif // FLOATINGTEXTCTRL_H
