#include "codelitediff.h"
#include <wx/xrc/xmlres.h>
#include "DiffSideBySidePanel.h"

static CodeLiteDiff* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == 0) {
        thePlugin = new CodeLiteDiff(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("CodeLiteDiff"));
    info.SetDescription(wxT("CodeLite Diff Plugin"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

CodeLiteDiff::CodeLiteDiff(IManager *manager)
    : IPlugin(manager)
{
    m_longName = wxT("CodeLite Diff Plugin");
    m_shortName = wxT("CodeLiteDiff");
}

CodeLiteDiff::~CodeLiteDiff()
{
}

clToolBar *CodeLiteDiff::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);

/*
    // You can use the below code a snippet:
    // First, check that CodeLite allows plugin to register plugins
    if (m_mgr->AllowToolbar()) {
        // Support both toolbars icon size
        int size = m_mgr->GetToolbarIconSize();

        // Allocate new toolbar, which will be freed later by CodeLite
        tb = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);

        // Set the toolbar size
        tb->SetToolBitmapSize(wxSize(size, size));

        // Add tools to the plugins toolbar. You must provide 2 sets of icons: 24x24 and 16x16
        if (size == 24) {
            tb->AddTool(XRCID("new_plugin"), wxT("New CodeLite Plugin Project"), wxXmlResource::Get()->LoadBitmap(wxT("plugin24")), wxT("New Plugin Wizard..."));
            tb->AddTool(XRCID("new_class"), wxT("Create New Class"), wxXmlResource::Get()->LoadBitmap(wxT("class24")), wxT("New Class..."));
            tb->AddTool(XRCID("new_wx_project"), wxT("New wxWidget Project"), wxXmlResource::Get()->LoadBitmap(wxT("new_wx_project24")), wxT("New wxWidget Project"));
        } else {
            tb->AddTool(XRCID("new_plugin"), wxT("New CodeLite Plugin Project"), wxXmlResource::Get()->LoadBitmap(wxT("plugin16")), wxT("New Plugin Wizard..."));
            tb->AddTool(XRCID("new_class"), wxT("Create New Class"), wxXmlResource::Get()->LoadBitmap(wxT("class16")), wxT("New Class..."));
            tb->AddTool(XRCID("new_wx_project"), wxT("New wxWidget Project"), wxXmlResource::Get()->LoadBitmap(wxT("new_wx_project16")), wxT("New wxWidget Project"));
        }
        // And finally, we must call 'Realize()'
        tb->Realize();
    }
*/
    // return the toolbar, it can be NULL if CodeLite does not allow plugins to register toolbars
    // or in case the plugin simply does not require toolbar
    return tb;
}

void CodeLiteDiff::CreatePluginMenu(wxMenu *pluginsMenu)
{
    wxMenu *menu = new wxMenu;
    menu->Append(ID_TOOL_NEW_DIFF, _("New Diff.."), _("Start new diff"));
    menu->Bind(wxEVT_COMMAND_MENU_SELECTED, &CodeLiteDiff::OnNewDiff, this, ID_TOOL_NEW_DIFF);
    pluginsMenu->Append(wxID_ANY, _("Diff Tool"), menu);
}

void CodeLiteDiff::HookPopupMenu(wxMenu *menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void CodeLiteDiff::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void CodeLiteDiff::UnPlug()
{
}

void CodeLiteDiff::OnNewDiff(wxCommandEvent& e)
{
    DiffSideBySidePanel* diff = new DiffSideBySidePanel(m_mgr->GetEditorPaneNotebook(), clDTL::kTwoPanes);
    diff->DiffNew(); // Indicate that we want a clean diff, not from a source control
    m_mgr->AddPage(diff, _("Diff"), wxNullBitmap, true);
}
