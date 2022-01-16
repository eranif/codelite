#ifndef TOPLEVELWINWRAPPER_H
#define TOPLEVELWINWRAPPER_H

#include "string_property.h"
#include "wxc_project_metadata.h"
#include "wxc_widget.h" // Base class: WrapperBase

#include <wx/gdicmn.h>

class TopLevelWinWrapper : public wxcWidget
{
    bool m_auiDropDownMenuHelperRegistered;

protected:
    TopLevelWinWrapper(int type);
    virtual ~TopLevelWinWrapper();
    wxString FormatCode(const wxString& chunk) const;
    virtual wxString BaseCtorImplPrefix() const = 0;
    virtual wxString BaseCtorDecl() const = 0;

public:
    static void WrapXRC(wxString& text);
    virtual wxString DesignerXRC(bool forPreviewDialog) const = 0;

    virtual wxString CppCtorCode() const;
    /**
     * @brief to avoid code duplications, check if another aui toolbar
     * already registered the helpers for this TLW
     */
    bool IsAuiToolBarDropDownHelpersRegistered() const { return m_auiDropDownMenuHelperRegistered; }

    /**
     * @brief set whether a aui toolbar has registered the helpers for this TLW
     */
    void SetAuiToolBarDropDownHelpersRegistered(bool reg) { m_auiDropDownMenuHelperRegistered = reg; }
    /**
     * @brief return the derived class constructor signature (including the braces)
     * by default this functions returns "(wxWindow* parent)"
     * @return
     */
    virtual wxString GetDerivedClassCtorSignature() const { return "(wxWindow* parent)"; }

    /**
     * @brief return the arguments that should be passed to the parent constructor
     * @return
     */
    virtual wxString GetParentCtorInitArgumentList() const { return "(parent)"; }

    virtual void GenerateCode(const wxcProjectMetadata& project, bool promptUser, bool baseOnly, wxString& baseCpp,
                              wxString& baseHeader, wxArrayString& headers, wxStringMap_t& additionalFiles);

    virtual bool IsTopWindow() const { return true; }
    /**
     * @brief return true if the top-level window is a real wxWindow derived class
     */
    virtual bool IsWindow() const { return true; }

    /**
     * @brief return true if this top-level window
     * should have a subclass as well
     */
    virtual bool WantsSubclass() const { return true; }

    /**
     * @brief return true if the TopLevel has an icon (this will be true for wxFrame, wxDialog and wxWizard)
     * @return by default return false, derived child can override this
     */
    virtual bool HasIcon() const { return false; }

    /**
     * @brief return true if the current top level window represents a wxWidgets's top level window
     * (wxWizard, wxDialog or wxFrame)
     */
    virtual bool IsWxTopLevelWindow() const;
};

#endif // TOPLEVELWINWRAPPER_H
