#ifndef NEWCUSTOMEVENTDLG_H
#define NEWCUSTOMEVENTDLG_H
#include "wxcrafter.h"

class NewCustomEventDlg : public NewCustomEventBaseDlg
{
public:
    NewCustomEventDlg(wxWindow* parent);
    virtual ~NewCustomEventDlg();

    wxString GetEventClass() const { return m_textCtrlEventClass->GetValue().Trim(); }

    wxString GetEventType() const { return m_textCtrlEventType->GetValue().Trim(); }
};
#endif // NEWCUSTOMEVENTDLG_H
