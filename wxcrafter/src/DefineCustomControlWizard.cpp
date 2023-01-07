#include "DefineCustomControlWizard.h"
#include "NewCustomEventDlg.h"
#include <globals.h>
#include <json_node.h>
#include <wx/msgdlg.h>

DefineCustomControlWizard::DefineCustomControlWizard(wxWindow* parent)
    : DefineCustomControlWizardBaseClass(parent)
{
}

DefineCustomControlWizard::~DefineCustomControlWizard() {}

void DefineCustomControlWizard::OnPageChanging(wxWizardEvent& event)
{
    if(event.GetDirection()) {
        if(event.GetPage() == m_wizardPageGeneral && !::IsValidCppIndetifier(m_textClassName->GetValue())) {
            wxMessageBox(_("Invalid C++ class name provided!"), wxT("wxCrafter"), wxOK | wxICON_WARNING | wxCENTER);
            event.Veto();
            return;

        } else if(event.GetPage() == m_wizardPageHeader && m_textCtrlIncludeLine->IsEmpty()) {
            wxMessageBox(_("Please set an include file for this control"), wxT("wxCrafter"),
                         wxOK | wxICON_WARNING | wxCENTER);
            event.Veto();
            return;

        } else if(event.GetPage() == m_wizardPageCpp && m_textCtrlInstantiationLine->IsEmpty()) {
            wxMessageBox(_("Control instantiation code is missing"), wxT("wxCrafter"),
                         wxOK | wxICON_WARNING | wxCENTER);
            event.Veto();
            return;
        }
    }
    event.Skip();
}

CustomControlTemplate DefineCustomControlWizard::GetControl() const
{
    CustomControlTemplate controlData;
    controlData.SetAllocationLine(m_textCtrlInstantiationLine->GetValue());
    controlData.SetClassName(m_textClassName->GetValue());
    controlData.SetIncludeFile(m_textCtrlIncludeLine->GetValue());
    controlData.SetXrcPreviewClass(m_textCtrlXRCPreviewClass->GetValue());

    wxStringMap_t events;
    int count = m_dvListCtrlEvents->GetItemCount();
    for(int i = 0; i < count; ++i) {
        wxString eventtype = m_dvListCtrlEvents->GetTextValue(i, 0);
        wxString eventclss = m_dvListCtrlEvents->GetTextValue(i, 1);

        events.insert(std::make_pair(eventtype, eventclss));
    }
    controlData.SetEvents(events);
    return controlData;
}

void DefineCustomControlWizard::OnDeleteEvent(wxCommandEvent& event)
{
    m_dvListCtrlEvents->DeleteItem(m_dvListCtrlEvents->GetSelectedRow());
}

void DefineCustomControlWizard::OnDeleteEventUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlEvents->GetSelection().IsOk());
}

void DefineCustomControlWizard::OnNewEvent(wxCommandEvent& event)
{
    NewCustomEventDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        wxVector<wxVariant> cols;
        cols.push_back(dlg.GetEventType());
        cols.push_back(dlg.GetEventClass());
        m_dvListCtrlEvents->AppendItem(cols, (wxUIntPtr)NULL);
    }
}
