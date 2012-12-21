#include "ImportFilesDialogNew.h"
#include <wx/dir.h>
#include <wx/filefn.h>
#include "pluginmanager.h"
#include "bitmap_loader.h"
#include "importfilessettings.h"
#include "ctags_manager.h"
#include "pluginmanager.h"
#include "windowattrmanager.h"
#include "editor_config.h"

class ImportFilesDlgData : public wxClientData
{
    wxString m_path;
    bool     m_isChecked;
    bool     m_isDummy;

public:
    ImportFilesDlgData(const wxString &path, bool checked, bool isDummy = false)
        : m_path(path)
        , m_isChecked(checked)
        , m_isDummy(isDummy)
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
    void SetIsDummy(bool isDummy) {
        this->m_isDummy = isDummy;
    }
    bool IsDummy() const {
        return m_isDummy;
    }
};

//--------------------------------------------------------------

ImportFilesDialogNew::ImportFilesDialogNew(wxWindow* parent)
    : ImportFilesDialogNewBase(parent)
{
    DoBuildTree();

    ImportFilesSettings options;
    if(!EditorConfigST::Get()->ReadObject(wxT("import_dir_options"), &options)) {
        //first time, read the settings from the ctags options
        options.SetFileMask( TagsManagerST::Get()->GetCtagsOptions().GetFileSpec() );
        bool noExt = TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_PARSE_EXT_LESS_FILES ? true : false;
        size_t flags(0);
        if(noExt) {
            flags |= IFS_INCLUDE_FILES_WO_EXT;
        }
        options.SetFlags(flags);
    }

    options.SetBaseDir( PluginManager::Get()->GetSelectedTreeItemInfo(TreeFileView).m_fileName.GetPath() );
    if(options.GetBaseDir().IsEmpty()) {
        options.SetBaseDir(wxGetCwd());
    }

    m_dirPicker->SetPath(options.GetBaseDir());
    m_textCtrSpec->SetValue( options.GetFileMask() );
    m_checkBoxFilesWOExt->SetValue(options.GetFlags() & IFS_INCLUDE_FILES_WO_EXT );
    WindowAttrManager::Load(this, wxT("ImportFilesDialog"), NULL);
}

ImportFilesDialogNew::~ImportFilesDialogNew()
{
    WindowAttrManager::Save(this, wxT("ImportFilesDialog"), NULL);
    ImportFilesSettings options;
    options.SetBaseDir(m_dirPicker->GetPath());
    options.SetFileMask(m_textCtrSpec->GetValue());

    size_t flags(0);
    if(m_checkBoxFilesWOExt->IsChecked()) flags |= IFS_INCLUDE_FILES_WO_EXT;
    options.SetFlags( flags );
    EditorConfigST::Get()->WriteObject(wxT("import_dir_options"), &options);
}

void ImportFilesDialogNew::DoBuildTree(const wxDataViewItem& parent, const wxDir& dir, bool initialState)
{
    wxString path;
    bool cont = dir.GetFirst(&path, "", wxDIR_DIRS);
    while (cont ) {

        wxVector<wxVariant> cols;
        cols.push_back( MakeIconText(path, PluginManager::Get()->GetStdIcons()->LoadBitmap("mime/16/folder") ) );
        cols.push_back(initialState);

        wxDir childDir( dir.GetNameWithSep() + path );
        wxDataViewItem child = m_dataviewModel->AppendItem(parent, cols, new ImportFilesDlgData( childDir.GetName(), initialState ));

        // Add dummy columns
        if ( childDir.IsOpened() && childDir.HasSubDirs() ) {
            wxVector<wxVariant> dummyCols;
            dummyCols.push_back( MakeIconText("dummy", PluginManager::Get()->GetStdIcons()->LoadBitmap("mime/16/folder") ) );
            dummyCols.push_back( false );
            m_dataviewModel->AppendItem( child, dummyCols, new ImportFilesDlgData("", false, true) );
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
    cols.push_back( MakeIconText(m_dirPicker->GetPath(), PluginManager::Get()->GetStdIcons()->LoadBitmap("mime/16/folder") ) );
    cols.push_back(false);

    m_root = m_dataviewModel->AppendItem(wxDataViewItem(0), cols, new ImportFilesDlgData(m_dirPicker->GetPath(), false));

    // For performance, we add only the direct children of the root node
    wxDir dir(m_dirPicker->GetPath());
    if ( dir.IsOpened() && dir.HasSubDirs() ) {
        DoBuildTree(m_root, dir, false);
    }
    m_dataview->Expand(m_root);
}

void ImportFilesDialogNew::OnValueChanged(wxDataViewEvent& event)
{
    event.Skip();
    wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(event.GetItem());
    if ( cols.size() > 1 ) {
        bool isChecked = cols.at(1).GetBool();
        ImportFilesDlgData *cd = reinterpret_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(event.GetItem()));
        if ( cd ) {
            cd->SetIsChecked( isChecked );
            DoCheckChildren( event.GetItem(), isChecked );
        }
#ifdef __WXGTK__
        Refresh();
#endif
    }
}

wxVariant ImportFilesDialogNew::MakeIconText(const wxString& text, const wxBitmap& bmp) const
{
    wxIcon icn;
    icn.CopyFromBitmap( bmp);
    wxDataViewIconText ict(text, icn);
    wxVariant v;
    v << ict;
    return v;
}

void ImportFilesDialogNew::DoCheckChildren(const wxDataViewItem& parent, bool check)
{
    if ( m_dataviewModel->HasChildren( parent ) ) {
        wxDataViewItemArray children;
        m_dataviewModel->GetChildren( parent, children );
        for(size_t i=0; i<children.GetCount(); ++i) {

            // First, update the UI by replacing the columns
            wxDataViewItem item = children.Item(i);
            m_dataviewModel->SetValue(wxVariant( check ), item, 1);

            // Update the client data
            ImportFilesDlgData *cd = reinterpret_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(item));
            if ( cd ) {
                cd->SetIsChecked( check );
            }

            // Check if this child has children
            if ( m_dataviewModel->HasChildren( item ) ) {
                DoCheckChildren(item, check);
            }

        }
    }
}

void ImportFilesDialogNew::OnItemExpanding(wxDataViewEvent& event)
{
    event.Skip();
    wxDataViewItemArray children;
    if ( m_dataviewModel->GetChildren(event.GetItem(), children) ) {
        wxDataViewItem child = children.Item(0);
        ImportFilesDlgData *cd = reinterpret_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(child));
        if ( cd && cd->IsDummy() ) {
            cd = reinterpret_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(event.GetItem()));
            m_dataviewModel->DeleteItem( child );
            wxDir dir ( cd->GetPath() );
            DoBuildTree( event.GetItem(), dir, cd->IsChecked() );
        }
    }
}

void ImportFilesDialogNew::GetDirectories(wxArrayString& dirs)
{
    DoGetCheckedDirs(m_root, dirs);
}

void ImportFilesDialogNew::DoGetCheckedDirs(const wxDataViewItem& parent, wxArrayString& dirs)
{
    ImportFilesDlgData *cd = reinterpret_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(parent));
    if ( cd && cd->IsChecked() ) {
        dirs.Add(cd->GetPath());
    }

    if ( m_dataviewModel->HasChildren(parent) ) {
        wxDataViewItemArray children;
        if ( m_dataviewModel->GetChildren(parent, children)) {

            wxDataViewItem child = children.Item(0);
            cd = reinterpret_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(child));
            // dummy children?
            if ( cd && cd->IsDummy())
                return;

            for(size_t i=0; i<children.GetCount(); ++i) {
                DoGetCheckedDirs(children.Item(i), dirs);
            }
        }
    }
}

bool ImportFilesDialogNew::ExtlessFiles()
{
    return m_checkBoxFilesWOExt->IsChecked();
}

wxString ImportFilesDialogNew::GetBaseDir()
{
    return m_dirPicker->GetPath();
}

wxString ImportFilesDialogNew::GetFileMask()
{
    return m_textCtrSpec->GetValue();
}
