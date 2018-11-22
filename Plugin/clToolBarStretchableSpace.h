#ifndef CLTOOLBARSTRETCHABLESPACE_H
#define CLTOOLBARSTRETCHABLESPACE_H

#include "clToolBarButtonBase.h"
#include "codelite_exports.h"

class clToolBar;
class WXDLLIMPEXP_SDK clToolBarStretchableSpace : public clToolBarButtonBase
{
    size_t m_width = 0;

public:
    clToolBarStretchableSpace(clToolBar* parent);
    virtual ~clToolBarStretchableSpace();

    virtual wxSize CalculateSize(wxDC& dc) const;
    virtual void Render(wxDC& dc, const wxRect& rect);
    
    void SetWidth(size_t width) { this->m_width = width; }
    size_t GetWidth() const { return m_width; }
};

#endif // CLTOOLBARSTRETCHABLESPACE_H
