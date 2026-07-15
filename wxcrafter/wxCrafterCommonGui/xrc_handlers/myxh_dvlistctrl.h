#ifndef _WX_MYXH_DVLISTC_H_
#define _WX_MYXH_DVLISTC_H_

#include <wx/xrc/xmlreshandler.h>

class wxDataViewListCtrl;

class MyWxDataViewListCtrlHandler : public wxXmlResourceHandler
{
public:
    MyWxDataViewListCtrlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;

private:
    // handlers for wxListCtrl itself and its listcol and listitem children
    wxDataViewListCtrl* HandleListCtrl();
    void HandleListCol();
};

#endif // _WX_XH_LISTC_H_
