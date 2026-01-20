#ifndef PROPERTYBASE_H
#define PROPERTYBASE_H

#include <assistant/common/json.hpp> // <nlohmann/json.hpp>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/translation.h>

enum PropertyeType {
    PT_BOOL,
    PT_CHOICE,
    PT_COLOR_PICKER,
    PT_FILE_PICKER,
    PT_FONT_PICKER,
    PT_MULTI_STRING_PICKER,
    PT_STRING,
    PT_VIRTUAL_FOLDER_PICKER,
    PT_COMBOX,
    PT_CATEGORY,
    PT_INT,
    PT_COL_HEADER_FLAGS,
    PT_BITMAP,
    PT_BITMAPTEXT_TABLE,
};

const extern wxEventType wxEVT_WXC_PROPERTY_CHANGED;
class PropertyBase : public wxEvtHandler
{
protected:
    wxString m_label;
    wxString m_tooltip;

    void DoBaseSerialize(nlohmann::json& json) const;
    void DoBaseUnSerialize(const nlohmann::json& json);
    void NotifyChanged();

public:
    explicit PropertyBase(const wxString& tooltip);
    ~PropertyBase() override = default;

    void SetLabel(const wxString& label) { this->m_label = label; }

    const wxString& GetLabel() const { return m_label; }

    wxString GetLabelForUI() const
    {
        wxString label = wxGetTranslation(GetLabel());
        label.EndsWith(":", &label);
        return label;
    }

    void SetTooltip(const wxString& tooltip) { this->m_tooltip = tooltip; }
    const wxString& GetTooltip() const { return m_tooltip; }

    virtual wxArrayString GetOptions() const { return wxArrayString(); }

    virtual wxArrayInt GetOptionsValues() const { return wxArrayInt(); }

    virtual PropertyeType GetType() = 0;
    virtual void SetValue(const wxString& value) = 0;
    virtual wxString GetValue() const = 0;
    virtual long GetValueLong() const { return 0; }
    virtual nlohmann::json Serialize() const = 0;
    virtual void UnSerialize(const nlohmann::json& json) = 0;
};

#endif // PROPERTYBASE_H
