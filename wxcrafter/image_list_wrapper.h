#ifndef IMAGELISTWRAPPER_H
#define IMAGELISTWRAPPER_H

#include "top_level_win_wrapper.h"

class ImageListWrapper : public TopLevelWinWrapper
{
public:
    ImageListWrapper();
    virtual ~ImageListWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual wxString BaseCtorDecl() const;
    virtual wxString BaseCtorImplPrefix() const;
    virtual wxString DesignerXRC(bool forPreviewDialog) const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString DoGenerateClassMember() const;
    virtual wxString CreateBaseclassName() const { return GetName(); }
    // not a real window
    virtual bool IsWindow() const { return false; }
    // no base class
    virtual bool WantsSubclass() const { return false; }
    virtual bool IsLicensed() const;
};

#endif // IMAGELISTWRAPPER_H
