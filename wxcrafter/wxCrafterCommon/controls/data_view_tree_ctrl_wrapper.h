#ifndef DATAVIEWTREECTRLWRAPPER_H
#define DATAVIEWTREECTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class DataViewTreeCtrlWrapper : public wxcWidget
{
public:
    DataViewTreeCtrlWrapper();
    ~DataViewTreeCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // DATAVIEWTREECTRLWRAPPER_H
