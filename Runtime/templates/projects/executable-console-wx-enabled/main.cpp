#include <wx/init.h>

#if wxVERSION_NUMBER < 2900
    #include <wx/string.h>
#else
    #include <wx/crt.h>
#endif

int main( int argc, char** argv )
{
	// initialize wxWidgets
	wxInitializer init;
	wxPrintf( wxT("Hello in wxWidgets World!\n\n") );
	return 0;
}
