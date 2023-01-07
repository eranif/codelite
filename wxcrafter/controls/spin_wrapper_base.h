#ifndef SPINWRAPPERBASE_H
#define SPINWRAPPERBASE_H

#include "wxc_widget.h" // Base class: wxcWidget

class SpinWrapperBase : public wxcWidget
{
public:
    SpinWrapperBase(int ID);
    virtual ~SpinWrapperBase();

    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);

    void EnsureSaneValues(wxString& min, wxString& max, int& imin, int& imax, int& val) const;
};

#endif // SPINWRAPPERBASE_H
