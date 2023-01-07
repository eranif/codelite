#ifndef __multi_string_ctrl__
#define __multi_string_ctrl__

#include "gui.h"

extern const wxEventType wxEVT_MUTLI_STRING_UPDATE;

/** Implementing MultiStringCtrl */
class MultiStringCtrl : public wxTextCtrl
{
    wxString m_delim;
    wxString m_msg;

protected:
    virtual void OnTextEnter(wxCommandEvent& event);
    virtual void OnMouseLeft(wxMouseEvent& event);

    void DoEdit();
    void DoNotify();

public:
    /** Constructor */
    MultiStringCtrl(wxWindow* parent, const wxString& value, const wxString& delim = wxT(";"),
                    const wxString& msg = wxT(""));
    virtual ~MultiStringCtrl();
    void SetTip(const wxString& tooltip) { SetToolTip(tooltip); }
};

#endif // __multi_string_ctrl__
