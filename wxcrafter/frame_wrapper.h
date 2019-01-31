#ifndef FRAMEWRAPPER_H
#define FRAMEWRAPPER_H

#include "top_level_win_wrapper.h" // Base class: TopLevelWinWrapper
#include <wx/gdicmn.h>

class FrameWrapper : public TopLevelWinWrapper
{
protected:
    void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString BaseCtorImplPrefix() const;
    virtual wxString BaseCtorDecl() const;
    virtual wxString CppCtorCode() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);

public:
    virtual wxString GetDerivedClassCtorSignature() const;
    virtual wxString GetParentCtorInitArgumentList() const;
    virtual bool HasIcon() const;
    FrameWrapper();
    virtual ~FrameWrapper();
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    /**
     * @brief similar to ToXRC, however
     * we replace the topwindow with a wxPanel
     */
    virtual wxString DesignerXRC(bool forPreviewDialog) const;

    wxcWidget* Clone() const { return new FrameWrapper(); }

    virtual wxString GetWxClassName() const;
};

#endif // FRAMEWRAPPER_H
