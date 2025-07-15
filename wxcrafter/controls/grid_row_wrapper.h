#ifndef GRIDROWWRAPPER_H
#define GRIDROWWRAPPER_H

#include "wxc_widget.h"
#include <vector>

class GridRowWrapper : public wxcWidget
{
public:
    using Vector_t = std::vector<GridRowWrapper*>;

public:
    GridRowWrapper();
    ~GridRowWrapper() override = default;

public:
    wxString Code(int rowIndex) const;

public:
    // wxcWidget
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // GRIDROWWRAPPER_H
