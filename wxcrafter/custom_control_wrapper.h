#ifndef CUSTOMCONTROLWRAPPER_H
#define CUSTOMCONTROLWRAPPER_H

#include "wxc_settings.h"
#include "wxc_widget.h" // Base class: WrapperBase

class CustomControlWrapper : public wxcWidget
{
    wxString m_templInfoName;

protected:
    void DoUpdateEvents();

public:
    CustomControlWrapper();
    virtual ~CustomControlWrapper();

protected:
    void DoDeepCopy(const wxcWidget& rhs, enum DuplicatingOptions nametypesToChange,
                    const std::set<wxString>& existingNames, const wxString& chosenName = "",
                    const wxString& chosenInheritedName = "", const wxString& chosenFilename = "");

public:
    virtual void Serialize(JSONElement& json) const;
    virtual void UnSerialize(const JSONElement& json);

    void SetTemplInfoName(const wxString& templInfoName);
    const wxString& GetTemplInfoName() const { return m_templInfoName; }
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
};
#endif // CUSTOMCONTROLWRAPPER_H
