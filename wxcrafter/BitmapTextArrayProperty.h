#ifndef BITMAPTEXTARRAYPROPERTY_H
#define BITMAPTEXTARRAYPROPERTY_H

#include "property_base.h"
class BitmapTextArrayProperty : public PropertyBase
{
protected:
    wxString m_value;

public:
    BitmapTextArrayProperty(const wxString& label, const wxString& value, const wxString& tooltip);
    virtual ~BitmapTextArrayProperty();

    virtual PropertyeType GetType();
    virtual wxString GetValue() const;
    virtual JSONElement Serialize() const;
    virtual void SetValue(const wxString& value);
    virtual void UnSerialize(const JSONElement& json);
};

#endif // BITMAPTEXTARRAYPROPERTY_H
