#ifndef CODEBEAUTIFIER_H
#define CODEBEAUTIFIER_H

#include "plugin.h"

class CodeBeautifier : public IPlugin
{
protected:
	void DoFormatFile(IEditor *editor);
	wxString ExecuteUncrustify(const wxString &inputString, const wxString &ext);
	
public:
	CodeBeautifier(IManager *manager);
	virtual ~CodeBeautifier();
	
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void UnPlug();
	
	//event handlers
	void OnFormat(wxCommandEvent &e);
};

#endif //CODEBEAUTIFIER_H

