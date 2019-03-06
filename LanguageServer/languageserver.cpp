#include "languageserver.h"
#include <wx/xrc/xmlres.h>
#include "ieditor.h"
#include <wx/stc/stc.h>
#include "event_notifier.h"
#include <macros.h>
#include "globals.h"
#include "LanguageServerConfig.h"

static LanguageServerPlugin* thePlugin = NULL;

#define CHECK_LSP_RUNNING() if(!m_server.IsRunning()) { return; }

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new LanguageServerPlugin(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran"));
    info.SetName(wxT("LanguageServerPlugin"));
    info.SetDescription(_("Support for Language Server Protocol (LSP)"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

LanguageServerPlugin::LanguageServerPlugin(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Support for Language Server Protocol (LSP)");
    m_shortName = wxT("LanguageServerPlugin");
#ifdef __WXMSW__
    m_server.Start("D:\\software\\llvm-7\\LLVM\\bin\\clangd.exe", wxEmptyString);
#else
    wxString command;
    command << "clangd-7";
    ::WrapInShell(command);
    m_server.Start(command, wxEmptyString);
#endif
    EventNotifier::Get()->Bind(wxEVT_CC_FIND_SYMBOL, &LanguageServerPlugin::OnFindSymbold, this);
    
    // Load the configuration
    LanguageServerConfig::Get().Load();
}

LanguageServerPlugin::~LanguageServerPlugin() {}

void LanguageServerPlugin::CreateToolBar(clToolBar* toolbar)
{
    // You can add items to the main toolbar here
    wxUnusedVar(toolbar);
}

void LanguageServerPlugin::CreatePluginMenu(wxMenu* pluginsMenu) {}

void LanguageServerPlugin::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_CC_FIND_SYMBOL, &LanguageServerPlugin::OnFindSymbold, this);
}

void LanguageServerPlugin::OnFindSymbold(clCodeCompletionEvent& event)
{
    event.Skip();
    CHECK_LSP_RUNNING();
    
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    CHECK_PTR_RET(editor);

    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    int col = ctrl->GetColumn(ctrl->GetCurrentPos());
    int line = ctrl->LineFromPosition(ctrl->GetCurrentPos());
    m_server.FindDefinition(editor->GetFileName(), line, col);
}
