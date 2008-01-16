#ifndef CODEFORMATTER_H
#define CODEFORMATTER_H

#include "plugin.h"

class CodeFormatter : public IPlugin
{
protected:
	void DoFormatFile(IEditor *editor);

public:
	CodeFormatter(IManager *manager);
	virtual ~CodeFormatter();
	void AstyleFormat(const wxString &input, const wxString &options, wxString &output);
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
	//event handlers
	void OnFormat(wxCommandEvent &e);
	void OnFormatOptions(wxCommandEvent &e);
	void OnFormatUI(wxUpdateUIEvent &e);
	void OnFormatOptionsUI(wxUpdateUIEvent &e);
};

#endif //CODEFORMATTER_H


