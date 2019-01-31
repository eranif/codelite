#ifndef SERIALNUMBER_H
#define SERIALNUMBER_H

#include "globals.h"

#define WXC_PREFIX_V_1_0 "wxcrafter-BETA-"
#define WXC_PREFIX_V_2_0 "wxcrafter-2-0-"

extern const wxEventType wxEVT_LICENSE_UPDATED_SUCCESSFULLY;
extern const wxEventType wxEVT_LICENSE_UPDATED_UNSUCCESSFULLY;

class SerialNumber
{
public:
    SerialNumber();
    virtual ~SerialNumber();

    bool IsValid(const wxString& serialNumber, const wxString& email) const;
    bool Isvalid2(const wxString& serialNumber, const wxString& email) const;
};

#endif // SERIALNUMBER_H
