#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "macros.h"

#include <wx/arrstr.h>
#include <wx/filename.h>

class CTagsdSettings
{
    wxString m_file_mask = "*.cpp;*.h;*.hpp;*.cxx;*.cc;*.hxx";
    wxArrayString m_search_path;
    wxStringMap_t m_tokens;
    wxString m_codelite_indexer;
    wxString m_ignore_spec = ".git/;.svn/;build-debug/;build-release/;build/;CPack_Packages/";

private:
    void build_search_path(const wxFileName& filepath);

public:
    CTagsdSettings();
    ~CTagsdSettings();

    void Load(const wxFileName& filepath);
    void Save(const wxFileName& filepath);

    void SetCodeliteIndexer(const wxString& codelite_indexer) { this->m_codelite_indexer = codelite_indexer; }
    void SetFileMask(const wxString& file_mask) { this->m_file_mask = file_mask; }
    void SetIgnoreSpec(const wxString& ignore_spec) { this->m_ignore_spec = ignore_spec; }
    void SetSearchPath(const wxArrayString& search_path) { this->m_search_path = search_path; }
    void SetTokens(const wxStringMap_t& tokens) { this->m_tokens = tokens; }
    const wxString& GetCodeliteIndexer() const { return m_codelite_indexer; }
    const wxString& GetFileMask() const { return m_file_mask; }
    const wxString& GetIgnoreSpec() const { return m_ignore_spec; }
    const wxArrayString& GetSearchPath() const { return m_search_path; }
    const wxStringMap_t& GetTokens() const { return m_tokens; }
};

#endif // SETTINGS_HPP
