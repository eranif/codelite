//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : wxformbuilder.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "asyncprocess.h"
#include "confformbuilder.h"
#include "event_notifier.h"
#include "formbuildsettingsdlg.h"
#include "globals.h"
#include "processreaderthread.h"
#include "procutils.h"
#include "project.h"
#include "workspace.h"
#include "wxfbitemdlg.h"
#include "wxformbuilder.h"
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/mimetype.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>

static wxFormBuilder* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new wxFormBuilder(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("wxFormBuilder"));
    info.SetDescription(_("wxFormBuilder integration with CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

wxFormBuilder::wxFormBuilder(IManager* manager)
    : IPlugin(manager)
    , m_separatorItem(NULL)
    , m_openWithWxFbItem(NULL)
    , m_openWithWxFbSepItem(NULL)
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxFormBuilder::OnWxFBTerminated, this);
    m_longName = _("wxFormBuilder integration with CodeLite");
    m_shortName = wxT("wxFormBuilder");
    m_topWin = m_mgr->GetTheApp();

    m_topWin->Connect(XRCID("wxfb_new_dialog"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxFormBuilder::OnNewDialog), NULL, this);
    m_topWin->Connect(XRCID("wxfb_new_dialog_with_buttons"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxFormBuilder::OnNewDialogWithButtons), NULL, this);
    m_topWin->Connect(XRCID("wxfb_new_frame"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxFormBuilder::OnNewFrame), NULL, this);
    m_topWin->Connect(XRCID("wxfb_new_panel"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxFormBuilder::OnNewPanel), NULL, this);
    m_topWin->Connect(XRCID("wxfb_open"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxFormBuilder::OpenWithWxFb), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEventHandler(wxFormBuilder::OnOpenFile),
                                  NULL, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FILE, &wxFormBuilder::OnShowFileContextMenu, this);
}

wxFormBuilder::~wxFormBuilder() {}

void wxFormBuilder::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void wxFormBuilder::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("wxfb_settings"), _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, _("wxFormBuilder"), menu);

    m_topWin->Connect(XRCID("wxfb_settings"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxFormBuilder::OnSettings), NULL, this);
}

void wxFormBuilder::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeFileView_Folder) {
        menu->AppendSeparator();
        menu->Append(XRCID("WXFB_POPUP"), _("wxFormBuilder"), CreatePopupMenu());
    }
}

void wxFormBuilder::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FILE, &wxFormBuilder::OnShowFileContextMenu, this);
}

void wxFormBuilder::OnSettings(wxCommandEvent& e)
{
    FormBuildSettingsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.ShowModal();
}

wxMenu* wxFormBuilder::CreatePopupMenu()
{
    // Create the popup menu for the file explorer
    // The only menu that we are interseted is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("wxfb_new_dialog"), _("New wxDialog..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("wxfb_new_dialog_with_buttons"), _("New wxDialog with Default Buttons..."),
                          wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("wxfb_new_frame"), _("New wxFrame..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("wxfb_new_panel"), _("New wxPanel..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

void wxFormBuilder::OnNewDialog(wxCommandEvent& e)
{
    wxFBItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.SetTitle(_("New wxDialog"));
    if(dlg.ShowModal() == wxID_OK) {
        wxFBItemInfo info;
        info = dlg.GetData();
        info.kind = wxFBItemKind_Dialog;

        DoCreateWxFormBuilderProject(info);
    }
}

void wxFormBuilder::OnNewFrame(wxCommandEvent& e)
{
    wxFBItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.SetTitle(_("New wxFrame"));
    if(dlg.ShowModal() == wxID_OK) {
        wxFBItemInfo info;
        info = dlg.GetData();
        info.kind = wxFBItemKind_Frame;
        DoCreateWxFormBuilderProject(info);
    }
}

void wxFormBuilder::OnNewPanel(wxCommandEvent& e)
{
    wxFBItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.SetTitle(_("New wxPanel"));

    // no need for title in wxPanel
    dlg.DisableTitleField();

    if(dlg.ShowModal() == wxID_OK) {
        wxFBItemInfo info;
        info = dlg.GetData();
        info.kind = wxFBItemKind_Panel;
        DoCreateWxFormBuilderProject(info);
    }
}

void wxFormBuilder::DoCreateWxFormBuilderProject(const wxFBItemInfo& data)
{
    // add new virtual folder to the selected virtual directory
    wxString formbuilderVD;
    formbuilderVD = data.virtualFolder.BeforeFirst(wxT(':'));

    m_mgr->CreateVirtualDirectory(formbuilderVD, wxT("formbuilder"));
    wxString templateFile(m_mgr->GetInstallDirectory() + wxT("/templates/formbuilder/"));

    switch(data.kind) {
    default:
    case wxFBItemKind_Dialog:
        templateFile << wxT("DialogTemplate.fbp");
        break;
    case wxFBItemKind_Frame:
        templateFile << wxT("FrameTemplate.fbp");
        break;
    case wxFBItemKind_Panel:
        templateFile << wxT("PanelTemplate.fbp");
        break;
    case wxFBItemKind_Dialog_With_Buttons:
        templateFile << wxT("DialogTemplateWithButtons.fbp");
        break;
    }

    wxFileName tmplFile(templateFile);
    if(!tmplFile.FileExists()) {
        wxMessageBox(wxString::Format(_("Can't find wxFormBuilder template file '%s'"), tmplFile.GetFullPath().c_str()),
                     _("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
        return;
    }

    // place the files under the VD's project owner
    wxString err_msg;
    wxString project = data.virtualFolder.BeforeFirst(wxT(':'));
    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project, err_msg);
    if(proj) {
        wxString files_path = proj->GetFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
        // copy the file to here
        wxFileName fbpFile(files_path, data.file + wxT(".fbp"));
        if(!wxCopyFile(tmplFile.GetFullPath(), fbpFile.GetFullPath())) {
            wxMessageBox(wxString::Format(_("Failed to copy template file to '%s'"), fbpFile.GetFullPath().c_str()),
                         _("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
            return;
        }
        // open the file, and replace expand its macros
        wxString content;
        if(!ReadFileWithConversion(fbpFile.GetFullPath().c_str(), content)) {
            wxMessageBox(wxString::Format(_("Failed to read file '%s'"), fbpFile.GetFullPath().c_str()), _("CodeLite"),
                         wxOK | wxCENTER | wxICON_WARNING);
            return;
        }

        content.Replace(wxT("$(BaseFileName)"), data.file);
        content.Replace(wxT("$(ProjectName)"), data.className);
        content.Replace(wxT("$(Title)"), data.title);
        content.Replace(wxT("$(ClassName)"), data.className);

        if(!WriteFileWithBackup(fbpFile.GetFullPath().c_str(), content, false)) {
            wxMessageBox(wxString::Format(_("Failed to write file '%s'"), fbpFile.GetFullPath().c_str()), _("CodeLite"),
                         wxOK | wxCENTER | wxICON_WARNING);
            return;
        }

        // add the file to the project
        wxArrayString paths;
        paths.Add(fbpFile.GetFullPath());
        m_mgr->AddFilesToVirtualFolder(project + wxT(":formbuilder"), paths);

        // // first we launch wxFB with the -g flag set
        wxString genFileCmd;
        genFileCmd << GetWxFBPath() << wxT(" -g ") << fbpFile.GetFullPath();

        wxArrayString dummy, filesToAdd;
        ProcUtils::SafeExecuteCommand(genFileCmd, dummy);

        wxFileName cppFile(fbpFile.GetPath(), data.file + wxT(".cpp"));
        wxFileName headerFile(fbpFile.GetPath(), data.file + wxT(".h"));

        if(cppFile.FileExists()) {
            filesToAdd.Add(cppFile.GetFullPath());
        }

        if(headerFile.FileExists()) {
            filesToAdd.Add(headerFile.GetFullPath());
        }

        if(filesToAdd.GetCount()) {
            m_mgr->AddFilesToVirtualFolder(data.virtualFolder, filesToAdd);
        }

        DoLaunchWxFB(fbpFile.GetFullPath());
    }
}

void wxFormBuilder::OpenWithWxFb(wxCommandEvent& e)
{
    // get the file name
    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeFile) {
        if(item.m_fileName.GetExt() == wxT("fbp")) {
            DoLaunchWxFB(item.m_fileName.GetFullPath());
        } else {
            wxMessageBox(_("Please select a 'fbp' (Form Builder Project) file only"), _("CodeLite"),
                         wxOK | wxCENTER | wxICON_INFORMATION);
            return;
        }
    }
}

void wxFormBuilder::DoLaunchWxFB(const wxString& file)
{
    wxString fbpath = GetWxFBPath();
    //	if (fbpath.IsEmpty()) {
    //		wxMessageBox(_("Failed to launch wxFormBuilder, no path specified\nPlease set wxFormBuilder path from
    // Plugins
    //-> wxFormBuilder -> Settings..."),
    //		             _("CodeLite"), wxOK|wxCENTER|wxICON_WARNING);
    //		return;
    //	}
    ConfFormBuilder confData;
    m_mgr->GetConfigTool()->ReadObject(wxT("wxFormBuilder"), &confData);
    wxString cmd = confData.GetCommand();
    cmd.Replace(wxT("$(wxfb)"), fbpath);
    cmd.Replace(wxT("$(wxfb_project)"), wxString::Format(wxT("\"%s\""), file.c_str()));
    CreateAsyncProcess(this, cmd, IProcessCreateWithHiddenConsole | IProcessWrapInShell);
}

wxString wxFormBuilder::GetWxFBPath()
{
    // Launch wxFB
    ConfFormBuilder confData;
    m_mgr->GetConfigTool()->ReadObject(wxT("wxFormBuilder"), &confData);
    wxString fbpath = confData.GetFbPath();

#ifdef __WXGTK__
    if(fbpath.IsEmpty()) {
        // try to locate the file at '/usr/bin' or '/usr/local/bin'
        if(wxFileName::FileExists(wxT("/usr/local/bin/wxformbuilder"))) {
            fbpath = wxT("/usr/local/bin/wxformbuilder");
        } else if(wxFileName::FileExists(wxT("/usr/bin/wxformbuilder"))) {
            fbpath = wxT("/usr/bin/wxformbuilder");
        }
    }
#endif
    if(fbpath.IsEmpty()) {
        fbpath = wxT("wxformbuilder");
    }
    return fbpath;
}

void wxFormBuilder::OnNewDialogWithButtons(wxCommandEvent& e)
{
    wxFBItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.SetTitle(_("New wxDialog with Default Buttons"));
    if(dlg.ShowModal() == wxID_OK) {
        wxFBItemInfo info;
        info = dlg.GetData();
        info.kind = wxFBItemKind_Dialog_With_Buttons;

        DoCreateWxFormBuilderProject(info);
    }
}

void wxFormBuilder::OnOpenFile(clCommandEvent& e)
{
    e.Skip();
    // launch it with the default application
    wxFileName fullpath(e.GetFileName());
    if(fullpath.GetExt().MakeLower() != wxT("fbp")) {
        return;
    }

#ifdef __WXGTK__
    e.Skip(false);
    // Under Linux, use xdg-open
    wxString cmd;
    cmd << wxT("/bin/sh -c 'xdg-open \"") << fullpath.GetFullPath() << wxT("\"' 2> /dev/null");
    wxExecute(cmd);
    return;
#else
    wxMimeTypesManager* mgr = wxTheMimeTypesManager;
    wxFileType* type = mgr->GetFileTypeFromExtension(fullpath.GetExt());
    if(type) {
        wxString cmd = type->GetOpenCommand(fullpath.GetFullPath());
        wxDELETE(type);
        if(cmd.IsEmpty() == false) {
            e.Skip(false);
            wxExecute(cmd);
        }
    }
#endif
}

void wxFormBuilder::OnWxFBTerminated(clProcessEvent& e)
{
    if(e.GetProcess()) {
        delete e.GetProcess();
    }
}

void wxFormBuilder::OnShowFileContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    wxFileName file(event.GetFileName());
    if(file.GetExt() == "fbp") {
        wxMenu* menu = event.GetMenu();
        menu->PrependSeparator();
        menu->Prepend(
            new wxMenuItem(menu, XRCID("wxfb_open"), _("Open with wxFormBuilder..."), wxEmptyString, wxITEM_NORMAL));
    }
}
