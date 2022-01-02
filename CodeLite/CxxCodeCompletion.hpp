#ifndef CXXCODECOMPLETION_HPP
#define CXXCODECOMPLETION_HPP

#include "CxxExpression.hpp"
#include "codelite_exports.h"
#include "entry.h"
#include "istorage.h"
#include "macros.h"

#include <vector>
#include <wx/string.h>

using namespace std;

class WXDLLIMPEXP_CL CxxCodeCompletion
{
private:
    struct __local {
    private:
        wxString _type_name;
        wxString _assignment;
        bool _is_auto = false;

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
    };

private:
    ITagsStoragePtr m_lookup;
    const wxStringMap_t* m_tokens_map = nullptr;
    const wxString* m_text = nullptr;
    unordered_map<wxString, __local> m_locals;
    wxString m_optimized_scope;
    vector<pair<wxStringMap_t, CxxExpression*>> m_template_placeholders;

private:
    void reset();
    void prepend_scope(vector<wxString>& scopes, const wxString& scope) const;
    TagEntryPtr lookup_symbol(CxxExpression& curexpr, const vector<wxString>& visible_scopes);
    TagEntryPtr lookup_symbol_by_kind(const wxString& name, const vector<wxString>& visible_scopes,
                                      const vector<wxString>& kinds);
    TagEntryPtr lookup_operator_arrow(TagEntryPtr parent, const vector<wxString>& visible_scopes);

    TagEntryPtr lookup_child_symbol(TagEntryPtr parent, const wxString& child_symbol,
                                    const vector<wxString>& visible_scopes);

    wxString typedef_from_tag(TagEntryPtr tag) const;
    wxString shrink_scope(const wxString& text, unordered_map<wxString, __local>* locals) const;
    TagEntryPtr resolve_expression(CxxExpression& curexp, TagEntryPtr parent, const vector<wxString>& visible_scopes);
    TagEntryPtr resolve_compound_expression(vector<CxxExpression>& expression, const vector<wxString>& visible_scopes);

    const wxStringMap_t& get_tokens_map() const;
    const wxString& get_text() const;
    wxString get_return_value(TagEntryPtr tag) const;
    wxString resolve_placeholder(const wxString& s) const;
    vector<wxString> update_visible_scope(const vector<wxString>& curscopes, TagEntryPtr tag);

public:
    CxxCodeCompletion(ITagsStoragePtr lookup, const wxStringMap_t* tokens_map, const wxString* text);
    ~CxxCodeCompletion();

    TagEntryPtr code_complete(const wxString& expression, const vector<wxString>& visible_scopes);
};

#endif // CXXCODECOMPLETION_HPP
