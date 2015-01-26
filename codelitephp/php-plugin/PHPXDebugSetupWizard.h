#ifndef PHPXDEBUGSETUPWIZARD_H
#define PHPXDEBUGSETUPWIZARD_H
#include "php_ui.h"

class PHPXDebugSetupWizard : public PHPXDebugSetupWizardBase
{
public:
    PHPXDebugSetupWizard(wxWindow* parent);
    virtual ~PHPXDebugSetupWizard();
protected:
    virtual void OnFinished(wxWizardEvent& event);
    virtual void OnPageChanging(wxWizardEvent& event);
    void SelectAllIniText();
};
#endif // PHPXDEBUGSETUPWIZARD_H
