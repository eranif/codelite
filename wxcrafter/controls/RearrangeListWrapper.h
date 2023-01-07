#ifndef REARRANGELISTWRAPPER_H
#define REARRANGELISTWRAPPER_H

#include "check_list_box_wrapper.h" // Base class: CheckListBoxWrapper

class RearrangeListWrapper : public CheckListBoxWrapper
{
public:
    RearrangeListWrapper();
    virtual ~RearrangeListWrapper();

public:
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxcWidget* Clone() const;
};

#endif // REARRANGELISTWRAPPER_H
