#ifndef IMAGELISTWRAPPER_H
#define IMAGELISTWRAPPER_H

#include "top_level_win_wrapper.h"

class ImageListWrapper : public TopLevelWinWrapper
{
public:
    ImageListWrapper();
    ~ImageListWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    wxString BaseCtorDecl() const override;
    wxString BaseCtorImplPrefix() const override;
    wxString DesignerXRC(bool forPreviewDialog) const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString DoGenerateClassMember() const override;
    wxString CreateBaseclassName() const override { return GetName(); }
    // not a real window
    bool IsWindow() const override { return false; }
    // no base class
    bool WantsSubclass() const override { return false; }
};

#endif // IMAGELISTWRAPPER_H
