#ifndef FUNCTIONSPARSER_H
#define FUNCTIONSPARSER_H

#include "wxc_widget.h"
#include <cpp_scanner.h>

class FunctionsParser
{
    CppScanner m_scanner;
    wxcWidget::Map_t m_allEvents;
    wxString m_classname;

protected:
    void CheckIfFunctionsExists(const wxString& name);
    int ReadClassName(wxString& clsname);

public:
    FunctionsParser(wxcWidget::Map_t& connectedEvents, const wxString& clsname, const wxString& header);
    virtual ~FunctionsParser();

    const wxcWidget::Map_t& GetAllEvents() const { return m_allEvents; }
    void Parse();
};

#endif // FUNCTIONSPARSER_H
