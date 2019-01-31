#ifndef IMPORTDLG_H
#define IMPORTDLG_H
#include "wxcrafter.h"

class ImportDlg : public ImportDlgBaseClass
{
public:
    struct ImportFileData {
        wxFileName wxcpFilename;
        wxString virtualFolder;
        bool addToProject;
        bool loadWhenDone;

        ImportFileData()
            : addToProject(false)
            , loadWhenDone(false)
        {
        }
    };

public:
    enum IPD_ProjectType { IPD_FB, IPD_Smith, IPD_XRC };

protected:
    IPD_ProjectType m_Type;
    bool m_modified;

protected:
    virtual void OnDestFilePathUpdated(wxCommandEvent& event);
    virtual void OnFileImportTextUpdated(wxCommandEvent& event);
    virtual void OnAddToProjectUI(wxUpdateUIEvent& event);
    virtual void OnAddFileToProjectUI(wxUpdateUIEvent& event);
    virtual void OnBrowseForVirtualFolder(wxCommandEvent& event);

public:
    ImportDlg(IPD_ProjectType type, wxWindow* parent = NULL, const wxString& sourceFile = "");
    virtual ~ImportDlg();

    wxString GetFilepath() const { return m_filepathText->GetValue(); }
    const wxString GetOutputFilepath() const { return m_textName->GetValue(); }

    ImportFileData GetData() const;

protected:
    virtual void OnBrowseForOutputFolder(wxCommandEvent& event);
    virtual void OnOKButtonUpdateUI(wxUpdateUIEvent& event);
    virtual void OnBrowse(wxCommandEvent& event);
};
#endif // IMPORTDLG_H
