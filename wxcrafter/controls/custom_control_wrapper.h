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
    ~CustomControlWrapper() override = default;

protected:
    void DoDeepCopy(const wxcWidget& rhs,
                    enum DuplicatingOptions nametypesToChange,
                    const std::set<wxString>& existingNames,
                    const wxString& chosenName = "",
                    const wxString& chosenInheritedName = "",
                    const wxString& chosenFilename = "") override;

public:
    void SetTemplInfoName(const wxString& templInfoName);
    const wxString& GetTemplInfoName() const { return m_templInfoName; }

    void Serialize(nlohmann::json& json) const override;
    void UnSerialize(const nlohmann::json& json) override;
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};
#endif // CUSTOMCONTROLWRAPPER_H
