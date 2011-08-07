#include <wx/init.h>
#include <wx/string.h>

int main( int argc, char** argv )
{
	// initialize wxWidgets
	wxInitializer init;
	wxPrintf( wxT("Hello in wxWidgets World!\n\n") );
	return 0;
}
