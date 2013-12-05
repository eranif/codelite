#ifndef __Tweaks__
#define __Tweaks__

#include "plugin.h"
#include "cl_command_event.h"
#include "tweaks_settings.h"
#include <map>

class Tweaks : public IPlugin
{
    typedef std::map<wxString, int> ProjectIconMap_t;
    TweaksSettings m_settings;
    ProjectIconMap_t m_project2Icon;
    
protected:
    IEditor* FindEditorByPage( wxWindow* page );
    
public:
    Tweaks(IManager *manager);
    ~Tweaks();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();
    
    // Event handlers
    void OnSettings(wxCommandEvent &e);
    void OnColourTab(clColourEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);
    void OnFileViewBuildTree(clCommandEvent &e);
    void OnCustomizeProject(clColourEvent &e);
    void OnTabBorderColour(clColourEvent &e);
};

#endif //Tweaks
