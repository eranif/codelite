#ifndef DIALOGWRAPPER_H
#define DIALOGWRAPPER_H

#include "top_level_win_wrapper.h" // Base class: TopLevelWinWrapper

class DialogWrapper : public TopLevelWinWrapper
{

public:
    DialogWrapper();
    virtual ~DialogWrapper();

public:
    virtual bool HasIcon() const;
    virtual wxString DesignerXRC(bool forPreviewDialog) const;
    virtual wxString BaseCtorDecl() const;
    virtual wxString BaseCtorImplPrefix() const;
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // DIALOGWRAPPER_H
