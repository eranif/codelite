#ifndef GRIDCOLUMNWRAPPER_H
#define GRIDCOLUMNWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget
#include <vector>

class GridColumnWrapper : public wxcWidget
{
public:
    using Vector_t = std::vector<GridColumnWrapper*>;

public:
    GridColumnWrapper();
    ~GridColumnWrapper() override = default;

public:
    wxString Code(int colIndex) const;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // GRIDCOLUMNWRAPPER_H
