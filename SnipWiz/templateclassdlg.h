//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : templateclassdlg.h
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

#ifndef __templateclassdlg__
#define __templateclassdlg__

#include "templateclassbasedlg.h"

class SnipWiz;
class swStringDb;
class IManager;

class TemplateClassDlg : public TemplateClassBaseDlg
{
    SnipWiz* m_plugin;
    bool m_modified;
    wxString m_configPath;
    int m_curEol;
    wxString m_virtualFolder; // name of current project
    wxString m_projectPath;   // path to current project
    IManager* m_pManager;

protected:
    virtual void OnVirtualDirUI(wxUpdateUIEvent& event);
    virtual void OnStcHeaderFileContentChnaged(wxStyledTextEvent& event);
    virtual void OnStcImplFileContentChnaged(wxStyledTextEvent& event);
    void OnClassNameEntered(wxCommandEvent& event);
    void OnBrowseVD(wxCommandEvent& event);
    void OnBrowseFilePath(wxCommandEvent& event);
    void OnGenerate(wxCommandEvent& event);
    void OnGenerateUI(wxUpdateUIEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnTemplateClassSelected(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonAddUI(wxUpdateUIEvent& event);
    void OnButtonChange(wxCommandEvent& event);
    void OnButtonChangeUI(wxUpdateUIEvent& event);
    void OnButtonRemove(wxCommandEvent& event);
    void OnButtonRemoveUI(wxUpdateUIEvent& event);
    void OnInsertClassKeyword(wxCommandEvent& event);
    void OnInsertClassKeywordUI(wxUpdateUIEvent& event);
    void OnButtonClear(wxCommandEvent& e);
    void OnButtonClearUI(wxUpdateUIEvent& e);

    swStringDb* GetStringDb();
    void RefreshTemplateList();
    void Initialize();

public:
    /** Constructor */
    TemplateClassDlg(wxWindow* parent, SnipWiz* plugin, IManager* manager);

    bool SaveBufferToFile(const wxString filename, const wxString buffer, int eolType = 0);

    void SetModified(const bool& modified) { this->m_modified = modified; }
    void SetPlugin(SnipWiz* plugin) { this->m_plugin = plugin; }
    const bool& GetModified() const { return m_modified; }
    SnipWiz* GetPlugin() { return m_plugin; }

    void SetCurEol(const int& curEol) { this->m_curEol = curEol; }
    void SetConfigPath(const wxString& configPath);
    void SetProjectPath(const wxString& projectPath);
    void SetVirtualFolder(const wxString& virtualFolder) { this->m_virtualFolder = virtualFolder; }
    const int& GetCurEol() const { return m_curEol; }
    const wxString& GetConfigPath() const { return m_configPath; }
    const wxString& GetProjectPath() const { return m_projectPath; }
    const wxString& GetVirtualFolder() const { return m_virtualFolder; }
};

#endif // __templateclassdlg__
