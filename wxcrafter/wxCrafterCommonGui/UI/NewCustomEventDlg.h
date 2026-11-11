#pragma once
#include "wxcrafter.hpp"

class NewCustomEventDlg : public NewCustomEventBaseDlg
{
public:
    NewCustomEventDlg(wxWindow* parent);
    ~NewCustomEventDlg() override = default;

    wxString GetEventClass() const { return m_textCtrlEventClass->GetValue().Trim(); }

    wxString GetEventType() const { return m_textCtrlEventType->GetValue().Trim(); }
};
