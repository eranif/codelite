#pragma once

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
