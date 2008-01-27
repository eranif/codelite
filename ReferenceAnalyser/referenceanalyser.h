#ifndef __ReferenceAnalyser__
#define __ReferenceAnalyser__

#include <map>
#include <vector>

#include "plugin.h"
#include "analyseroptions.h"
#include "Graph.h"

typedef Graph<wxString> wxStringGraph;

class ReferenceAnalyser : public IPlugin
{	
	wxTextCtrl* m_analyserWindow;
	AnalyserOptions m_options;
	wxStringGraph m_classMembersGraph;
	
public:
	ReferenceAnalyser(IManager *manager);
	~ReferenceAnalyser();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
	//event handlers
	void OnAnalyse(wxCommandEvent &e);
	void OnAnalyserOptions(wxCommandEvent &e);
	void OnAnalyseUI(wxUpdateUIEvent &e);
	void OnAnalyserOptionsUI(wxUpdateUIEvent &e);
	
private:
	void Analyse(const wxString& classname);
	void Print(const wxString& classname = wxEmptyString);
	void LogMessage(const wxString& message);
};

#endif //ReferenceAnalyser

