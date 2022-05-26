#ifndef CXXEXPRESSION_HPP
#define CXXEXPRESSION_HPP

#include "CxxTokenizer.h"
#include "codelite_exports.h"
#include "entry.h"
#include "macros.h"

#include <vector>
#include <wx/string.h>

/// A remainder is the part of an expression chain that was not processed.
/// Consider the following simple expression:
///
/// std::string::size_
///
/// Passing the above to CxxExpression::from_expression
/// and assuming that the it succeed, the mainder will be:
/// filter -> size_
/// operand_string -> "::"
struct WXDLLIMPEXP_CL CxxRemainder {
    wxString filter;
    wxString operand_string;
};

/// a class representing a single expression
/// in a longer chain of expressions
class WXDLLIMPEXP_CL CxxExpression
{
private:
    wxString m_type_name;
    wxArrayString m_scopes; // potential scopes for the type
    int m_operand = 0;
    wxString m_operand_string;                     // string representation for the operand
    wxArrayString m_template_init_list;            // template init list vector<wxString> -> `wxString`
    wxArrayString m_template_placeholder_list;     // template placeholders (e.g. `T`)
    std::vector<wxArrayString> m_subscript_params; // subscript operator params
    wxArrayString m_func_call_params;              // function params
    size_t m_flags = 0;

public:
    enum eFlags {
        FLAGS_NONE = 0,
        FLAGS_AUTO = (1 << 0),
        FLAGS_THIS = (1 << 1),
    };

private:
    static bool parse_template(CxxTokenizer& tokenizer, wxArrayString* init_list);
    static bool parse_subscript(CxxTokenizer& tokenizer, wxArrayString* subscript_param);
    static bool parse_func_call(CxxTokenizer& tokenizer, wxArrayString* func_call_params);
    static bool parse_list(CxxTokenizer& tokenizer, wxArrayString* params, int open_char, int close_char);
    static bool handle_cxx_casting(CxxTokenizer& tokenizer, wxString* cast_type);

public:
    CxxExpression();
    ~CxxExpression();

    bool ok() const { return !m_type_name.empty() || m_operand != 0; }

    /**
     * set the operand for this expression
     */
    void set_operand(int op);

    const wxString& type_name() const { return m_type_name; }
    wxString& type_name() { return m_type_name; }

    wxString template_placeholder_to_type(const wxString& placeholder) const;

    /**
     * @brief given a template definition line, parse and build the template placeholders list
     * e.g. `template<typename Key, typename Value> class A {...}` -> we build the placeholders list `{"Key", "Value"}`
     */
    void parse_template_placeholders(const wxString& expr);

    /**
     * @brief reutn template placeholders map
     */
    wxStringMap_t get_template_placeholders_map() const;
    /**
     * @brief is this expression represents a template one?
     */
    bool is_template() const { return !m_template_init_list.empty(); }

    const wxString& operand_string() const;
    const wxArrayString& template_init_list() const { return m_template_init_list; }
    const std::vector<wxArrayString>& subscript_params() const { return m_subscript_params; }
    void set_subscript_params(const std::vector<wxArrayString>& params);
    const wxArrayString& func_call_params() const { return m_func_call_params; }
    bool is_auto() const { return m_flags & FLAGS_AUTO; }
    bool is_this() const { return m_flags & FLAGS_THIS; }

    /// example `subclass_pattern`:
    /// `class ContextManager : public Singleton<ContextManager>, OtherClass, SecondClass<wxString, wxArrayString>`
    /// returns:
    /// {"Singleton<ContextManager>", "OtherClass", "SecondClass<wxString, wxArrayString>"}
    static std::vector<wxString> split_subclass_expression(const wxString& subclass_pattern);
    static std::vector<CxxExpression> from_expression(const wxString& expression, CxxRemainder* remainder);

    /**
     * should the code completion check for subscript operator overloading?
     */
    bool check_subscript_operator() const { return !m_subscript_params.empty(); }
    void pop_subscript_operator()
    {
        if(!m_subscript_params.empty()) {
            m_subscript_params.erase(m_subscript_params.begin());
        }
    }
};

#endif // CXXEXPRESSION_HPP
