#include "NewProjectWizard.h"
#include "globals.h"
#include "editor_config.h"
#include "pluginmanager.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "workspace.h"
#include "debuggermanager.h"
#include <wx/filename.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>

class NewProjectDlgData : public SerializedObject
{
    size_t   m_flags;
    int      m_category;
    wxString m_lastSelection;

public:
    enum {
        NpSeparateDirectory = 0x00000001
    };

public:
    NewProjectDlgData()
        : m_flags(NpSeparateDirectory)
        , m_category(-1) {
    }

    virtual ~NewProjectDlgData() {
    }

    void DeSerialize(Archive& arch) {
        arch.Read(wxT("m_flags"), m_flags);
        arch.Read(wxT("m_category"), m_category);
        arch.Read(wxT("m_lastSelection"), m_lastSelection);
    }

    void Serialize(Archive& arch) {
        arch.Write(wxT("m_flags"), m_flags);
        arch.Write(wxT("m_category"), m_category);
        arch.Write(wxT("m_lastSelection"), m_lastSelection);
    }


    void SetFlags(size_t flags) {
        this->m_flags = flags;
    }
    size_t GetFlags() const {
        return m_flags;
    }
    void SetCategory(int category) {
        this->m_category = category;
    }
    int GetCategory() const {
        return m_category;
    }
    void SetLastSelection(const wxString& lastSelection) {
        this->m_lastSelection = lastSelection;
    }
    const wxString& GetLastSelection() const {
        return m_lastSelection;
    }
};

// ------------------------------------------------------------
// ------------------------------------------------------------
class NewProjectClientData : public wxClientData
{
    ProjectPtr m_project;

public:
    NewProjectClientData(ProjectPtr project) {
        m_project = project;
    }
    virtual ~NewProjectClientData() {
        m_project = NULL;
    }
    void setProject(const ProjectPtr& project) {
        this->m_project = project;
    }
    const ProjectPtr& getProject() const {
        return m_project;
    }
};

// ------------------------------------------------------------
// ------------------------------------------------------------

NewProjectWizard::NewProjectWizard(wxWindow* parent)
    : NewProjectWizardBase(parent)
{
    NewProjectDlgData info;
    EditorConfigST::Get()->ReadObject(wxT("NewProjectDlgData"), &info);

    NewProjImgList images;
    // Get list of project templates
    wxImageList *lstImages (NULL);
    GetProjectTemplateList(PluginManager::Get(), m_list, &m_mapImages, &lstImages);
    wxDELETE(lstImages);

    // Populate the dataview model
    m_dataviewTemplatesModel->Clear();
    std::list<ProjectPtr>::iterator iter = m_list.begin();
    wxVector<wxVariant> cols;
    std::map<wxString, wxDataViewItem> categoryMap;

    wxDataViewItem selection;
    for (; iter != m_list.end(); ++iter) {
        wxString internalType = (*iter)->GetProjectInternalType();
        if (internalType.IsEmpty()) internalType = _("Others");

        if ( categoryMap.count( internalType ) == 0 ) {
            cols.clear();
            wxIcon icn;
            icn.CopyFromBitmap( images.Bitmap("gear16") );
            wxDataViewIconText ict(internalType, icn);
            wxVariant v;
            v << ict;
            cols.push_back( v );
            categoryMap[internalType] = m_dataviewTemplatesModel->AppendItem(wxDataViewItem(0), cols);
        }

        wxString imgId = (*iter)->GetProjectIconName();

        cols.clear();
        wxIcon icn;
        icn.CopyFromBitmap( images.Bitmap(imgId) );
        wxDataViewIconText ict((*iter)->GetName(), icn);
        wxVariant v;
        v << ict;

        cols.push_back( v );
        wxDataViewItem item = m_dataviewTemplatesModel->AppendItem(categoryMap[internalType], cols, new NewProjectClientData((*iter)));
        if ( (*iter)->GetName() == info.GetLastSelection() ) {
            selection = item;
        }
    }

    // Append list of compilers
    wxArrayString choices;

    // Aet list of compilers from configuration file
    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while (cmp) {
        choices.Add(cmp->GetName());
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }

    m_choiceCompiler->Append( choices );
    if (choices.IsEmpty() == false) {
        m_choiceCompiler->SetSelection(0);
    }

    m_textCtrlProjectPath->SetValue( WorkspaceST::Get()->GetWorkspaceFileName().GetPath());

    // Get list of debuggers
    m_choiceDebugger->Append( DebuggerMgr::Get().GetAvailableDebuggers() );
    if ( !m_choiceDebugger->IsEmpty() ) {
        m_choiceDebugger->SetSelection( 0 );
    }

    m_cbSeparateDir->SetValue( info.GetFlags() & NewProjectDlgData::NpSeparateDirectory );

    // Make sure the old selection is restored
    if ( selection.IsOk() ) {
        m_dataviewTemplates->Select(selection);
        m_dataviewTemplates->EnsureVisible(selection);
        NewProjectClientData* cd = dynamic_cast<NewProjectClientData*>( m_dataviewTemplatesModel->GetClientObject(selection) );
        if ( cd ) {
            m_projectData.m_srcProject = cd->getProject();
        }
    }

    UpdateProjectPage();
}

NewProjectWizard::~NewProjectWizard()
{
    // Keep the options
    NewProjectDlgData info;
    size_t            flags (0);

    if(m_cbSeparateDir->IsChecked())
        flags |= NewProjectDlgData::NpSeparateDirectory;

    info.SetFlags(flags);
    wxDataViewItem sel = m_dataviewTemplates->GetSelection();
    if ( sel.IsOk() ) {
        NewProjectClientData *cd =  dynamic_cast<NewProjectClientData*>(m_dataviewTemplatesModel->GetClientObject(sel));
        if ( cd ) {
            info.SetLastSelection( cd->getProject()->GetName() ) ;
        }
    }

    EditorConfigST::Get()->WriteObject(wxT("NewProjectDlgData"), &info);
}

void NewProjectWizard::UpdateFullFileName()
{
    wxString projectPath;
    projectPath << m_textCtrlProjectPath->GetValue();

    projectPath = projectPath.Trim().Trim(false);

    wxString tmpSep( wxFileName::GetPathSeparator() );
    if ( !projectPath.EndsWith(tmpSep) && projectPath.IsEmpty() == false ) {
        projectPath << wxFileName::GetPathSeparator();
    }

    if ( m_txtProjName->GetValue().Trim().Trim(false).IsEmpty() ) {
        m_stxtFullFileName->SetLabel(wxEmptyString);
        return;
    }

    if ( m_cbSeparateDir->IsChecked()) {
        //append the workspace name
        projectPath << m_txtProjName->GetValue();
        projectPath << wxFileName::GetPathSeparator();
    }

    projectPath << m_txtProjName->GetValue();
    projectPath << wxT(".project");

    m_stxtFullFileName->SetLabel( projectPath );
}

void NewProjectWizard::UpdateProjectPage()
{
    //update the description
    if ( m_projectData.m_srcProject ) {
        wxString desc = m_projectData.m_srcProject->GetDescription();
        desc = desc.Trim().Trim(false);
        desc.Replace(wxT("\t"), wxT(" "));
        // m_txtDescription->SetValue( desc );

        // select the correct compiler
        ProjectSettingsPtr settings  = m_projectData.m_srcProject->GetSettings();
        if (settings) {
            ProjectSettingsCookie ck;
            BuildConfigPtr buildConf = settings->GetFirstBuildConfiguration(ck);
            if (buildConf) {
                m_choiceCompiler->SetStringSelection( buildConf->GetCompilerType() );
                m_choiceCompiler->SetStringSelection( buildConf->GetDebuggerType() );
            }
        }
    }
}

void NewProjectWizard::OnBrowseProjectPath(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString path(m_textCtrlProjectPath->GetValue());
    wxString new_path = ::wxDirSelector(_("Select Project Path:"), path, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if (new_path.IsEmpty() == false) {

        static wxString INVALID_CHARS = " ,'()";
        if ( new_path.find_first_of(INVALID_CHARS) != wxString::npos ) {
            int answer = ::wxMessageBox(wxString() << _("The selected project path '") << new_path << _("'\nContains some invalid characters\nContinue anyways?"), "CodeLite", wxYES_NO|wxCANCEL|wxICON_WARNING, this);
            if ( answer != wxYES ) {
                return;
            }
        }
        m_textCtrlProjectPath->SetValue(new_path);
    }
}

void NewProjectWizard::OnItemSelected(wxDataViewEvent& event)
{
    NewProjectClientData* cd = dynamic_cast<NewProjectClientData*>(m_dataviewTemplatesModel->GetClientObject(event.GetItem()));
    if ( cd ) {
        m_projectData.m_srcProject = cd->getProject();
        UpdateProjectPage();
    }
}

void NewProjectWizard::OnProjectNameChanged(wxCommandEvent& event)
{
    wxUnusedVar( event );
    UpdateFullFileName();
}

void NewProjectWizard::OnProjectPathUpdated(wxCommandEvent& event)
{
    wxUnusedVar( event );
    UpdateFullFileName();
}

void NewProjectWizard::OnFinish(wxWizardEvent& event)
{
    wxFileName fn( m_stxtFullFileName->GetLabel() );
    
    // Ensure that the target folder exists
    fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    
    // make sure that there is no conflict in files between the template project and the selected path
    if (m_projectData.m_srcProject) {
        ProjectPtr p = m_projectData.m_srcProject;
        wxString base_dir( fn.GetPath() );
        std::vector<wxFileName> files;
        p->GetFiles(files);

        for (size_t i=0; i<files.size(); ++i) {
            wxFileName f = files.at(i);
            wxString new_file = base_dir + wxT("/") + f.GetFullName();

            if ( wxFileName::FileExists(new_file) ) {
                // this file already - notify the user
                wxString msg;
                msg << _("The File '") << f.GetFullName() << _("' already exists at the target directory '") << base_dir << wxT("'\n");
                msg << _("Please select a different project path\n");
                msg << _("The file '") << f.GetFullName() << _("' is part of the template project [") << p->GetName() << wxT("]");
                wxMessageBox(msg, _("CodeLite"), wxOK|wxICON_HAND);
                event.Veto();
                return;
            }
        }
    }

    m_projectData.m_name         = m_txtProjName->GetValue();
    m_projectData.m_path         = fn.GetPath();
    m_projectData.m_cmpType      = m_choiceCompiler->GetStringSelection();
    m_projectData.m_debuggerType = m_choiceDebugger->GetStringSelection();
    event.Skip();
}

void NewProjectWizard::OnPageChanging(wxWizardEvent& event)
{
    if ( event.GetDirection() ) {
        // -------------------------------------------------------
        // Switching from the Templates page
        // -------------------------------------------------------
        if ( event.GetPage() == m_wizardPageTemplate ) {
            if ( !CheckProjectTemplate() ) {
                event.Veto();
                return;
            }

        } else if ( event.GetPage() == m_wizardPageDetails ) {
            if( !CheckProjectName() || !CheckProjectPath() ) {
                event.Veto();
                return;
            }
        }
    }
    event.Skip();
}

bool NewProjectWizard::CheckProjectName()
{
    wxString projectName = m_txtProjName->GetValue();
    projectName.Trim().Trim(false);
    if(projectName.find_first_not_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")) != wxString::npos || projectName.IsEmpty() ) {
        wxMessageBox(_("Project name can only contain alpha numeric characters and/or the underscore '_'"), "CodeLite", wxOK | wxICON_WARNING | wxCENTER, this);
        return false;
    }
    return true;
}

bool NewProjectWizard::CheckProjectPath()
{
    wxFileName fn(m_textCtrlProjectPath->GetValue());
    if ( !fn.Exists() ) {
        wxMessageBox(_("Invalid project path selected: ") + fn.GetPath(), "CodeLite", wxOK | wxICON_WARNING |wxCENTER, this);
        return false;
    }
    return true;
}

bool NewProjectWizard::CheckProjectTemplate()
{
    // Make sure we have a selection
    wxDataViewItem sel = m_dataviewTemplates->GetSelection();
    if ( !sel.IsOk() || !dynamic_cast<NewProjectClientData*>(m_dataviewTemplatesModel->GetClientObject(sel) ) ) {
        ::wxMessageBox(_("Please select a template from the list"), "CodeLite", wxOK|wxCENTER|wxICON_WARNING, this);
        return false;
        
    }
    return true;
}
