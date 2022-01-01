#ifndef CXXEXPRESSION_HPP
#define CXXEXPRESSION_HPP

#include "CxxTokenizer.h"
#include "codelite_exports.h"
#include "entry.h"

#include <vector>
#include <wx/string.h>

using namespace std;

/// a class representing a single expression
/// in a longer chain of expressions
class WXDLLIMPEXP_CL CxxExpression
{
    wxString m_type_name;
    wxArrayString m_scopes; // potential scopes for the type
    int m_operand = 0;
    wxArrayString m_init_list;        // template init list
    wxArrayString m_subscript_params; // subscript operator params
    wxArrayString m_func_call_params; // function params
    size_t m_flags = 0;

public:
    enum eFlags {
        FLAGS_NONE = 0,
        FLAGS_AUTO = (1 << 0),
    };

private:
    static bool parse_template(CxxTokenizer& tokenizer, wxArrayString* init_list);
    static bool parse_subscript(CxxTokenizer& tokenizer, wxArrayString* subscript_param);
    static bool parse_func_call(CxxTokenizer& tokenizer, wxArrayString* func_call_params);
    static bool parse_list(CxxTokenizer& tokenizer, wxArrayString* params, int open_char, int close_char);
    static bool handle_casting(CxxTokenizer& tokenizer, wxString* cast_type);

public:
    CxxExpression();
    ~CxxExpression();

    const wxString& type_name() const { return m_type_name; }
    int operand() const { return m_operand; }
    const wxArrayString& init_list() const { return m_init_list; }
    const wxArrayString& subscript_params() const { return m_subscript_params; }
    const wxArrayString& func_call_params() const { return m_func_call_params; }
    bool is_auto() const { return m_flags & FLAGS_AUTO; }
    static vector<CxxExpression> from_expression(const wxString& expression, CxxExpression* remainder);
};

#endif // CXXEXPRESSION_HPP
