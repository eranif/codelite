#ifndef CLPRINTOUT_H
#define CLPRINTOUT_H

#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/prntbase.h>

class LEditor;
class clPrintout : public wxPrintout
{
    LEditor* m_edit;
    int m_printed;
    wxRect m_pageRect;
    wxRect m_printRect;
    int m_minPage;
    int m_maxPage;
    
protected:
    bool PrintScaling(wxDC* dc);

public:
    //! constructor
    clPrintout(LEditor* edit, const wxChar* title = wxT(""));

    //! event handlers
    bool OnPrintPage(int page);
    bool OnBeginDocument(int startPage, int endPage);

    //! print functions
    bool HasPage(int page);
    void GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo);
};

#endif // CLPRINTOUT_H
