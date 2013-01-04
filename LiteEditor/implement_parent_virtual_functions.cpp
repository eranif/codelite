#include "implement_parent_virtual_functions.h"
#include "editor_config.h"
#include "implparentvirtualfunctionsdata.h"
#include <wx/tokenzr.h>
#include "ctags_manager.h"
#include "cpp_comment_creator.h"
#include "windowattrmanager.h"
#include "context_cpp.h"

ImplementParentVirtualFunctionsDialog::ImplementParentVirtualFunctionsDialog( wxWindow* parent, const wxString &scopeName, const std::vector<TagEntryPtr> &tags, wxChar doxyPrefix, ContextCpp *contextCpp )
    : ImplementParentVirtualFunctionsBase(parent    )
    , m_tags                             (tags      )
    , m_doxyPrefix                       (doxyPrefix)
    , m_contextCpp                       (contextCpp)
    , m_scope                            (scopeName )
{
    WindowAttrManager::Load(this, wxT("ImplementParentVirtualFunctionsDialog"), NULL);
    ImplParentVirtualFunctionsData data;
    EditorConfigST::Get()->ReadObject(wxT("ImplParentVirtualFunctionsData"), &data);

    m_checkBoxFormat->SetValue(data.GetFlags() & ImplParentVirtualFunctionsData::FormatText);
    DoInitialize(false);
}

ImplementParentVirtualFunctionsDialog::~ImplementParentVirtualFunctionsDialog()
{
    ImplParentVirtualFunctionsData data;

    size_t flags(0);

    if(m_checkBoxFormat->IsChecked())
        flags |= ImplParentVirtualFunctionsData::FormatText;

    data.SetFlags(flags);
    EditorConfigST::Get()->WriteObject(wxT("ImplParentVirtualFunctionsData"), &data);
    WindowAttrManager::Save(this, wxT("ImplementParentVirtualFunctionsDialog"), NULL);
}

void ImplementParentVirtualFunctionsDialog::DoInitialize(bool updateDoxyOnly)
{
    m_dataviewModel->Clear();

    wxVector<wxVariant> cols;

    // Add declration
    //////////////////////////////////////////////////////
    for (size_t i=0; i<m_tags.size(); ++i) {
        cols.clear();
        cols.push_back(true);                    // generate it
        cols.push_back(m_tags.at(i)->GetName()); // function name
        cols.push_back("public");                // visibility
        cols.push_back(false);                   // virtual
        cols.push_back(false);                   // doxy

        clFunctionImplDetails *cd = new clFunctionImplDetails();
        cd->SetTag( m_tags.at(i) );
        m_dataviewModel->AppendItem(wxDataViewItem(0), cols, cd);
    }
}

wxString ImplementParentVirtualFunctionsDialog::GetDecl(const wxString& visibility)
{
    wxString decl;
    wxDataViewItemArray children;
    m_dataviewModel->GetChildren(wxDataViewItem(0), children);

    for(size_t i=0; i<children.GetCount(); ++i) {
        clFunctionImplDetails *cd = dynamic_cast<clFunctionImplDetails*>( m_dataviewModel->GetClientObject(children.Item(i)));
        if ( cd->GetVisibility() == visibility ) {
            decl << cd->GetDecl(this);
        }
    }
    return decl;
}

wxString ImplementParentVirtualFunctionsDialog::GetImpl()
{
    wxString impl;
    wxDataViewItemArray children;
    m_dataviewModel->GetChildren(wxDataViewItem(0), children);

    for(size_t i=0; i<children.GetCount(); ++i) {
        clFunctionImplDetails *cd = dynamic_cast<clFunctionImplDetails*>( m_dataviewModel->GetClientObject(children.Item(i)));
        impl << cd->GetImpl(this);
    }
    return impl;
}

wxString ImplementParentVirtualFunctionsDialog::DoMakeCommentForTag(TagEntryPtr tag) const
{
    // Add doxygen comment
    CppCommentCreator commentCreator(tag, m_doxyPrefix);
    DoxygenComment dc;
    dc.comment = commentCreator.CreateComment();
    dc.name    = tag->GetName();
    m_contextCpp->DoMakeDoxyCommentString( dc );

    // Format the comment
    wxString textComment = dc.comment;
    textComment.Replace("\r", "\n");
    wxArrayString lines = wxStringTokenize(textComment, "\n", wxTOKEN_STRTOK);
    textComment.Clear();

    for (size_t i=0; i<lines.GetCount(); ++i)
        textComment << lines.Item(i) << wxT("\n");
    return textComment;
}

void ImplementParentVirtualFunctionsDialog::SetTargetFile(const wxString& file)
{
    m_textCtrlImplFile->ChangeValue( file );
}

void ImplementParentVirtualFunctionsDialog::OnValueChanged(wxDataViewEvent& event)
{
    event.Skip();
    clFunctionImplDetails *cd = dynamic_cast<clFunctionImplDetails*>( m_dataviewModel->GetClientObject(event.GetItem()) );
    wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(event.GetItem());
    cd->SetVisibility( cols.at(2).GetString() );
    cd->SetPrependVirtualKeyword( cols.at(3).GetBool() );
    cd->SetSelected( cols.at(0).GetBool() );
    cd->SetDoxygen( cols.at(4).GetBool() );
}

// -----------------------------------------------------------------------------------------
wxString clFunctionImplDetails::GetImpl(ImplementParentVirtualFunctionsDialog* dlg) const
{
    if( !IsSelected() )
        return "";

    wxString impl;
    m_tag->SetScope( dlg->m_scope );
    impl << TagsManagerST::Get()->FormatFunction(m_tag, FunctionFormat_Impl) << wxT("\n");
    impl.Trim().Trim(false);
    impl << "\n\n";

    return impl;
}

wxString clFunctionImplDetails::GetDecl(ImplementParentVirtualFunctionsDialog* dlg) const
{
    if( !IsSelected() )
        return "";

    wxString decl;
    if ( IsDoxygen() ) {
        decl << dlg->DoMakeCommentForTag( m_tag );
    }

    m_tag->SetScope(dlg->m_scope);
    decl << TagsManagerST::Get()->FormatFunction(m_tag, IsPrependVirtualKeyword() ? FunctionFormat_WithVirtual : 0);
    decl.Trim().Trim(false);
    decl << "\n";
    return decl;
}
