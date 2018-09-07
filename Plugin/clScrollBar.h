#ifndef CLSCROLLBAR_H
#define CLSCROLLBAR_H

#include "codelite_exports.h"
#include <wx/scrolbar.h>

class WXDLLIMPEXP_SDK clScrollBarHelper : public wxScrollBar
{
public:
    clScrollBarHelper(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxSB_VERTICAL);
    virtual ~clScrollBarHelper();
    void SetScrollbar(int position, int thumbSize, int range, int pageSize, bool refresh = true);
};

#endif // CLSCROLLBAR_H
