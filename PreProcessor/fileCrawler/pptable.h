#ifndef PPTABLE_H
#define PPTABLE_H

#include <wx/arrstr.h>
#include <map>
#include <wx/ffile.h>
#include <vector>

/**
 * @brief perform search and replace using CL pattern
 * an example:
 * pattern=wx_dynamic_cast(%0, %1)
 * replacement=dynamic_cast<%0>(%1)
 * in=wx_dynamic_cast(wxApp*, ptr)->OnInit();
 *
 * the expected result is:
 * dynamic_cast<wxApp*>(ptr)->OnInit()
 *
 * It also supports simple search and replace
 */
wxString CLReplacePattern(const wxString &in, const wxString& pattern, const wxString& replacement);

/**
 * @brief perform search and replace using CL pattern
 * an example:
 * pattern=wx_dynamic_cast(%0, %1)
 * replacement=dynamic_cast<%0>(%1)
 * in=wx_dynamic_cast(wxApp*, ptr)->OnInit();
 *
 * the expected result is:
 * dynamic_cast<wxApp*>(ptr)->OnInit()
 *
 * It also supports simple search and replace
 */
std::string CLReplacePatternA(const std::string& in, const std::string& pattern, const std::string& replacement);

struct PPToken {
    enum {
        IsFunctionLike = 0x00000001,
        IsValid        = 0x00000002,
        IsOverridable  = 0x00000004
    };

    int             line;           // line where found
    wxString        name;           // preprocessor name
    wxString        replacement;    // un processed replacement
    wxArrayString   args;           // for function like macros, contains the argument's names
    size_t          flags;          // PP token flags

    PPToken() : line(0), flags(IsOverridable)
    {}

    ~PPToken()
    {}

    void expandOnce(const wxArrayString& initList);
    void processArgs(const wxString &argsList);
    void print(wxFFile &fp);
    static bool readInitList(const wxString &in, int from, wxString& initList, wxArrayString &initListArr);
    static bool readInitList(const std::string &in, size_t from, std::string& initList, std::vector<std::string> &initListArr);
    wxString fullname();
    void squeeze();
};

class PPTable
{

    static PPTable*             ms_instance;
    std::map<wxString, PPToken> m_table;

public:
    static PPTable* Instance();
    static void Release();

private:
    PPTable();
    ~PPTable();

public:
    PPToken   Token(const wxString &name);
    bool      Contains(const wxString &name);
    void      Add  (const PPToken& token);
    void      Print(wxFFile &fp);
    wxString  Export();
    void      Clear();
    void      Squeeze();
};
#endif // PPTABLE_H
