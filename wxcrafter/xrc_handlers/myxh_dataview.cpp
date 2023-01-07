#include "myxh_dataview.h"
#include <wx/dataview.h>
#include <wx/imaglist.h>
#include <wx/tokenzr.h>

MyWxDataViewCtrlHandler::MyWxDataViewCtrlHandler()
    : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxDV_SINGLE);
    XRC_ADD_STYLE(wxDV_MULTIPLE);
    XRC_ADD_STYLE(wxDV_ROW_LINES);
    XRC_ADD_STYLE(wxDV_HORIZ_RULES);
    XRC_ADD_STYLE(wxDV_VERT_RULES);
    XRC_ADD_STYLE(wxDV_VARIABLE_LINE_HEIGHT);
    XRC_ADD_STYLE(wxDV_NO_HEADER);
    AddWindowStyles();
}

wxObject* MyWxDataViewCtrlHandler::DoCreateResource()
{
    if(m_class == "wxDataViewColumn") {
        HandleListCol();

    } else {
        wxASSERT_MSG(m_class == "wxDataViewCtrl", wxT("can't handle unknown node"));

        return HandleListCtrl();
    }

    return m_parentAsWindow;
}

bool MyWxDataViewCtrlHandler::CanHandle(wxXmlNode* node)
{
    return IsOfClass(node, "wxDataViewCtrl") || IsOfClass(node, "wxDataViewColumn");
}

void MyWxDataViewCtrlHandler::HandleListCol()
{
    // Add column to the wxDataViewCtrl
    wxDataViewCtrl* const list = wxDynamicCast(m_parentAsWindow, wxDataViewCtrl);
    wxCHECK_RET(list, wxT("must have wxDataViewCtrl parent"));

    if(!HasParam(wxT("coltype"))) { return; }

    // Column properties
    wxString coltype = GetText("coltype");
    int colwidth = GetLong("width", -1);
    wxString label = GetText("label");
    wxString salign = GetText("align", false);
    int style = GetLong("style", 0);
    wxString cellmode = GetText("cellmode", false);

    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT;
    if(cellmode == "wxDATAVIEW_CELL_ACTIVATABLE")
        mode = wxDATAVIEW_CELL_ACTIVATABLE;
    else if(cellmode == "")
        mode = wxDATAVIEW_CELL_EDITABLE;

    wxAlignment al = wxALIGN_LEFT;
    if(salign == "wxALIGN_RIGHT")
        al = wxALIGN_RIGHT;
    else if(salign == "wxALIGN_CENTER")
        al = wxALIGN_CENTER;

    if(coltype == "bitmap") {
        list->AppendBitmapColumn(label, list->GetColumnCount(), mode, colwidth, al, style);

    } else if(coltype == "check") {
        list->AppendToggleColumn(label, list->GetColumnCount(), mode, colwidth, al, style);

    } else if(coltype == "text") {
        list->AppendTextColumn(label, list->GetColumnCount(), mode, colwidth, al, style);

    } else if(coltype == "icontext") {
        list->AppendIconTextColumn(label, list->GetColumnCount(), mode, colwidth, al, style);

    } else if(coltype == "progress") {
        list->AppendProgressColumn(label, list->GetColumnCount(), mode, colwidth, al, style);

    } else if(coltype == "choice") {
        wxString content = GetText("choices", false);
        wxArrayString choices = ::wxStringTokenize(content, ",", wxTOKEN_STRTOK);
        list->AppendColumn(new wxDataViewColumn(label,
                                                new wxDataViewChoiceRenderer(choices, mode, wxDVR_DEFAULT_ALIGNMENT),
                                                list->GetColumnCount(), colwidth, al, style));
    }
}

wxDataViewCtrl* MyWxDataViewCtrlHandler::HandleListCtrl()
{
    XRC_MAKE_INSTANCE(list, wxDataViewCtrl)

    list->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle());
    list->SetName(GetName());
    CreateChildrenPrivately(list);
    SetupWindow(list);
    return list;
}
