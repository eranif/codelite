#include "lldbdebugger.h"
#include <wx/xrc/xmlres.h>

static LLDBDebugger* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == 0) {
        thePlugin = new LLDBDebugger(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("LLDBDebugger"));
    info.SetDescription(wxT("LLDB Debugger for CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

LLDBDebugger::LLDBDebugger(IManager *manager)
    : IPlugin(manager)
{
    m_longName = wxT("LLDB Debugger for CodeLite");
    m_shortName = wxT("LLDBDebugger");
}

LLDBDebugger::~LLDBDebugger()
{
}

clToolBar *LLDBDebugger::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
    return tb;
}

void LLDBDebugger::CreatePluginMenu(wxMenu *pluginsMenu)
{
}

void LLDBDebugger::HookPopupMenu(wxMenu *menu, MenuType type)
{
    if (type == MenuTypeEditor) {
        //TODO::Append items for the editor context menu
    } else if (type == MenuTypeFileExplorer) {
        //TODO::Append items for the file explorer context menu
    } else if (type == MenuTypeFileView_Workspace) {
        //TODO::Append items for the file view / workspace context menu
    } else if (type == MenuTypeFileView_Project) {
        //TODO::Append items for the file view/Project context menu
    } else if (type == MenuTypeFileView_Folder) {
        //TODO::Append items for the file view/Virtual folder context menu
    } else if (type == MenuTypeFileView_File) {
        //TODO::Append items for the file view/file context menu
    }
}

void LLDBDebugger::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
    if (type == MenuTypeEditor) {
        //TODO::Unhook items for the editor context menu
    } else if (type == MenuTypeFileExplorer) {
        //TODO::Unhook  items for the file explorer context menu
    } else if (type == MenuTypeFileView_Workspace) {
        //TODO::Unhook  items for the file view / workspace context menu
    } else if (type == MenuTypeFileView_Project) {
        //TODO::Unhook  items for the file view/Project context menu
    } else if (type == MenuTypeFileView_Folder) {
        //TODO::Unhook  items for the file view/Virtual folder context menu
    } else if (type == MenuTypeFileView_File) {
        //TODO::Unhook  items for the file view/file context menu
    }
}

void LLDBDebugger::UnPlug()
{
    //TODO:: perform the unplug action for this plugin
}
