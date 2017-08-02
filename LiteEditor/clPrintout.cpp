#include "clPrintout.h"
#include "cl_editor.h"

//----------------------------------------------------------------------------
// clPrintout
//----------------------------------------------------------------------------
extern wxPageSetupDialogData* g_pageSetupData;

clPrintout::clPrintout(LEditor* edit, const wxString& title)
    : wxPrintout(title)
    , m_edit(edit)
    , m_minPage(0)
    , m_maxPage(0)
{
}

bool clPrintout::OnPrintPage(int page)
{

    wxDC* dc = GetDC();
    if(!dc) return false;

    // scale DC
    PrintScaling(dc);

    // print page
    m_edit->FormatRange(true, page == 1 ? 0 : m_pageEnds[page-2], m_pageEnds[page-1],
                                      dc, dc, m_printRect, m_pageRect);

    return true;
}

bool clPrintout::OnBeginDocument(int startPage, int endPage)
{

    if(!wxPrintout::OnBeginDocument(startPage, endPage)) {
        return false;
    }

    return true;
}

void clPrintout::GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo)
{
    // initialize values
    *minPage = 0;
    *maxPage = 0;
    *selPageFrom = 0;
    *selPageTo = 0;

    // scale DC if possible
    wxDC* dc = GetDC();
    if(!dc) return;
    PrintScaling(dc);

    // get print page informations and convert to printer pixels
    wxSize ppiScr;
    GetPPIScreen(&ppiScr.x, &ppiScr.y);
    wxSize page = g_pageSetupData->GetPaperSize();
    page.x = static_cast<int>(page.x * ppiScr.x / 25.4);
    page.y = static_cast<int>(page.y * ppiScr.y / 25.4);
    // In landscape mode we need to swap the width and height
    if ( g_pageSetupData->GetPrintData().GetOrientation() == wxLANDSCAPE ) {
        wxSwap(page.x, page.y);
    }
    m_pageRect = wxRect(0, 0, page.x, page.y);

    // get margins informations and convert to printer pixels
    wxPoint pt = g_pageSetupData->GetMarginTopLeft();
    int left = pt.x;
    int top = pt.y;
    pt = g_pageSetupData->GetMarginBottomRight();
    int right = pt.x;
    int bottom = pt.y;

    top = static_cast<int>(top * ppiScr.y / 25.4);
    bottom = static_cast<int>(bottom * ppiScr.y / 25.4);
    left = static_cast<int>(left * ppiScr.x / 25.4);
    right = static_cast<int>(right * ppiScr.x / 25.4);

    m_printRect = wxRect(left, top, page.x - (left + right), page.y - (top + bottom));

    // count pages
    m_pageEnds.Clear();
    int printed = 0;
    while ( printed < m_edit->GetLength() ) {
        printed = m_edit->FormatRange(false, printed, m_edit->GetLength(),
                                      dc, dc, m_printRect, m_pageRect);
        m_pageEnds.Add(printed);
        *maxPage += 1;
    }
    
    if(*maxPage > 0) *minPage = 1;
    *selPageFrom = *minPage;
    *selPageTo = *maxPage;

    m_minPage = *minPage;
    m_maxPage = *maxPage;
}

bool clPrintout::HasPage(int page) {  return page <= (int)m_pageEnds.Count(); }

bool clPrintout::PrintScaling(wxDC* dc)
{

    // check for dc, return if none
    if(!dc) return false;

    // get printer and screen sizing values
    wxSize ppiScr;
    GetPPIScreen(&ppiScr.x, &ppiScr.y);
    if(ppiScr.x == 0) { // most possible guess 96 dpi
        ppiScr.x = 96;
        ppiScr.y = 96;
    }
    wxSize ppiPrt;
    GetPPIPrinter(&ppiPrt.x, &ppiPrt.y);
    if(ppiPrt.x == 0) { // scaling factor to 1
        ppiPrt.x = ppiScr.x;
        ppiPrt.y = ppiScr.y;
    }
    wxSize dcSize = dc->GetSize();
    wxSize pageSize;
    GetPageSizePixels(&pageSize.x, &pageSize.y);

    // set user scale
    float scale_x = (float)(ppiPrt.x * dcSize.x) / (float)(ppiScr.x * pageSize.x);
    float scale_y = (float)(ppiPrt.y * dcSize.y) / (float)(ppiScr.y * pageSize.y);
    dc->SetUserScale(scale_x, scale_y);

    return true;
}
