#include <wx/init.h>
#include "CodeLiteLLDBApp.h"

int main(int argc, char** argv)
{
    wxInitializer intializer(argc, argv);
    
    if ( argc < 2 ) {
        printf("Usage: codelite-lldb <unique-string>\n");
        return -1;
    }
    wxString strSocketPath;
    strSocketPath << "/tmp/codelite-lldb." << argv[1] << ".sock";
    
    CodeLiteLLDBApp app(strSocketPath);
    app.MainLoop();
    return 0;
}
