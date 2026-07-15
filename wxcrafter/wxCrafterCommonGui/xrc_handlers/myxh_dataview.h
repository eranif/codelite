#ifndef _WX_MYXH_DATAVIEW_H_
#define _WX_MYXH_DATAVIEW_H_

#include <wx/xrc/xmlreshandler.h>

class wxDataViewCtrl;

class MyWxDataViewCtrlHandler : public wxXmlResourceHandler
{
public:
    MyWxDataViewCtrlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;

private:
    // handlers for wxListCtrl itself and its listcol and listitem children
    wxDataViewCtrl* HandleListCtrl();
    void HandleListCol();
};

#endif // _WX_MYXH_DATAVIEW_H_
