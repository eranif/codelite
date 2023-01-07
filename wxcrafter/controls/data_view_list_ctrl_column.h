#ifndef DATAVIEWLISTCTRLCOLUMN_H
#define DATAVIEWLISTCTRLCOLUMN_H

#include "wxc_widget.h" // Base class: wxcWidget

class DataViewListCtrlColumn : public wxcWidget
{
public:
    DataViewListCtrlColumn();
    virtual ~DataViewListCtrlColumn();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;

    /**
     * @brief return the "real" name as an empty string. This will allow use placing
     * more than one column with a similar name in the designer
     */
    virtual wxString GetRealName() const { return ""; }
};

#endif // DATAVIEWLISTCTRLCOLUMN_H
