#ifndef PROPERTYBASE_H
#define PROPERTYBASE_H

#include "wxcLib/json_node.h"
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>

enum PropertyeType {
    PT_BOOL,
    PT_CHOICE,
    PT_COLOR_PICKER,
    PT_FILE_PICKER,
    PT_FONT_PICKER,
    PT_MUTLI_STRING_PICKER,
    PT_STRING,
    PT_VIRTUAL_FOLDER_PICKER,
    PT_COMBOX,
    PT_CATEGORY,
    PT_INT,
    PT_COL_HEADER_FLAGS,
    PT_BITMAP,
    PT_BITMAPTEXT_TABLE,
};

const extern wxEventType wxEVT_WXC_PROPETY_CHANGED;
class PropertyBase : public wxEvtHandler
{
protected:
    wxString m_label;
    wxString m_tooltip;

    void DoBaseSerialize(JSONElement& json) const;
    void DoBaseUnSerialize(const JSONElement& json);
    void NotifyChanged();

public:
    PropertyBase(const wxString& tooltip);
    virtual ~PropertyBase();

    void SetLabel(const wxString& label) { this->m_label = label; }

    const wxString& GetLabel() const { return m_label; }

    wxString GetLabelForUI() const
    {
        wxString label = GetLabel();
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
    virtual JSONElement Serialize() const = 0;
    virtual void UnSerialize(const JSONElement& json) = 0;
};

#endif // PROPERTYBASE_H
