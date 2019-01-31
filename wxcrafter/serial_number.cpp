#include "serial_number.h"
#include <wxmd5.h>

const wxEventType wxEVT_LICENSE_UPDATED_SUCCESSFULLY = ::wxNewEventType();
const wxEventType wxEVT_LICENSE_UPDATED_UNSUCCESSFULLY = ::wxNewEventType();

SerialNumber::SerialNumber() {}

SerialNumber::~SerialNumber() {}

bool SerialNumber::IsValid(const wxString& serialNumber, const wxString& email) const
{
#if STANDALONE_BUILD
    if(Isvalid2(serialNumber, email)) return true;

    // Try v1.X license
    for(size_t i = 0; i < 50; i++) {

        // Generate the serial number based on the username
        wxString inputString;
        inputString << WXC_PREFIX_V_1_0 << email << "-" << i;

        wxString md5 = wxMD5::GetDigest(inputString);
        md5.MakeUpper();
        if(md5 == serialNumber) return true;
    }
    return false;
#else
    return true;
#endif
}

bool SerialNumber::Isvalid2(const wxString& serialNumber, const wxString& email) const
{
#if STANDALONE_BUILD
    for(size_t i = 0; i < 50; i++) {

        // Generate the serial number based on the username
        wxString inputString;
        inputString << WXC_PREFIX_V_2_0 << email << "-" << i;

        wxString md5 = wxMD5::GetDigest(inputString);
        md5.MakeUpper();
        if(md5 == serialNumber) return true;
    }
    return false;
#else
    return true;
#endif
}
