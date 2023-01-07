#ifndef TOP_PANELWRAPPER_H
#define TOP_PANELWRAPPER_H

#include "top_level_win_wrapper.h"
#include "wxc_widget.h" // Base class: WrapperBase

class PanelWrapperTopLevel : public TopLevelWinWrapper
{
protected:
    virtual wxString BaseCtorDecl() const;
    virtual wxString BaseCtorImplPrefix() const;

public:
    PanelWrapperTopLevel();
    virtual ~PanelWrapperTopLevel();

public:
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual wxString CppCtorCode() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString DesignerXRC(bool forPreviewDialog) const;
    virtual bool IsTopWindow() const;
    virtual wxcWidget* Clone() const { return new PanelWrapperTopLevel(); }
};

#endif // TOP_PANELWRAPPER_H
