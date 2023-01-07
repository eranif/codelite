#ifndef DEFINECUSTOMCONTROLWIZARD_H
#define DEFINECUSTOMCONTROLWIZARD_H

#include "wxc_settings.h"
#include "wxcrafter.h"

class DefineCustomControlWizard : public DefineCustomControlWizardBaseClass
{
public:
    DefineCustomControlWizard(wxWindow* parent);
    virtual ~DefineCustomControlWizard();
    CustomControlTemplate GetControl() const;

protected:
    virtual void OnDeleteEvent(wxCommandEvent& event);
    virtual void OnDeleteEventUI(wxUpdateUIEvent& event);
    virtual void OnNewEvent(wxCommandEvent& event);
    virtual void OnPageChanging(wxWizardEvent& event);
};
#endif // DEFINECUSTOMCONTROLWIZARD_H
