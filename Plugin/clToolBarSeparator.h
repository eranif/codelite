#ifndef CLTOOLBARSEPARATOR_H
#define CLTOOLBARSEPARATOR_H

#include "clToolBarButtonBase.h"

class WXDLLIMPEXP_SDK clToolBarSeparator : public clToolBarButtonBase
{
public:
    clToolBarSeparator(clToolBar* parent);
    virtual ~clToolBarSeparator();

    void Render(wxDC& dc, const wxRect& rect);

public:
    virtual wxSize CalculateSize(wxDC& dc) const;
};

#endif // CLTOOLBARSEPARATOR_H
