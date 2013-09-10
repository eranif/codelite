#ifndef PLUGINWIZARD_H
#define PLUGINWIZARD_H

#include "gizmos_base.h"
#include "newplugindata.h"

class PluginWizard : public PluginWizardBase
{
public:
    PluginWizard(wxWindow* parent);
    virtual ~PluginWizard();

    bool Run(NewPluginData& pd);
protected:
    virtual void OnProjectPathChanged(wxFileDirPickerEvent& event);
    virtual void OnFinish(wxWizardEvent& event);
    virtual void OnPageChanging(wxWizardEvent& event);
};
#endif // PLUGINWIZARD_H
