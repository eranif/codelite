#ifndef IMPORTFROMFB_H
#define IMPORTFROMFB_H

#include "wxc_widget.h"
#include "wxgui_defs.h"

namespace ImportFromwxFB
{
wxString ConvertFBOptionsString(const wxString& content, const wxString& separator = ";");
void ProcessBitmapProperty(const wxString& value,
                           wxcWidget* wrapper,
                           const wxString& property = PROP_BITMAP_PATH,
                           const wxString& client_hint = "");
wxString GetEventtypeFromHandlerstub(const wxString& stub);

} // namespace ImportFromwxFB

#endif // IMPORTFROMFB_H
