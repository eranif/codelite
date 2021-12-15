#include <wx/string.h>

const wxString sample_cxx_file = R"(
class MyClass {
    std::string m_name;
    
public:
    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
};

int main(int argc, char** argv)
{
    std::string name = "hello";
    MyClass cls, cls2;
    cls.SetName(name);
    return 0;
}
)";

const wxString sample_text_language_h = R"(class TemplateHelper
{
    std::vector<wxArrayString> templateInstantiationVector;
};
)";

const wxString sample_text = R"(
#ifdef __WXMSW__
#endif
m_string.)";

wxString file_content =
    R"(
using namespace std;
void foo() {
    vector<string> V;
    V.empty();
    // other content goes here
    // bla bla
)";

const wxString tokenizer_sample_file_0 = R"(#include <unistd.h>
/** comment **/
int main(int argc, char** argv)
{
    /**
        multi line comment
        
    **/
    std::string name = "hello\nworld";
    MyClass cls, 1cls2;
    cls.SetName(name);
    return 0;
}
//last line comment
//)";

const wxString tokenizer_sample_file_1 = R"(#include <unistd.h>
/** comment **/
int main(int argc, char** argv)
{
    /**
        multi line comment
        
    **/
    std::string name = "hello\nworld";
    MyClass cls, 1cls2;
    cls.SetName(name);
    return 0;
}
)";

const wxString tokenizer_sample_file_2 = R"(#include "TextView.h"
#include "wxTerminalColourHandler.h"
#include "wxTerminalCtrl.h"

#ifdef __WXMSW__
#include "wx/msw/wrapwin.h" // includes windows.h
#endif
#include <fileutils.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>

wxTerminalColourHandler::wxTerminalColourHandler()
{
    // we use the Ubuntu colour scheme
    // Text colours
    m_colours.insert({ 30, wxColour(1, 1, 1) });
)";
