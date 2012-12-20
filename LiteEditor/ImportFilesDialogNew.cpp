#include "ImportFilesDialogNew.h"
#include <wx/dir.h>
#include <wx/filefn.h>

class ImportFilesDlgData : public wxClientData
{
    wxString m_path;
    bool     m_isChecked;

public:
    ImportFilesDlgData(const wxString &path, bool checked)
        : m_path(path)
        , m_isChecked(checked)
    {}

    virtual ~ImportFilesDlgData()
    {}

    void SetIsChecked(bool isChecked) {
        this->m_isChecked = isChecked;
    }
    void SetPath(const wxString& path) {
        this->m_path = path;
    }
    bool IsChecked() const {
        return m_isChecked;
    }
    const wxString& GetPath() const {
        return m_path;
    }
};

//--------------------------------------------------------------

ImportFilesDialogNew::ImportFilesDialogNew(wxWindow* parent)
    : ImportFilesDialogNewBase(parent)
{
   DoBuildTree();
}

ImportFilesDialogNew::~ImportFilesDialogNew()
{
}

void ImportFilesDialogNew::DoBuildTree(const wxDataViewItem& parent, const wxDir& dir)
{
    wxString path;
    bool cont = dir.GetFirst(&path, "", wxDIR_DIRS);
    while (cont ) {
        wxVector<wxVariant> cols;
        cols.push_back( path );
        cols.push_back(false);
        wxDataViewItem child = m_dataviewModel->AppendItem(parent, cols, new ImportFilesDlgData( path, false ));
        wxDir childDir( dir.GetNameWithSep() + path );
        if ( childDir.IsOpened() && childDir.HasSubDirs() ) {
            DoBuildTree(child, childDir);
        }
        cont = dir.GetNext(&path);
    }
}

void ImportFilesDialogNew::OnDirChanged(wxFileDirPickerEvent& event)
{
    wxUnusedVar(event);
    DoBuildTree();
}

void ImportFilesDialogNew::DoBuildTree()
{
    m_dataviewModel->Clear();
    
    wxString curpath = m_dirPicker->GetPath();
    if ( !wxDir::Exists(curpath) ) {
        m_dirPicker->SetPath( ::wxGetCwd() );
    }
    
    wxVector<wxVariant> cols;
    cols.push_back( m_dirPicker->GetPath() );
    cols.push_back(true);
    
    m_root = m_dataviewModel->AppendItem(wxDataViewItem(0), cols, new ImportFilesDlgData(m_dirPicker->GetPath(), true));
    wxDir dir(m_dirPicker->GetPath());
    if ( dir.IsOpened() && dir.HasSubDirs() ) {
        DoBuildTree(m_root, dir);
    }
}

