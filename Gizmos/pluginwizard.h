#ifndef PLUGINWIZARD_H
#define PLUGINWIZARD_H

#include "wx/wizard.h"
#include "plugindata.h"

class PluginWizard : public wxWizard
{
	wxWizardPageSimple *m_page1;
	wxWizardPageSimple *m_page2;
public:
	PluginWizard(wxWindow *parent, wxWindowID id);
	~PluginWizard();
	bool Run(PluginData &data);
};

#endif //PLUGINWIZARD_H
