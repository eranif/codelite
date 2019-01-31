#ifndef GRIDCOLUMNWRAPPER_H
#define GRIDCOLUMNWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget
#include <vector>

class GridColumnWrapper : public wxcWidget
{
public:
    typedef std::vector<GridColumnWrapper*> Vector_t;

public:
    GridColumnWrapper();
    virtual ~GridColumnWrapper();

public:
    wxString Code(int colIndex) const;

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // GRIDCOLUMNWRAPPER_H
