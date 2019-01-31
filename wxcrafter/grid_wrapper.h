#ifndef GRIDWRAPPER_H
#define GRIDWRAPPER_H

#include "grid_column_wrapper.h"
#include "grid_row_wrapper.h"
#include "wxc_widget.h" // Base class: WrapperBase

class GridWrapper : public wxcWidget
{
public:
    GridWrapper();
    virtual ~GridWrapper();

    // Since version 1.2 we want to change the columns/lines management
    virtual void UnSerialize(const JSONElement& json);

protected:
    void GetRowsCols(GridRowWrapper::Vector_t& rows, GridColumnWrapper::Vector_t& cols) const;

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // GRIDWRAPPER_H
