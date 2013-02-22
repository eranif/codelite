#ifndef WXCLLOGTEXTCTRL_H
#define WXCLLOGTEXTCTRL_H

#include <wx/log.h>
#include <wx/textctrl.h>

class wxclTextCtrl : public wxLog
{
public:
    wxclTextCtrl(wxTextCtrl* text);
    virtual ~wxclTextCtrl();
    void Reset() ;

    wxTextCtrl* GetPTextCtrl() {
        return m_pTextCtrl;
    }
protected:
    void DoInit();

    // implement sink function
    virtual void DoLogText(const wxString& msg) {
        m_pTextCtrl->AppendText(msg + wxS("\n"));
    }

private:
    // the control we use
    wxTextCtrl *m_pTextCtrl;
};


#endif // WXCLLOGTEXTCTRL_H
