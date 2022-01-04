#ifndef CXXCODECOMPLETION_HPP
#define CXXCODECOMPLETION_HPP

#include "CxxExpression.hpp"
#include "codelite_exports.h"
#include "entry.h"
#include "istorage.h"
#include "macros.h"

#include <memory>
#include <vector>
#include <wx/string.h>

using namespace std;

class CxxCodeCompletion;

struct TemplateManager {
    CxxCodeCompletion* m_completer = nullptr;
    vector<wxStringMap_t> m_table;

    TemplateManager(CxxCodeCompletion* completer)
        : m_completer(completer)
    {
    }
    void clear();
    void add_placeholders(const wxStringMap_t& table, const vector<wxString>& visible_scopes);
    wxString resolve(const wxString& name, const vector<wxString>& visible_scopes) const;
    typedef shared_ptr<TemplateManager> ptr_t;
};

class WXDLLIMPEXP_CL CxxCodeCompletion
{
    friend class TemplateManager;

private:
    struct __local {
    private:
        wxString _type_name;
        wxString _assignment;
        wxString _name;
        bool _is_auto = false;
        wxString _pattern;

    public:
        bool is_auto() const { return _is_auto; }
        const wxString& type_name() const
        {
            if(is_auto()) {
                return _assignment;
            } else {
                return _type_name;
            }
        }

        void set_type_name(const wxString& s) { _type_name = s; }
        void set_assignment(const wxString& s) { _assignment = s; }
        void set_is_auto(bool b) { _is_auto = b; }
        void set_name(const wxString& name) { _name = name; }
        const wxString& name() const { return _name; }

        void set_pattern(const wxString& pattern) { _pattern = pattern; }
        const wxString& pattern() const { return _pattern; }
    };

private:
    ITagsStoragePtr m_lookup;
    unordered_map<wxString, __local> m_locals;
    wxString m_optimized_scope;
    wxString m_filename;
    int m_line_number = 0;
    wxString m_current_scope_name;
    size_t m_recurse_protector = 0;
    bool m_text_parsed = false;
    wxStringMap_t m_types_table; // helper table to solve what we cant (usually the limitations are coming from ctags)
    wxStringMap_t m_macros_table;
    TemplateManager::ptr_t m_template_manager;

private:
    void prepend_scope(vector<wxString>& scopes, const wxString& scope) const;
    TagEntryPtr lookup_symbol(CxxExpression& curexpr, const vector<wxString>& visible_scopes, TagEntryPtr parent);
    TagEntryPtr lookup_symbol_by_kind(const wxString& name, const vector<wxString>& visible_scopes,
                                      const vector<wxString>& kinds);
    TagEntryPtr lookup_operator_arrow(TagEntryPtr parent, const vector<wxString>& visible_scopes);

    TagEntryPtr lookup_child_symbol(TagEntryPtr parent, const wxString& child_symbol,
                                    const vector<wxString>& visible_scopes, const vector<wxString>& kinds);

    wxString typedef_from_pattern(const wxString& pattern) const;
    wxString shrink_scope(const wxString& text, unordered_map<wxString, __local>* locals) const;
    TagEntryPtr resolve_expression(CxxExpression& curexp, TagEntryPtr parent, const vector<wxString>& visible_scopes);
    TagEntryPtr resolve_compound_expression(vector<CxxExpression>& expression, const vector<wxString>& visible_scopes);

    const wxStringMap_t& get_tokens_map() const;
    wxString get_return_value(TagEntryPtr tag) const;

    /**
     * @brief apply user hacks
     */
    bool resolve_user_type(const wxString& type, const vector<wxString>& visible_scopes, wxString* resolved) const;

    vector<wxString> update_visible_scope(const vector<wxString>& curscopes, TagEntryPtr tag);
    void update_template_table(TagEntryPtr resolved, CxxExpression& curexpr, const vector<wxString>& visible_scopes,
                               wxStringSet_t& visited);

    /**
     * @brief based on the file + line number, determine the current scope name
     */
    void determine_current_scope();

    size_t parse_locals(const wxString& text, unordered_map<wxString, __local>* locals) const;

    /**
     * @brief given `parent` node, return list of all its parents, in order
     * of inheritance. the list contains the `parent` itself as the first entry
     */
    vector<TagEntryPtr> get_scopes(TagEntryPtr parent, const vector<wxString>& visible_scopes);

    vector<TagEntryPtr> get_children_of_scope(TagEntryPtr parent, const vector<wxString>& kinds);

public:
    typedef shared_ptr<CxxCodeCompletion> ptr_t;

public:
    CxxCodeCompletion(ITagsStoragePtr lookup);
    ~CxxCodeCompletion();

    /**
     * @brief reset the completer (clear all cached data)
     */
    void reset();

    /**
     * @brief set the typedef helper table
     */
    void set_types_table(const wxStringMap_t& t) { m_types_table = t; }

    /**
     * @brief set macros table
     */
    void set_macros_table(const wxStringMap_t& t) { m_macros_table = t; }

    /**
     * @brief replace the text
     * this method resets the completer
     */
    void set_text(const wxString& text, const wxString& filename, int current_line);

    /**
     * @brief parse an expression and resolve it into a TagEntry. If the expression does not end with
     * an operand, return the remainder
     * @param expression
     * @param visible_scopes [in] addition visible scopes (e.g. "std")
     * @param remainder [output] holds the last part that was not used. `a.foo().bar().get` -> the remainder will hold
     * `get`
     * @return a tag representing the scope
     */
    TagEntryPtr code_complete(const wxString& expression, const vector<wxString>& visible_scopes,
                              CxxExpression* remainder = nullptr);

    /**
     * @brief return the local variables available for the current scope (passed in the c-tor)
     */
    vector<TagEntryPtr> get_locals();

    /**
     * @brief return list of completions filtered by name for a given parent
     */
    size_t get_completions(TagEntryPtr parent, const wxString& filter, vector<TagEntryPtr>& candidates,
                           const vector<wxString>& visible_scopes, size_t limit = (size_t)-1);
};

#endif // CXXCODECOMPLETION_HPP
