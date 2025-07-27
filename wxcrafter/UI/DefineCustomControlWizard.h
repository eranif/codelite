#ifndef DEFINECUSTOMCONTROLWIZARD_H
#define DEFINECUSTOMCONTROLWIZARD_H

#include "wxc_settings.h"
#include "wxcrafter.h"

class DefineCustomControlWizard : public DefineCustomControlWizardBaseClass
{
public:
    DefineCustomControlWizard(wxWindow* parent);
    ~DefineCustomControlWizard() override = default;
    CustomControlTemplate GetControl() const;

protected:
    void OnDeleteEvent(wxCommandEvent& event) override;
    void OnDeleteEventUI(wxUpdateUIEvent& event) override;
    void OnNewEvent(wxCommandEvent& event) override;
    void OnPageChanging(wxWizardEvent& event) override;
};
#endif // DEFINECUSTOMCONTROLWIZARD_H
