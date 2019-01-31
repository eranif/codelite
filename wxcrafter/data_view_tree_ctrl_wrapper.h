#ifndef DATAVIEWTREECTRLWRAPPER_H
#define DATAVIEWTREECTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class DataViewTreeCtrlWrapper : public wxcWidget
{
public:
    DataViewTreeCtrlWrapper();
    virtual ~DataViewTreeCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;

    virtual bool IsLicensed() const;
};

#endif // DATAVIEWTREECTRLWRAPPER_H
