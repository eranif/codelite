#include "NewPHPClass.h"
#include <windowattrmanager.h>
#include <wx/dirdlg.h>
#include <wx/tokenzr.h>
#include <wx/textdlg.h>

NewPHPClass::NewPHPClass(wxWindow* parent, const wxString& classPath)
    : NewPHPClassBase(parent)
    , m_userModifiedFileName(false)
    , m_outputPath(classPath)
{
    SetName("NewPHPClass");
    WindowAttrManager::Load(this);
}

NewPHPClass::~NewPHPClass() {  }

void NewPHPClass::OnOKUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlClassName->GetValue().IsEmpty()); }

PHPClassDetails NewPHPClass::GetDetails() const
{
    PHPClassDetails pcd;
    size_t flags = 0;
    if(m_checkBoxCtor->IsChecked()) flags |= PHPClassDetails::GEN_CTOR;
    if(m_checkBoxDtor->IsChecked()) flags |= PHPClassDetails::GEN_DTOR;
    if(m_checkBoxSingleton->IsChecked()) flags |= PHPClassDetails::GEN_SINGLETON;
    if(m_checkBoxFolderPerNamespace->IsChecked()) flags |= PHPClassDetails::GEN_FOLDER_PER_NAMESPACE;
    pcd.SetFlags(flags);

    wxString filepath;
    filepath << m_outputPath << wxFILE_SEP_PATH;
    if(pcd.GetFlags() & PHPClassDetails::GEN_FOLDER_PER_NAMESPACE) {
        filepath << m_textCtrlNamespace->GetValue() << wxFILE_SEP_PATH;
    }
    filepath << m_textCtrlClassName->GetValue() << ".php";

    wxFileName fn(filepath);
    fn.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ENV_VARS);

    pcd.SetFilepath(fn);
    pcd.SetName(m_textCtrlClassName->GetValue());
    pcd.SetClassNamespace(m_textCtrlNamespace->GetValue());
    pcd.SetType(m_choiceType->GetStringSelection());

    pcd.SetExtends(::wxStringTokenize(m_textCtrlExtends->GetValue(), ",", wxTOKEN_STRTOK));
    pcd.SetImplements(::wxStringTokenize(m_textCtrlImplements->GetValue(), ",", wxTOKEN_STRTOK));
    return pcd;
}

void NewPHPClass::OnClassNameUpdate(wxCommandEvent& event) { event.Skip(); }

void NewPHPClass::OnFolderPerNamespace(wxCommandEvent& event) { wxUnusedVar(event); }

void NewPHPClass::OnNamespaceTextUpdated(wxCommandEvent& event) { wxUnusedVar(event); }

void NewPHPClass::OnFileNameUpdated(wxCommandEvent& event) { m_userModifiedFileName = true; }

void NewPHPClass::OnMakeSingletonUI(wxUpdateUIEvent& event)
{
    event.Enable(m_choiceType->GetStringSelection() == "class");
}

void NewPHPClass::OnFolderUpdated(wxCommandEvent& event) { wxUnusedVar(event); }

void NewPHPClass::OnEditExtends(wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this,
                          _("Place each parent in a separate line"),
                          _("Edit Class Extends"),
                          ::wxJoin(::wxSplit(m_textCtrlExtends->GetValue(), ','), '\n'),
                          wxTE_MULTILINE | wxTextEntryDialogStyle);
    if(dlg.ShowModal() == wxID_OK) {
        m_textCtrlExtends->ChangeValue(::wxJoin(::wxSplit(dlg.GetValue(), '\n'), ','));
    }
}

void NewPHPClass::OnEditImplements(wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this,
                          _("Place each parent in a separate line"),
                          _("Edit Class Interfaces"),
                          ::wxJoin(::wxSplit(m_textCtrlImplements->GetValue(), ','), '\n'),
                          wxTE_MULTILINE | wxTextEntryDialogStyle);

    if(dlg.ShowModal() == wxID_OK) {
        m_textCtrlImplements->ChangeValue(::wxJoin(::wxSplit(dlg.GetValue(), '\n'), ','));
    }
}

wxString PHPClassDetails::ToString(const wxString& EOL, const wxString& indent) const
{
    wxString classString;
    if(!GetNamespace().IsEmpty()) {
        classString << "namespace " << GetNamespace() << ";" << EOL << EOL;
    }

    classString << GetType() << " " << GetName() << " ";

    // Add extends
    const wxArrayString& extends = GetExtends();
    const wxArrayString& implements = GetImplements();

    if(!extends.IsEmpty()) {
        classString << "extends ";
        for(size_t i = 0; i < extends.GetCount(); ++i) {
            classString << extends.Item(i) << ", ";
        }
        classString.RemoveLast(2).Append(" ");
    }

    if(!implements.IsEmpty()) {
        classString << "implements ";
        for(size_t i = 0; i < implements.GetCount(); ++i) {
            classString << implements.Item(i) << ", ";
        }
        classString.RemoveLast(2).Append(" ");
    }

    classString  << EOL << "{" << EOL;

    if(IsClass() && (GetFlags() & GEN_SINGLETON)) {
        classString << indent << "/** @var self */" << EOL;
        classString << indent << "protected static $instance;" << EOL;
    }

    if(IsClass() && (GetFlags() & (GEN_CTOR | GEN_SINGLETON))) {
        if(GetFlags() & GEN_SINGLETON) {
            classString << EOL;
        }
        if(GetFlags() & GEN_SINGLETON) {
            classString << indent << "protected function __construct()" << EOL;
        } else {
            classString << indent << "public function __construct()" << EOL;
        }
        classString << indent << "{" << EOL;
        if(!(GetFlags() & GEN_SINGLETON)) {
            classString << indent << indent << EOL;
        }
        classString << indent << "}" << EOL;
    }

    if(IsClass() && (GetFlags() & GEN_DTOR)) {
        if (GetFlags() & (GEN_CTOR | GEN_SINGLETON)) {
            classString << EOL;
        }
        classString << indent << "public function __destruct()" << EOL;
        classString << indent << "{" << EOL;
        classString << indent << indent << EOL;
        classString << indent << "}" << EOL;
    }

    if(IsClass() && (GetFlags() & GEN_SINGLETON)) {
        classString << EOL;
        classString << indent << "/**" << EOL;
        classString << indent << " * @return self" << EOL;
        classString << indent << " */" << EOL;
        classString << indent << "public static function getInstance()" << EOL;
        classString << indent << "{" << EOL;
        classString << indent << indent << "if (!self::$instance) {" << EOL;
        classString << indent << indent << indent << "self::$instance = new self();" << EOL;
        classString << indent << indent << "}" << EOL;
        classString << EOL;
        classString << indent << indent << "return self::$instance;" << EOL;
        classString << indent << "}" << EOL;
    }
    classString << "}" << EOL;
    return classString;
}
