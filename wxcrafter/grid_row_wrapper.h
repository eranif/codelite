#ifndef GRIDROWWRAPPER_H
#define GRIDROWWRAPPER_H

#include "wxc_widget.h"
#include <vector>

class GridRowWrapper : public wxcWidget
{
public:
    typedef std::vector<GridRowWrapper*> Vector_t;

public:
    GridRowWrapper();
    virtual ~GridRowWrapper();

public:
    wxString Code(int rowIndex) const;

public:
    // wxcWidget
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // GRIDROWWRAPPER_H
