#ifndef SPINWRAPPERBASE_H
#define SPINWRAPPERBASE_H

#include "wxc_widget.h" // Base class: wxcWidget

class SpinWrapperBase : public wxcWidget
{
public:
    explicit SpinWrapperBase(int ID);
    ~SpinWrapperBase() override = default;

    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;

    void EnsureSaneValues(wxString& min, wxString& max, int& imin, int& imax, int& val) const;
};

#endif // SPINWRAPPERBASE_H
