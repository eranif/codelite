#ifndef TIMERCTRLWRAPPER_H
#define TIMERCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TimePickerCtrlWrapper : public wxcWidget
{
public:
    TimePickerCtrlWrapper();
    virtual ~TimePickerCtrlWrapper();

public:
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    wxString CppCtorCode() const;
    void LoadPropertiesFromXRC(const wxXmlNode* node);
    void LoadPropertiesFromwxFB(const wxXmlNode* node);
    void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
    wxcWidget* Clone() const { return new TimePickerCtrlWrapper(); }
};

#endif // TIMERCTRLWRAPPER_H
