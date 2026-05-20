//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : copyright.cpp
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
#include "copyright.h"

#include "Cxx/cpptoken.h"
#include "Cxx/cppwordscanner.h"
#include "Keyboard/clKeyboardManager.h"
#include "copyrights_options_dlg.h"
#include "copyrights_proj_sel_dlg.h"
#include "copyrightsconfigdata.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "macromanager.h"
#include "progress_dialog.h"
#include "project.h"
#include "workspace.h"

#include <algorithm>
#include <vector>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager) { return new Copyright(manager); }

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor("Eran Ifrah");
    info.SetName("Copyright");
    info.SetDescription(
        _("Copyright Plugin - a small plugin that allows you to place copyright block on top of your source files"));
    info.SetVersion("v1.0");
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

Copyright::Copyright(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Copyright Plugin - Place copyright block on top of your source files");
    m_shortName = "Copyright";

    // connect events
    wxTheApp->Bind(wxEVT_MENU, &Copyright::OnOptions, this, XRCID("CR_copyrights_options"));
    wxTheApp->Bind(wxEVT_MENU, &Copyright::OnInsertCopyrights, this, XRCID("CR_insert_copyrights"));
    wxTheApp->Bind(wxEVT_MENU, &Copyright::OnBatchInsertCopyrights, this, XRCID("CR_batch_insert_copyrights"));

    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &Copyright::OnEditorContextMenu, this);

    clKeyboardManager::Get()->AddAccelerator(_("Copyright"),
                                             {{"CR_copyrights_options", _("Settings...")},
                                              {"CR_insert_copyrights", _("Insert Copyright Block")},
                                              {"CR_batch_insert_copyrights", _("Batch Insert of Copyright Block")}});
}

void Copyright::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void Copyright::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();

    menu->Append(new wxMenuItem(
        menu, XRCID("CR_insert_copyrights"), _("Insert Copyright Block"), _("Insert Copyright Block"), wxITEM_NORMAL));
    menu->Append(new wxMenuItem(menu,
                                XRCID("CR_batch_insert_copyrights"),
                                _("Batch Insert of Copyright Block"),
                                _("Batch Insert of Copyright Block"),
                                wxITEM_NORMAL));
    menu->AppendSeparator();
    menu->Append(new wxMenuItem(menu, XRCID("CR_copyrights_options"), _("Settings..."), wxEmptyString, wxITEM_NORMAL));
    pluginsMenu->Append(wxID_ANY, _("Copyrights"), menu);
}

void Copyright::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &Copyright::OnOptions, this, XRCID("CR_copyrights_options"));
    wxTheApp->Unbind(wxEVT_MENU, &Copyright::OnInsertCopyrights, this, XRCID("CR_insert_copyrights"));
    wxTheApp->Unbind(wxEVT_MENU, &Copyright::OnBatchInsertCopyrights, this, XRCID("CR_batch_insert_copyrights"));
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &Copyright::OnEditorContextMenu, this);
}

void Copyright::OnInsertCopyrights(wxCommandEvent& e)
{
    wxUnusedVar(e);

    // read configuration
    CopyrightsConfigData data;
    m_mgr->GetConfigTool()->ReadObject("CopyrightsConfig", &data);

    // make sure that the template file exists
    if (!wxFileName::FileExists(data.GetTemplateFilename())) {
        wxMessageBox(
            wxString::Format(_("Template file name '%s', does not exist!"), data.GetTemplateFilename().GetData()),
            wxT("CodeLite"),
            wxICON_WARNING | wxOK);
        return;
    }

    // read the copyrights file
    wxString content;
    if (!ReadFileWithConversion(data.GetTemplateFilename(), content)) {
        wxMessageBox(wxString::Format(_("Failed to read template file '%s'"), data.GetTemplateFilename().c_str()),
                     wxT("CodeLite"),
                     wxICON_WARNING | wxOK);
        return;
    }

    IEditor* editor = m_mgr->GetActiveEditor();
    if (!editor) {
        wxMessageBox(wxString::Format(_("There is no active editor\n")), wxT("CodeLite"), wxICON_WARNING | wxOK);
        return;
    }

    // verify that the file consist only with comment code
    CppWordScanner scanner(data.GetTemplateFilename().mb_str().data());

    CppTokensMap l;
    scanner.FindAll(l);

    if (!l.is_empty()) {
        if (wxMessageBox(_("Template file contains text which is not comment, continue anyway?"),
                         wxT("CodeLite"),
                         wxICON_QUESTION | wxYES_NO) == wxNO) {
            return;
        }
    }

    // expand constants
    wxString _content = ExpandAllVariables(
        content, m_mgr->GetWorkspace(), editor->GetProjectName(), wxEmptyString, editor->GetFileName().GetFullPath());

    // we are good to go :)
    wxString ignoreString = data.GetIgnoreString();
    ignoreString = ignoreString.Trim().Trim(false);

    if (ignoreString.IsEmpty() == false) {
        if (editor->GetEditorText().Find(data.GetIgnoreString()) != wxNOT_FOUND) {
            clLogMessage(_("File contains ignore string, skipping it"));
            return;
        }
    }
    editor->InsertText(0, _content);
}

void Copyright::OnOptions(wxCommandEvent& e)
{
    // pop up the options dialog
    CopyrightsOptionsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr->GetConfigTool());
    dlg.ShowModal();
}

void Copyright::OnBatchInsertCopyrights(wxCommandEvent& e)
{
    // pop up the projects selection dialog
    if (m_mgr->IsWorkspaceOpen() == false) {
        wxMessageBox(_("Batch insert requires a workspace to be opened"), wxT("CodeLite"), wxICON_WARNING | wxOK);
        return;
    }

    if (!m_mgr->SaveAll()) {
        return;
    }

    // read configuration
    CopyrightsConfigData data;
    m_mgr->GetConfigTool()->ReadObject("CopyrightsConfig", &data);

    wxString content;
    if (!Validate(content)) {
        return;
    }

    CopyrightsProjectSelDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr->GetWorkspace());
    if (dlg.ShowModal() == wxID_OK) {
        wxArrayString projects;
        dlg.GetProjects(projects);

        std::vector<std::pair<wxString, std::vector<wxFileName>>> filenamesByProject;
        // loop over the project and collect list of files to work with
        for (const auto& projectName : projects) {
            ProjectPtr p = m_mgr->GetWorkspace()->GetProject(projectName);
            if (p) {
                filenamesByProject.push_back({projectName, {}});
                p->GetFilesAsVectorOfFileName(filenamesByProject.back().second);
            }
        }

        wxString mask(data.GetFileMasking());
        mask.Replace("*.", wxEmptyString);
        mask = mask.Trim().Trim(false);

        const wxArrayString exts = ::wxStringTokenize(mask, ";");
        const auto noMatchingExtension = [&](const wxFileName& filename) {
            return exts.Index(filename.GetExt(), false) == wxNOT_FOUND;
        };
        // filter out non-matching files (according to masking)
        for (auto& [_, filenames] : filenamesByProject) {
            std::erase_if(filenames, noMatchingExtension);
        }
        std::erase_if(filenamesByProject, [](const auto& p) { return p.second.empty(); });

        if (filenamesByProject.empty() == false) {
            MassUpdate(filenamesByProject, content);
        }
    }
}

void Copyright::MassUpdate(const std::vector<std::pair<wxString, std::vector<wxFileName>>>& filenamesByProject,
                           const wxString& content)
{
    const auto totalFileCount =
        std::accumulate(filenamesByProject.begin(), filenamesByProject.end(), 0u, [](unsigned acc, const auto& p) {
            return acc + p.second.size();
        });
    // last confirmation from the user
    if (wxMessageBox(wxString::Format(_("You are about to modify %u files. Continue?"), totalFileCount),
                     wxT("CodeLite"),
                     wxYES_NO | wxICON_QUESTION) == wxNO) {
        return;
    }

    clProgressDlg* prgDlg = new clProgressDlg(nullptr, _("Processing file ..."), "", static_cast<int>(totalFileCount));

    CopyrightsConfigData data;
    m_mgr->GetConfigTool()->ReadObject("CopyrightsConfig", &data);

    // now loop over the files and add copyrights block
    int progression = 0;
    for (const auto& [projectName, filenames] : filenamesByProject) {
        for (const auto& fn : filenames) {
            wxString file_content;
            wxString _content =
                ExpandAllVariables(content, m_mgr->GetWorkspace(), projectName, wxEmptyString, fn.GetFullPath());
            if (ReadFileWithConversion(fn.GetFullPath(), file_content)) {

                wxString msg;

                // if the file contains the ignore string, skip this file
                wxString ignoreString = data.GetIgnoreString();
                ignoreString = ignoreString.Trim().Trim(false);

                if (ignoreString.IsEmpty() == false && file_content.Find(data.GetIgnoreString()) != wxNOT_FOUND) {
                    msg << _("File contains ignore string, skipping it: ") << fn.GetFullName();
                    if (!prgDlg->Update(progression++, msg)) {
                        prgDlg->Destroy();
                        return;
                    }
                } else {

                    msg << _("Inserting comment to file: ") << fn.GetFullName();
                    if (!prgDlg->Update(progression++, msg)) {
                        prgDlg->Destroy();
                        return;
                    }

                    file_content.Prepend(_content);
                    if (data.GetBackupFiles() && !FileUtils::Backup(fn.GetFullPath())) {
                        continue;
                    }
                    wxCSConv fontEncConv(EditorConfigST::Get()->GetOptions()->GetFileFontEncoding());
                    FileUtils::WriteFileContent(fn.GetFullPath(), file_content, fontEncConv);

                    if (auto* editor = clGetManager()->FindEditor(fn.GetFullPath())) {
                        editor->ReloadFromDisk();
                    }
                }
            }
        }
    }
    prgDlg->Destroy();
}

bool Copyright::Validate(wxString& content)
{
    CopyrightsConfigData data;
    m_mgr->GetConfigTool()->ReadObject("CopyrightsConfig", &data);

    // make sure that the template file exists
    if (!wxFileName::FileExists(data.GetTemplateFilename())) {
        wxMessageBox(
            wxString::Format(_("Template file name '%s', does not exist!"), data.GetTemplateFilename().GetData()),
            wxT("CodeLite"),
            wxICON_WARNING | wxOK);
        return false;
    }

    // read the copyrights file
    if (!ReadFileWithConversion(data.GetTemplateFilename(), content)) {
        wxMessageBox(wxString::Format(_("Failed to read template file '%s'"), data.GetTemplateFilename().c_str()),
                     wxT("CodeLite"),
                     wxICON_WARNING | wxOK);
        return false;
    }

    // verify that the file consist only with comment code
    CppWordScanner scanner(data.GetTemplateFilename().mb_str().data());

    CppTokensMap l;
    scanner.FindAll(l);

    if (!l.is_empty()) {
        if (wxMessageBox(_("Template file contains text which is not comment, continue anyways?"),
                         wxT("CodeLite"),
                         wxICON_QUESTION | wxYES_NO) == wxNO) {
            return false;
        }
    }
    content.Replace("`", "'");
    return true;
}

void Copyright::OnEditorContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    event.GetMenu()->Append(XRCID("CR_insert_copyrights"), _("Insert Copyrights Block"));
}
