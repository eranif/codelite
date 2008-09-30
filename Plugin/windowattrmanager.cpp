#include "editor_config.h"
#include "windowattrmanager.h"

void WindowAttrManager::Load(wxWindow* win, const wxString& name, IConfigTool* conf)
{
	if(conf == NULL) {
		conf = EditorConfigST::Get();
	}
	
	SimpleRectValue val;
	if(conf->ReadObject(name, &val)){
		win->Move(val.GetRect().GetTopLeft());
		win->SetSize(val.GetRect().GetSize());
	}
	
}

void WindowAttrManager::Save(wxWindow* win, const wxString& name, IConfigTool* conf)
{
	if(conf == NULL) {
		conf = EditorConfigST::Get();
	}
	
	SimpleRectValue val;
	val.SetRect(wxRect(win->GetPosition(), win->GetSize()));
	
	conf->WriteObject(name, &val);
}

