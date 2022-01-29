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

/// internal class used by the code completion
/// it wraps the real lookup table + unit testing
/// in-memory table
struct LookupTable {
    ITagsStoragePtr pdb;                           // the real database
    unordered_map<wxString, TagEntryPtr> tests_db; // test database, for unit testings
    explicit LookupTable(ITagsStoragePtr real_db, const unordered_map<wxString, TagEntryPtr>& unit_tests_db)
    {
        pdb = real_db;
        tests_db = unit_tests_db;
    }
    explicit LookupTable(ITagsStoragePtr real_db) { pdb = real_db; }
    ~LookupTable()
    {
        pdb.Reset(nullptr);
        tests_db.clear();
    }

    void set_tests_db(unordered_map<wxString, TagEntryPtr>& db) { this->tests_db.swap(db); }
    void clear_tests_db() { this->tests_db.clear(); }

    typedef shared_ptr<LookupTable> ptr_t;

    TagEntryPtr GetScope(const wxString& filename, int line_number);
    void GetTagsByPath(const wxString& path, vector<TagEntryPtr>& tags, int limit = 1);
    void GetSubscriptOperator(const wxString& scope, vector<TagEntryPtr>& tags);
    void GetTagsByPathAndKind(const wxString& path, vector<TagEntryPtr>& tags, const vector<wxString>& kinds,
                              int limit = 1);
    void GetTagsByScopeAndKind(const wxString& scope, const wxArrayString& kinds, const wxString& filter,
                               vector<TagEntryPtr>& tags, bool applyLimit = true);
    void GetFilesForCC(const wxString& userTyped, wxArrayString& matches);
    void GetTagsByScopeAndName(const wxString& scope, const wxString& name, bool partialNameAllowed,
                               vector<TagEntryPtr>& tags);
    void GetTagsByScopeAndName(const wxArrayString& scope, const wxString& name, bool partialNameAllowed,
                               vector<TagEntryPtr>& tags);
    size_t GetFileScopedTags(const wxString& filepath, const wxString& name, const wxArrayString& kinds,
                             vector<TagEntryPtr>& tags);
    void GetTagsByScope(const wxString& scope, vector<TagEntryPtr>& tags);
};

// internal to this TU
struct FileScope {
private:
    unordered_map<wxString, TagEntryPtr> static_members; // static members (`string Foo::m_str`)
    vector<wxString> local_scopes;

public:
    void clear()
    {
        local_scopes.clear();
        static_members.clear();
    }

    void add_static_member(TagEntryPtr tag)
    {
        if(static_members.count(tag->GetName())) {
            static_members.erase(tag->GetName());
        }
        static_members.insert({ tag->GetName(), tag });
    }

    bool is_static_member(const wxString& name) const { return static_members.count(name); }
    TagEntryPtr get_static_member(const wxString& name) const
    {
        if(is_static_member(name)) {
            return static_members.find(name)->second;
        }
        return nullptr;
    }

    const vector<wxString>& get_file_scopes() const { return local_scopes; }
    void set_file_scopes(const wxStringSet_t& scopes)
    {
        local_scopes.clear();
        local_scopes.insert(local_scopes.end(), scopes.begin(), scopes.end());
    }
};

class WXDLLIMPEXP_CL CxxCodeCompletion
{
    friend class TemplateManager;

private:
    struct __local {
    private:
        wxString _type_name;
        wxString _assignment;
        wxString _assignment_raw;
        wxString _name;
        bool _is_auto = false;
        wxString _pattern;
        int _line_numner = wxNOT_FOUND;

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
        void set_assignment_raw(const wxString& s) { _assignment_raw = s; }
        void set_is_auto(bool b) { _is_auto = b; }
        void set_name(const wxString& name) { _name = name; }
        const wxString& name() const { return _name; }

        void set_pattern(const wxString& pattern) { _pattern = pattern; }
        const wxString& pattern() const { return _pattern; }

        int line_number() const { return _line_numner; }
        void set_line_number(int l) { _line_numner = l; }
    };

private:
    LookupTable::ptr_t m_lookup;
    unordered_map<wxString, CxxCodeCompletion::__local> m_locals;
    FileScope m_file_only_tags;
    wxString m_filename;
    int m_line_number = 0;
    TagEntryPtr m_current_function_tag;
    TagEntryPtr m_current_container_tag;
    size_t m_recurse_protector = 0;
    bool m_text_parsed = false;
    vector<pair<wxString, wxString>>
        m_types_table; // helper table to solve what we cant (usually the limitations are coming from ctags)
    wxStringMap_t m_macros_table_map;
    vector<pair<wxString, wxString>> m_macros_table;
    TemplateManager::ptr_t m_template_manager;
    bool m_first_time = true;
    wxString m_codelite_indexer;

private:
    /**
     * @brief prepend scope to the list of scopes. Place it first
     */
    void prepend_scope(vector<wxString>& scopes, const wxString& scope) const;
    /**
     * @brief prepend the file specific scopes + global scope (if missing) to the requested visible scopes
     */
    vector<wxString> prepend_extra_scopes(const vector<wxString>& visible_scopes);

    TagEntryPtr lookup_symbol(CxxExpression& curexpr, const vector<wxString>& visible_scopes, TagEntryPtr parent);
    TagEntryPtr lookup_symbol_by_kind(const wxString& name, const vector<wxString>& visible_scopes,
                                      const vector<wxString>& kinds);
    TagEntryPtr lookup_operator_arrow(TagEntryPtr parent, const vector<wxString>& visible_scopes);
    TagEntryPtr lookup_subscript_operator(TagEntryPtr parent, const vector<wxString>& visible_scopes);

    TagEntryPtr lookup_child_symbol(TagEntryPtr parent, TemplateManager::ptr_t template_manager,
                                    const wxString& child_symbol, const vector<wxString>& visible_scopes,
                                    const vector<wxString>& kinds);

    wxString typedef_from_tag(TagEntryPtr tag) const;
    void shrink_scope(const wxString& text, unordered_map<wxString, __local>* locals, FileScope* file_tags) const;
    TagEntryPtr resolve_expression(CxxExpression& curexp, TagEntryPtr parent, const vector<wxString>& visible_scopes);
    TagEntryPtr resolve_compound_expression(vector<CxxExpression>& expression, const vector<wxString>& visible_scopes,
                                            const CxxExpression& orig_expression);

    const wxStringMap_t& get_tokens_map() const;

    wxString do_get_return_value(const wxString& pattern, const wxString& name) const;

    bool read_template_definition(CxxTokenizer& tokenizer, wxString* definition) const;

    /**
     * @brief apply user hacks
     */
    bool resolve_user_type(const wxString& type, const vector<wxString>& visible_scopes, wxString* resolved) const;

    vector<wxString> update_visible_scope(const vector<wxString>& curscopes, TagEntryPtr tag);
    void update_template_table(TagEntryPtr resolved, CxxExpression& curexpr, const vector<wxString>& visible_scopes,
                               wxStringSet_t& visited);

    size_t parse_locals(const wxString& text, unordered_map<wxString, __local>* locals) const;

    /**
     * @brief given `parent` node, return list of all its parents, in order
     * of inheritance. the list contains the `parent` itself as the first entry
     */
    vector<TagEntryPtr> get_scopes(TagEntryPtr parent, const vector<wxString>& visible_scopes);

    vector<TagEntryPtr> get_children_of_scope(TagEntryPtr parent, const vector<wxString>& kinds, const wxString& filter,
                                              const vector<wxString>& visible_scopes);

    /**
     * @brief return parsable pattern from a tag
     */
    wxString normalize_pattern(TagEntryPtr tag) const;

    size_t get_anonymous_tags(const wxString& name, const wxArrayString& kinds, vector<TagEntryPtr>& tags) const;
    size_t get_keywords_tags(const wxString& name, vector<TagEntryPtr>& tags);

    TagEntryPtr on_method(CxxExpression& curexp, TagEntryPtr tag, const vector<wxString>& visible_scopes);
    TagEntryPtr on_typedef(CxxExpression& curexp, TagEntryPtr tag, const vector<wxString>& visible_scopes);
    TagEntryPtr on_member(CxxExpression& curexp, TagEntryPtr tag, const vector<wxString>& visible_scopes);
    TagEntryPtr on_static_local(CxxExpression& curexp, const vector<wxString>& visible_scopes);
    TagEntryPtr on_local(CxxExpression& curexp, const vector<wxString>& visible_scopes);
    TagEntryPtr on_this(CxxExpression& curexp, const vector<wxString>& visible_scopes);

    /**
     * @brief similar to CxxExpression::from_expression however, this method also passes
     * the result tokens in the pre_processor() code to replace any macros before serving them
     * for processing
     */
    vector<CxxExpression> from_expression(const wxString& expression, CxxRemainder* remainder);
    /**
     * @brief attempt to perform simple substituation of `name` with
     * a matached entry from the `m_macros_table_map`
     */
    wxString& simple_pre_process(wxString& name) const;

    /**
     * @brief return the direct parents of a tag
     */
    vector<TagEntryPtr> get_parents_of_tag_no_recurse(TagEntryPtr parent, TemplateManager::ptr_t template_manager,
                                                      const vector<wxString>& visible_scopes);

    TagEntryPtr find_scope_tag(CxxExpression& curexp, const vector<wxString>& visible_scopes);
    bool is_scope_tag(CxxExpression& curexp, const vector<wxString>& visible_scopes)
    {
        return find_scope_tag(curexp, visible_scopes);
    }
    TagEntryPtr find_scope_tag_externvar(CxxExpression& curexp, const vector<wxString>& visible_scopes);
    TagEntryPtr on_extern_var(CxxExpression& curexp, TagEntryPtr var, const vector<wxString>& visible_scopes);

public:
    typedef shared_ptr<CxxCodeCompletion> ptr_t;

public:
    CxxCodeCompletion(ITagsStoragePtr lookup, const wxString& codelite_indexer,
                      const unordered_map<wxString, TagEntryPtr>& unit_tests_db);
    CxxCodeCompletion(ITagsStoragePtr lookup, const wxString& codelite_indexer);
    ~CxxCodeCompletion();

    /// Test API - START
    void test_set_db(unordered_map<wxString, TagEntryPtr>& db) { m_lookup->set_tests_db(db); }
    void test_clear_db() { m_lookup->clear_tests_db(); }
    /// Test API - END

    /**
     * @brief return the return value of a tag (of type function)
     */
    wxString get_return_value(TagEntryPtr tag) const;

    TagEntryPtr get_current_function_tag() const { return m_current_function_tag; }
    TagEntryPtr get_current_scope_tag() const { return m_current_container_tag; }

    /**
     * @brief sort input list of tags to a reasonable order
     */
    void sort_tags(const vector<TagEntryPtr>& tags, vector<TagEntryPtr>& sorted_tags, bool include_ctor_dtor,
                   const wxStringSet_t& visible_files);

    /**
     * @brief reset the completer (clear all cached data)
     */
    void reset();

    /**
     * determine the current scope and return it
     */
    void determine_current_scope();

    /**
     * @brief set the typedef helper table
     */
    void set_types_table(const vector<pair<wxString, wxString>>& t) { m_types_table = t; }

    /**
     * @brief set macros table
     */
    void set_macros_table(const vector<pair<wxString, wxString>>& t);

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
                              CxxRemainder* remainder = nullptr);

    /**
     * @brief search the database for list of constructors
     */
    size_t get_class_constructors(TagEntryPtr tag, vector<TagEntryPtr>& tags);

    /**
     * @brief word completion. differ from code_complete by the expression. code_complete is called when user hit
     * -> or . , while word_completion is used when user types something or in a middle of a word
     * @return list of tags matches the context and filter placed in: `candidates`
     */
    size_t word_complete(const wxString& filepath, int line, const wxString& expression, const wxString& text,
                         const vector<wxString>& visible_scopes, bool exact_match, vector<TagEntryPtr>& candidates,
                         const wxStringSet_t& visible_files = {});
    /**
     * return list of files for completion based on the prefix typed
     */
    size_t get_file_completions(const wxString& user_typed, vector<TagEntryPtr>& files, const wxString& suffix);
    /**
     * @brief return the local variables available for the current scope (passed in the c-tor)
     */
    vector<TagEntryPtr> get_locals(const wxString& filter) const;

    /**
     * @brief return list local functions
     */
    size_t get_local_tags(const wxString& filter, const wxStringSet_t& kinds, vector<TagEntryPtr>& tags) const;

    /**
     * @brief return list of completions filtered by name for a given parent
     */
    size_t get_completions(TagEntryPtr parent, const wxString& operand_string, const wxString& filter,
                           vector<TagEntryPtr>& candidates, const vector<wxString>& visible_scopes,
                           size_t limit = (size_t)-1);
    /**
     * @brief return children tag of the current scope and any other relevant scopes
     * this method calls internally to `determine_current_scope()`
     * so make sure you called `set_text` with file and line number
     */
    size_t get_children_of_current_scope(const vector<wxString>& kinds, const wxString& filter,
                                         const vector<wxString>& visible_scopes,
                                         vector<TagEntryPtr>* current_scope_children,
                                         vector<TagEntryPtr>* other_scopes_children,
                                         vector<TagEntryPtr>* global_scope_children);

    /**
     * return list of completion for non expression attempt
     */
    size_t get_word_completions(const CxxRemainder& remainder, vector<TagEntryPtr>& candidates,
                                const vector<wxString>& visible_scopes, const wxStringSet_t& visible_files);

    size_t find_definition(const wxString& filepath, int line, const wxString& expr, const wxString& text,
                           const vector<wxString>& visible_scopes, vector<TagEntryPtr>& matches);
};

#endif // CXXCODECOMPLETION_HPP
