#include "serial_number.h"
#include <wxmd5.h>

const wxEventType wxEVT_LICENSE_UPDATED_SUCCESSFULLY = ::wxNewEventType();
const wxEventType wxEVT_LICENSE_UPDATED_UNSUCCESSFULLY = ::wxNewEventType();

SerialNumber::SerialNumber() {}

SerialNumber::~SerialNumber() {}

bool SerialNumber::IsValid(const wxString& serialNumber, const wxString& email) const { return true; }

bool SerialNumber::Isvalid2(const wxString& serialNumber, const wxString& email) const { return true; }
