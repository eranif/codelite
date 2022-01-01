#include "CxxCodeCompletion.hpp"

#include "CxxExpression.hpp"
#include "CxxScannerTokens.h"
#include "CxxVariableScanner.h"
#include "ctags_manager.h"
#include "function.h"
#include "language.h"

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

TagEntryPtr CxxCodeCompletion::resolve_compound_expression(const vector<CxxExpression>& expression,
                                                           const vector<wxString>& visible_scopes) const
{
    // resolve each expression
    if(expression.empty()) {
        return nullptr;
    }

    TagEntryPtr resolved;
    for(const CxxExpression& curexpr : expression) {
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

TagEntryPtr CxxCodeCompletion::find_by_name_and_path_and_kind(const wxString& name,
                                                              const vector<wxString>& visible_scopes,
                                                              const vector<wxString>& kinds) const
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

TagEntryPtr CxxCodeCompletion::find_by_name_and_path(const wxString& name, const vector<wxString>& visible_scopes) const
{
    // try classes first
    auto tag =
        find_by_name_and_path_and_kind(name, visible_scopes, { "class", "struct", "namespace", "enum", "union" });
    if(!tag) {
        // try methods
        return find_by_name_and_path_and_kind(name, visible_scopes, { "function", "prototype" });
    }
    return tag;
}

TagEntryPtr CxxCodeCompletion::resolve_expression(const CxxExpression& curexp, TagEntryPtr parent,
                                                  const vector<wxString>& visible_scopes) const
{
    // test locals first, if its empty, its the first time we are entering here
    if(!parent) {
        if(curexp.operand() == '.' || curexp.operand() == T_ARROW) {
            if(m_locals.count(curexp.type_name())) {
                wxString exprstr =
                    m_locals.find(curexp.type_name())->second.type_name() + (wxString() << (char)curexp.operand());
                vector<CxxExpression> expr_arr = CxxExpression::from_expression(exprstr, nullptr);
                return resolve_compound_expression(expr_arr, visible_scopes);
            }
        }
    }

    // update the visible scopes
    vector<wxString> scopes;
    if(parent && parent->IsContainer()) {
        prepend_scope(scopes, parent->GetPath());
    } else if(parent && parent->IsMethod()) {
        prepend_scope(scopes, parent->GetScope());
    }

    scopes.insert(scopes.end(), visible_scopes.begin(), visible_scopes.end());
    auto resolved = find_by_name_and_path(curexp.type_name(), scopes);
    CHECK_PTR_RET_NULL(resolved);

    if(resolved->IsContainer()) {
        // nothing more to be done here
        return resolved;
    }

    // continue only if one of 3: member, method or typedef
    if(!resolved->IsMethod() && !resolved->IsMember() && !resolved->IsTypedef()) {
        return nullptr;
    }

    if(resolved->IsMethod()) {
        // parse the return value
        wxString new_expr = get_return_value(resolved) + (wxString() << (char)curexp.operand());
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
}
