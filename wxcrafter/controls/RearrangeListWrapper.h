#ifndef REARRANGELISTWRAPPER_H
#define REARRANGELISTWRAPPER_H

#include "check_list_box_wrapper.h" // Base class: CheckListBoxWrapper

class RearrangeListWrapper : public CheckListBoxWrapper
{
public:
    RearrangeListWrapper();
    ~RearrangeListWrapper() override = default;

public:
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override;
};

#endif // REARRANGELISTWRAPPER_H
