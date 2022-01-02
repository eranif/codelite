#include "CxxCodeCompletion.hpp"

#include "CxxExpression.hpp"
#include "CxxScannerTokens.h"
#include "CxxVariableScanner.h"
#include "ctags_manager.h"
#include "function.h"
#include "language.h"

#include <deque>

CxxCodeCompletion::CxxCodeCompletion(ITagsStoragePtr lookup, const wxStringMap_t* tokens_map, const wxString* text)
    : m_lookup(lookup)
    , m_tokens_map(tokens_map)
    , m_text(text)
{
}

CxxCodeCompletion::~CxxCodeCompletion() {}

TagEntryPtr CxxCodeCompletion::code_complete(const wxString& expression, const vector<wxString>& visible_scopes)
{
    // build expression from the expression
    CxxExpression remainder;
    vector<wxString> scopes = { visible_scopes.begin(), visible_scopes.end() };
    vector<CxxExpression> expr_arr = CxxExpression::from_expression(expression, &remainder);
    auto where =
        find_if(visible_scopes.begin(), visible_scopes.end(), [](const wxString& scope) { return scope.empty(); });

    if(where == visible_scopes.end()) {
        // add the global scope
        scopes.push_back(wxEmptyString);
    }

    m_locals.clear();
    m_optimized_scope = shrink_scope(get_text(), &m_locals);
    return resolve_compound_expression(expr_arr, scopes);
}

TagEntryPtr CxxCodeCompletion::resolve_compound_expression(vector<CxxExpression>& expression,
                                                           const vector<wxString>& visible_scopes)
{
    // resolve each expression
    if(expression.empty()) {
        return nullptr;
    }

    TagEntryPtr resolved;
    for(CxxExpression& curexpr : expression) {
        resolved = resolve_expression(curexpr, resolved, visible_scopes);
        CHECK_PTR_RET_NULL(resolved);
    }
    return resolved;
}

wxString CxxCodeCompletion::shrink_scope(const wxString& text, unordered_map<wxString, __local>* locals) const
{
    CxxVariableScanner scanner(text, eCxxStandard::kCxx11, get_tokens_map(), false);
    wxString trimmed_text;
    scanner.OptimizeBuffer(text, trimmed_text);
    CxxVariable::Vec_t variables = scanner.GetVariables(false);
    locals->reserve(variables.size());

    for(auto var : variables) {
        __local local;
        local.set_assignment(var->GetDefaultValue());
        local.set_type_name(var->GetTypeAsString());
        local.set_is_auto(var->IsAuto());
        locals->insert({ var->GetName(), local });
    }
    return trimmed_text;
}

TagEntryPtr CxxCodeCompletion::lookup_operator_arrow(TagEntryPtr parent, const vector<wxString>& visible_scopes)
{
    return lookup_child_symbol(parent, "operator->", visible_scopes);
}

TagEntryPtr CxxCodeCompletion::lookup_child_symbol(TagEntryPtr parent, const wxString& child_symbol,
                                                   const vector<wxString>& visible_scopes)
{
    // to avoid spaces and other stuff that depends on the user typing
    // we tokenize the symbol name
    vector<wxString> requested_symbol_tokens;
    {
        CxxLexerToken tok;
        CxxTokenizer tokenizer;
        tokenizer.Reset(child_symbol);
        while(tokenizer.NextToken(tok)) {
            requested_symbol_tokens.push_back(tok.GetWXString());
        }
    }

    auto compare_tokens_func = [&requested_symbol_tokens](const wxString& symbol_name) -> bool {
        CxxLexerToken tok;
        CxxTokenizer tokenizer;
        tokenizer.Reset(symbol_name);
        for(const wxString& token : requested_symbol_tokens) {
            if(!tokenizer.NextToken(tok)) {
                return false;
            }

            if(tok.GetWXString() != token)
                return false;
        }

        // if we can read more tokens from the tokenizer, it means that
        // the comparison is not complete, it "contains" the requested name
        if(tokenizer.NextToken(tok)) {
            return false;
        }
        return true;
    };

    deque<TagEntryPtr> q;
    q.push_front(parent);
    wxStringSet_t visited;
    while(!q.empty()) {
        auto t = q.front();
        q.pop_front();

        // avoid visiting the same tag twice
        if(!visited.insert(t->GetPath()).second)
            continue;

        vector<TagEntryPtr> tags;
        m_lookup->GetTagsByScope(t->GetPath(), tags);
        for(TagEntryPtr child : tags) {
            if(compare_tokens_func(child->GetName())) {
                return child;
            }
        }

        // if we got here, no match
        wxArrayString inherits = t->GetInheritsAsArrayNoTemplates();
        for(const wxString& inherit : inherits) {
            auto match = lookup_symbol_by_kind(inherit, visible_scopes, { "class", "struct" });
            if(match) {
                q.push_back(match);
            }
        }
    }
    return nullptr;
}

TagEntryPtr CxxCodeCompletion::lookup_symbol_by_kind(const wxString& name, const vector<wxString>& visible_scopes,
                                                     const vector<wxString>& kinds)
{
    vector<TagEntryPtr> tags;
    for(const wxString& scope : visible_scopes) {
        wxString path;
        if(!scope.empty()) {
            path << scope << "::";
        }
        path << name;
        m_lookup->GetTagsByPathAndKind(path, tags, kinds);
        if(tags.size() == 1) {
            // we got a match
            return tags[0];
        }
    }
    return tags.empty() ? nullptr : tags[0];
}

TagEntryPtr CxxCodeCompletion::lookup_symbol(CxxExpression& curexpr, const vector<wxString>& visible_scopes)
{
    wxString name_to_find = curexpr.type_name();
    wxString placeholder_resolved = resolve_placeholder(name_to_find);
    if(!placeholder_resolved.empty()) {
        name_to_find = placeholder_resolved;
    }

    // try classes first
    auto resolved = lookup_symbol_by_kind(name_to_find, visible_scopes,
                                          { "typedef", "class", "struct", "namespace", "enum", "union" });
    if(!resolved) {
        // try methods
        resolved = lookup_symbol_by_kind(name_to_find, visible_scopes, { "function", "prototype" });
    }

    if(resolved) {
        if(curexpr.is_template()) {
            // now that we resolved this expression, we can build our template placeholder list
            curexpr.parse_template_placeholders(resolved->GetPatternClean());
            m_template_placeholders.insert(m_template_placeholders.begin(),
                                           { curexpr.get_template_placeholders_map(), &curexpr });
        }

        // Check for operator-> overloading
        if(curexpr.operand_string() == "->") {
            // search for operator-> overloading
            TagEntryPtr arrow_tag = lookup_operator_arrow(resolved, visible_scopes);
            if(arrow_tag) {
                resolved = arrow_tag;
            }
        }
    }
    return resolved;
}

vector<wxString> CxxCodeCompletion::update_visible_scope(const vector<wxString>& curscopes, TagEntryPtr tag)
{
    vector<wxString> scopes;
    scopes.insert(scopes.end(), curscopes.begin(), curscopes.end());

    if(tag && (tag->IsContainer() || tag->IsTypedef())) {
        prepend_scope(scopes, tag->GetPath());
    } else if(tag && tag->IsMethod()) {
        prepend_scope(scopes, tag->GetScope());
    }
    return scopes;
}

TagEntryPtr CxxCodeCompletion::resolve_expression(CxxExpression& curexp, TagEntryPtr parent,
                                                  const vector<wxString>& visible_scopes)
{
    // test locals first, if its empty, its the first time we are entering here
    if(!parent) {
        if(curexp.operand_string() == "." || curexp.operand_string() == "->") {
            if(m_locals.count(curexp.type_name())) {
                wxString exprstr = m_locals.find(curexp.type_name())->second.type_name() + curexp.operand_string();
                vector<CxxExpression> expr_arr = CxxExpression::from_expression(exprstr, nullptr);
                return resolve_compound_expression(expr_arr, visible_scopes);
            }
        }
    }

    // update the visible scopes
    vector<wxString> scopes = update_visible_scope(visible_scopes, parent);

    auto resolved = lookup_symbol(curexp, scopes);
    CHECK_PTR_RET_NULL(resolved);

    if(resolved->IsContainer()) {
        // nothing more to be done here
        return resolved;
    }

    // continue only if one of 3: member, method or typedef
    if(!resolved->IsMethod() && !resolved->IsMember() && !resolved->IsTypedef()) {
        return nullptr;
    }

    // after we resolved the expression, update the scope
    scopes = update_visible_scope(scopes, resolved);

    if(resolved->IsMethod()) {
        // parse the return value
        wxString new_expr = get_return_value(resolved) + curexp.operand_string();
        vector<CxxExpression> expr_arr = CxxExpression::from_expression(new_expr, nullptr);
        return resolve_compound_expression(expr_arr, scopes);
    } else if(resolved->IsTypedef()) {
        // substitude the type with the typeref
        wxString new_expr = typedef_from_tag(resolved) + curexp.operand_string();
        vector<CxxExpression> expr_arr = CxxExpression::from_expression(new_expr, nullptr);
        return resolve_compound_expression(expr_arr, scopes);
    }
    return nullptr;
}

const wxStringMap_t& CxxCodeCompletion::get_tokens_map() const
{
    static wxStringMap_t empty_map;
    if(m_tokens_map) {
        return *m_tokens_map;
    }
    return empty_map;
}

const wxString& CxxCodeCompletion::get_text() const
{
    static wxString empty_string;
    if(m_text) {
        return *m_text;
    }
    return empty_string;
}

wxString CxxCodeCompletion::get_return_value(TagEntryPtr tag) const
{
    clFunction f;
    if(LanguageST::Get()->FunctionFromPattern(tag, f)) {
        wxString return_value;
        if(!f.m_returnValue.m_typeScope.empty()) {
            return_value << f.m_returnValue.m_typeScope << "::";
        }
        return_value << f.m_returnValue.m_type;
        return return_value;
    }
    return wxEmptyString;
}

void CxxCodeCompletion::prepend_scope(vector<wxString>& scopes, const wxString& scope) const
{
    auto where = find_if(scopes.begin(), scopes.end(), [=](const wxString& s) { return s == scope; });

    if(where != scopes.end()) {
        scopes.erase(where);
    }

    scopes.insert(scopes.begin(), scope);
}

void CxxCodeCompletion::reset()
{
    m_locals.clear();
    m_optimized_scope.clear();
    m_template_placeholders.clear();
}

wxString CxxCodeCompletion::typedef_from_tag(TagEntryPtr tag) const
{
    if(!tag->IsTypedef()) {
        return wxEmptyString;
    }

    wxString pattern = tag->GetPatternClean();
    CxxTokenizer tkzr;
    CxxLexerToken tk;

    tkzr.Reset(pattern);

    wxString typedef_str;
    vector<wxString> V;
    wxString last_identifier;
    if(!tkzr.NextToken(tk)) {
        return wxEmptyString;
    }

    if(tk.GetType() == T_USING) {
        // "using" syntax:
        // using element_type = _Tp;
        // read until the "=";

        // consume everything until we find the "="
        while(tkzr.NextToken(tk)) {
            if(tk.GetType() == '=')
                break;
        }

        // read until the ";"
        while(tkzr.NextToken(tk)) {
            if(tk.GetType() == ';') {
                break;
            }
            if(tk.is_builtin_type()) {
                V.push_back((V.empty() ? "" : " ") + tk.GetWXString() + " ");
            } else {
                V.push_back(tk.GetWXString());
            }
        }

    } else if(tk.GetType() == T_TYPEDEF) {
        // standard "typedef" syntax:
        // typedef wxString MyString;
        while(tkzr.NextToken(tk)) {
            if(tk.is_keyword()) {
                continue;
            }
            if(tk.GetType() == ';') {
                // end of typedef
                if(!V.empty()) {
                    V.pop_back();
                }
                break;
            } else {
                if(tk.is_builtin_type()) {
                    V.push_back((V.empty() ? "" : " ") + tk.GetWXString() + " ");
                } else {
                    V.push_back(tk.GetWXString());
                }
            }
        }
    }

    if(V.empty()) {
        return wxEmptyString;
    }

    for(const wxString& s : V) {
        typedef_str << s;
    }
    return typedef_str.Trim();
}

wxString CxxCodeCompletion::resolve_placeholder(const wxString& s) const
{
    for(size_t i = 0; i < m_template_placeholders.size(); ++i) {
        if(m_template_placeholders[i].first.count(s)) {
            return m_template_placeholders[i].first.find(s)->second;
        }
    }
    return wxEmptyString;
}
