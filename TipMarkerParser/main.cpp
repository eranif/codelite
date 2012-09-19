#include <stdio.h>
#include <wx/init.h>
#include <wx/ffile.h>
#include "Markup.h"

int main(int argc, char **argv)
{
    wxInitializer init;
    wxFFile fp("../test.h", "rb");
    wxString fileContent;
    fp.ReadAll(&fileContent, wxConvUTF8);
    fp.Close();

    ::setMarkupLexerInput(fileContent);
    wxString text;
    while ( true ) {
        int type = ::MarkupLex();
        if ( !type )
            break;

        switch ( type ) {
        case BOLD_START:
        case BOLD_END:
        case HORIZONTAL_LINE:
		case COLOR_START:
		case COLOR_END:
            if ( text.IsEmpty() == false ) {
                wxPrintf(wxT("%s"), text.c_str());
                text.Clear();
            }
            wxPrintf(wxT("=%s="), ::MarkupText().c_str());
            break;
        case NEW_LINE:
            wxPrintf(wxT("%s"), text.c_str());
            text.Clear();
        default:
            text << ::MarkupText();
            break;
        }
    }

    if ( text.IsEmpty() == false ) {
        wxPrintf(wxT("%s"), text.c_str());
        text.Clear();
    }
    return 0;
}
