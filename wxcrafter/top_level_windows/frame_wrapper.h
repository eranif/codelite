#ifndef FRAMEWRAPPER_H
#define FRAMEWRAPPER_H

#include "top_level_win_wrapper.h" // Base class: TopLevelWinWrapper
#include <wx/gdicmn.h>

class FrameWrapper : public TopLevelWinWrapper
{
protected:
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString BaseCtorImplPrefix() const override;
    wxString BaseCtorDecl() const override;
    wxString CppCtorCode() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;

public:
    FrameWrapper();
    ~FrameWrapper() override = default;

    wxString GetDerivedClassCtorSignature() const override;
    wxString GetParentCtorInitArgumentList() const override;
    bool HasIcon() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    /**
     * @brief similar to ToXRC, however
     * we replace the topwindow with a wxPanel
     */
    wxString DesignerXRC(bool forPreviewDialog) const override;

    wxcWidget* Clone() const override { return new FrameWrapper(); }

    wxString GetWxClassName() const override;
};

#endif // FRAMEWRAPPER_H
