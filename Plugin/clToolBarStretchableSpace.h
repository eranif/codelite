#ifndef CLTOOLBARSTRETCHABLESPACE_H
#define CLTOOLBARSTRETCHABLESPACE_H

#include "clToolBar.h"
#include "clToolBarButtonBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clToolBarStretchableSpace : public clToolBarButtonBase
{
    size_t m_width = 0;

public:
    clToolBarStretchableSpace(clToolBarGeneric* parent);
    virtual ~clToolBarStretchableSpace() = default;

    virtual wxSize CalculateSize(wxDC& dc) const;
    virtual void Render(wxDC& dc, const wxRect& rect);

    void SetWidth(size_t width) { this->m_width = width; }
    size_t GetWidth() const { return m_width; }
};
#endif // CLTOOLBARSTRETCHABLESPACE_H
