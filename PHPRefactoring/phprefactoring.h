#ifndef __PHPRefactoring__
#define __PHPRefactoring__

#include "phpoptions.h"
#include "phprefactoringoptions.h"
#include "plugin.h"

enum PhpEditorEventIds {
    wxID_SETTINGS = 10000,
    wxID_EXTRACT_METHOD = 10010,
    wxID_RENAME_LOCAL_VARIABLE = 10020,
    wxID_RENAME_CLASS_PROPERTY = 10030,
    wxID_CONVERT_LOCAL_TO_INSTANCE_VARIABLE = 10040,
    wxID_RENAME_CLASS_AND_NAMESPACES = 10050,
    wxID_OPTIMIZE_USE_STATEMENTS = 10060,
};

class PHPRefactoring : public IPlugin
{
    IManager* m_manager;
    PhpOptions m_settingsPhp;
    PHPRefactoringOptions m_settings;

protected:
    wxString m_selectedFolder;

private:
    void RenameVariable(const wxString& action);
    void RefactorFile(const wxString& action, const wxString& extraParameters, IEditor*& editor);
    void RunCommand(const wxString& parameters, const wxString& workingDir = wxEmptyString);
    void OnPhpSettingsChanged(clCommandEvent& event);

public:
    PHPRefactoring(IManager* manager);
    ~PHPRefactoring();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();
    void OnEditorContextMenu(clContextMenuEvent& event);
    void OnExtractMethod(wxCommandEvent& e);
    void OnRenameLocalVariable(wxCommandEvent& e);
    void OnRenameClassProperty(wxCommandEvent& e);
    void OnConvertLocalToInstanceVariable(wxCommandEvent& e);
    void OnRenameClassAndNamespaces(wxCommandEvent& e);
    void OnOptimizeUseStatements(wxCommandEvent& e);
    void OnOptimizeUseStatementsTest(wxCommandEvent& e);
    void OnMenuCommand(wxCommandEvent& e);
    void OnContextMenu(clContextMenuEvent& event);
};

#endif // PHPRefactoring
