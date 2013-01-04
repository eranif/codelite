#ifndef __implement_parent_virtual_functions__
#define __implement_parent_virtual_functions__

#include "implementparentvirtualfunctionsbase.h"
#include <vector>
#include "entry.h"
#include <wx/clntdata.h>

class ContextCpp;
class ImplementParentVirtualFunctionsDialog;

class clFunctionImplDetails : public wxClientData
{
protected:
    wxString    m_visibility;
    bool        m_prependVirtualKeyword;
    TagEntryPtr m_tag;
    bool        m_selected;
    bool        m_doxygen;

    friend class ImplementParentVirtualFunctionsDialog;

protected:
    clFunctionImplDetails() : m_visibility("public"), m_prependVirtualKeyword(false), m_selected(true), m_doxygen(false) {}
    virtual ~clFunctionImplDetails() {}
    void SetTag(TagEntryPtr tag) {
        m_tag = tag;
    }
    void SetVisibility(const wxString &visibility) {
        this->m_visibility = visibility;
    }
    const wxString& GetVisibility() const {
        return m_visibility;
    }
    void SetPrependVirtualKeyword(bool prependVirtualKeyword) {
        this->m_prependVirtualKeyword = prependVirtualKeyword;
    }
    bool IsPrependVirtualKeyword() const {
        return m_prependVirtualKeyword;
    }
    void SetSelected(bool selected) {
        this->m_selected = selected;
    }
    bool IsSelected() const {
        return m_selected;
    }
    void SetDoxygen(bool doxygen) {
        this->m_doxygen = doxygen;
    }
    bool IsDoxygen() const {
        return m_doxygen;
    }

    wxString GetImpl(ImplementParentVirtualFunctionsDialog* dlg) const;
    wxString GetDecl(ImplementParentVirtualFunctionsDialog* dlg) const;
};

// ------------------------------------------------------------

class ImplementParentVirtualFunctionsDialog : public ImplementParentVirtualFunctionsBase
{
    std::vector<TagEntryPtr> m_tags;
    wxChar                   m_doxyPrefix;
    ContextCpp*              m_contextCpp;
    wxString                 m_scope;
    friend class clFunctionImplDetails;

protected:
    virtual void OnValueChanged(wxDataViewEvent& event);
    void DoInitialize(bool updateDoxyOnly);
    wxString DoMakeCommentForTag(TagEntryPtr tag) const;

public:
    /** Constructor */
    ImplementParentVirtualFunctionsDialog(wxWindow* parent, const wxString &scopeName, const std::vector<TagEntryPtr> &tags, wxChar doxyPrefix /* @ or \\ */, ContextCpp *contextCpp);
    virtual ~ImplementParentVirtualFunctionsDialog();


    wxString GetDecl(const wxString &visibility);
    wxString GetImpl();

    void SetTargetFile(const wxString &file);
    wxString GetTargetFile() const {
        return m_textCtrlImplFile->GetValue();
    }

    bool IsFormatAfterInsert() const {
        return m_checkBoxFormat->IsChecked();
    }
};

#endif // __implement_parent_virtual_functions__
