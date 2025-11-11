#ifndef GRIDWRAPPER_H
#define GRIDWRAPPER_H

#include "grid_column_wrapper.h"
#include "grid_row_wrapper.h"
#include "wxc_widget.h" // Base class: WrapperBase

class GridWrapper : public wxcWidget
{
public:
    GridWrapper();
    ~GridWrapper() override = default;

    // Since version 1.2 we want to change the columns/lines management
    void UnSerialize(const nlohmann::json& json) override;

protected:
    void GetRowsCols(GridRowWrapper::Vector_t& rows, GridColumnWrapper::Vector_t& cols) const;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // GRIDWRAPPER_H
