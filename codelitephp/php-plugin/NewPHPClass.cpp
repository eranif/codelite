#include "NewPHPClass.h"
#include <windowattrmanager.h>
#include <wx/dirdlg.h>
#include "php_entry.h"
#include <wx/tokenzr.h>
#include <wx/textdlg.h>

NewPHPClass::NewPHPClass(wxWindow* parent, const wxString &classPath)
    : NewPHPClassBase(parent)
    , m_userModifiedFileName(false)
{
    m_textCtrlFilePath->SetValue( classPath );
    WindowAttrManager::Load(this, "NewPHPClass");
}

NewPHPClass::~NewPHPClass()
{
    WindowAttrManager::Save(this, "NewPHPClass");
}

void NewPHPClass::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlClassName->GetValue().IsEmpty() &&
                  !m_textCtrlFileName->GetValue().IsEmpty()  &&
                  !m_textCtrlFilePath->IsEmpty() );
}

PHPClassDetails NewPHPClass::GetDetails() const
{
    PHPClassDetails pcd;
    size_t flags = 0;
    if ( m_checkBoxCtor->IsChecked() ) flags |= PHPClassDetails::GEN_CTOR;
    if ( m_checkBoxDtor->IsChecked() ) flags |= PHPClassDetails::GEN_DTOR;
    if ( m_checkBoxSingleton->IsChecked() ) flags |= PHPClassDetails::GEN_SINGLETON;
    if ( m_checkBoxFolderPerNamespace->IsChecked() ) flags |= PHPClassDetails::GEN_FOLDER_PER_NAMESPACE;
    pcd.SetFlags( flags );
    
    wxString filepath;
    filepath << m_textCtrlFilePath->GetValue() << wxFILE_SEP_PATH;
    if ( pcd.GetFlags() & PHPClassDetails::GEN_FOLDER_PER_NAMESPACE ) {
        filepath << m_textCtrlNamespace->GetValue() << wxFILE_SEP_PATH;
    }
    filepath << m_textCtrlFileName->GetValue();
    
    wxFileName fn(filepath);
    fn.Normalize(wxPATH_NORM_DOTS|wxPATH_NORM_TILDE|wxPATH_NORM_ENV_VARS);
    
    pcd.SetFilepath( fn );
    pcd.SetName( m_textCtrlClassName->GetValue( ));
    pcd.SetClassNamespace(m_textCtrlNamespace->GetValue());
    pcd.SetType( m_choiceType->GetStringSelection() );

    pcd.SetExtends( ::wxStringTokenize(m_textCtrlExtends->GetValue(), ",", wxTOKEN_STRTOK) );
    pcd.SetImplements( ::wxStringTokenize(m_textCtrlImplements->GetValue(), ",", wxTOKEN_STRTOK ));
    return pcd;
}

void NewPHPClass::OnClassNameUpdate(wxCommandEvent& event)
{
    wxString class_name = m_textCtrlClassName->GetValue();
    wxString file_name  = wxFileName("", m_textCtrlFileName->GetValue()).GetName(); // the filename without the extension

    if ( file_name.IsEmpty() || !m_userModifiedFileName ) {
        m_textCtrlFileName->ChangeValue( class_name + ".php" );
    }
}

void NewPHPClass::OnFolderPerNamespace(wxCommandEvent& event)
{
    wxUnusedVar(event);
}

void NewPHPClass::OnNamespaceTextUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
}

void NewPHPClass::OnFileNameUpdated(wxCommandEvent& event)
{
    m_userModifiedFileName = true;
}

void NewPHPClass::OnMakeSingletonUI(wxUpdateUIEvent& event)
{
    event.Enable( m_choiceType->GetStringSelection() == "class" );
}

void NewPHPClass::OnBrowse(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector(_("Select a folder"), m_textCtrlFilePath->GetValue(), wxDD_DIR_MUST_EXIST);
    if ( !path.IsEmpty() ) {
        m_textCtrlFilePath->ChangeValue( path );
    }
}

void NewPHPClass::OnFolderUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
}

void NewPHPClass::OnEditExtends(wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this, 
                          _("Place each parent in a separate line"), 
                          _("Edit Class Extends"), 
                          ::wxJoin( ::wxSplit(m_textCtrlExtends->GetValue(), ','), '\n'), 
                          wxTE_MULTILINE | wxTextEntryDialogStyle);
    if ( dlg.ShowModal() == wxID_OK ) {
        m_textCtrlExtends->ChangeValue( ::wxJoin( ::wxSplit(dlg.GetValue(), '\n'), ',' ) );
    }
}

void NewPHPClass::OnEditImplements(wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this, 
                          _("Place each parent in a separate line"), 
                          _("Edit Class Interfaces"), 
                          ::wxJoin( ::wxSplit(m_textCtrlImplements->GetValue(), ','), '\n'), 
                          wxTE_MULTILINE | wxTextEntryDialogStyle);

    if ( dlg.ShowModal() == wxID_OK ) {
        m_textCtrlImplements->ChangeValue( ::wxJoin( ::wxSplit(dlg.GetValue(), '\n'), ',' ) );
    }
}

wxString PHPClassDetails::ToString(const wxString & EOL, const wxString &indent) const
{
    wxString classString;
    if ( !GetClassNamespace().IsEmpty() ) {
        classString << "namespace " << GetClassNamespace() << ";" << EOL << EOL;
    }

    classString << GetType() << " " << GetName() << " ";
    
    // Add extends
    const wxArrayString &extends    = GetExtends();
    const wxArrayString &implements = GetImplements();
    
    if ( !extends.IsEmpty() ) {
        classString << "extends ";
        for(size_t i=0; i<extends.GetCount(); ++i) {
            classString << extends.Item(i) << ", ";
        }
        classString.RemoveLast(2).Append(" ");
    }

    if ( !implements.IsEmpty() ) {
        classString << "implements ";
        for(size_t i=0; i<implements.GetCount(); ++i) {
            classString << implements.Item(i) << ", ";
        }
        classString.RemoveLast(2).Append(" ");
    }
    
    classString << "{" << EOL;
    
    if ( IsClass() && (GetFlags() & GEN_CTOR) ) {
        classString << indent << "public function __construct() {" << EOL;
        classString << indent << indent << EOL;
        classString << indent << "}" << EOL << EOL;
    }

    if ( IsClass() && (GetFlags() & GEN_DTOR) ) {
        classString << indent << "public function __destruct() {" << EOL;
        classString << indent << indent << EOL;
        classString << indent << "}" << EOL << EOL;
    }

    if ( IsClass() && (GetFlags() & GEN_SINGLETON)) {
        wxString returnType;
        if ( GetClassNamespace().IsEmpty() ) {
            returnType = GetName();
        } else {
            // user provided a namespace
            wxString fixedName, fixedNS;
            PHPEntry::CorrectClassName(GetName(), GetClassNamespace(), fixedName, fixedNS);
            returnType << fixedNS << "\\" << fixedName;

        }

        classString << indent << "/** @return " << returnType << " **/" << EOL;
        classString << indent << "static public function instance() {" << EOL;
        classString << indent << indent << "static $s_instance = null;" << EOL;
        classString << indent << indent << "if ( $s_instance === null ) {" << EOL;
        classString << indent << indent << indent << "$s_instance = new " << GetName() << "();" << EOL;
        classString << indent << indent << "}" << EOL;
        classString << indent << indent << "return $s_instance;" << EOL;
        classString << indent << "}" << EOL << EOL;

    }
    classString << "}" << EOL;
    return classString;
}
