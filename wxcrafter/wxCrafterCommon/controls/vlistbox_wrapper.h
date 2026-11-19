#pragma once

#include "wxc_widget.h"

/// Wrapper for wxVListBox.
///
/// wxVListBox is an abstract class: it needs OnDrawItem() and OnMeasureItem(). If the "Subclass" property is set, the
/// generated code creates that class. Otherwise, the generated form class gets a small nested class with a default
/// implementation (it draws "Item N"), so that the code compiles and runs.
///
/// There is no wxXmlResource handler that can create an abstract class without a subclass, so the designer shows a
/// placeholder for it.
class VListBoxWrapper : public wxcWidget
{
public:
    VListBoxWrapper();
    ~VListBoxWrapper() override = default;

    wxcWidget* Clone() const override { return new VListBoxWrapper(); }
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;

protected:
    /// Used by the derived classes
    VListBoxWrapper(int type, const wxString& className, const wxString& namePattern);

    /// Does the XRC loader know how to create this class?
    virtual bool HasXRCHandler() const { return true; }

    /// Generate the default implementation class (nested in the generated form class), if no subclass is set
    void DoGenerateExtraFunctions(wxString& decl, wxString& impl) const override;

    /// The methods of the default implementation class.
    /// @param decl the declarations, inside the class body
    /// @param impl the definitions, outside of the class
    /// @param qualifiedName the full name of the default implementation class, for the definitions
    virtual void DefaultImplMethods(wxString& decl, wxString& impl, const wxString& qualifiedName) const;

    /// True if no subclass is set, which means that the generated default implementation class is used
    bool UsesDefaultImpl() const;

    /// The name of the generated default implementation class
    wxString DefaultImplName() const { return GetName() + "_Impl"; }
};
