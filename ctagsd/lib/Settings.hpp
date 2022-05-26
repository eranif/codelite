#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "macros.h"

#include <vector>
#include <wx/arrstr.h>
#include <wx/filename.h>

class CTagsdSettings
{
    wxString m_file_mask = "*.cpp;*.h;*.hpp;*.cxx;*.cc;*.hxx";
    wxArrayString m_search_path;
    std::vector<std::pair<wxString, wxString>> m_tokens;
    std::vector<std::pair<wxString, wxString>> m_types;
    wxString m_codelite_indexer;
    wxString m_ignore_spec = "/.git/;/.svn/;/build/;/build-;/CPack_Packages/;/CMakeFiles/";
    size_t m_limit_results = 150;
    wxString m_settings_dir;

private:
    void build_search_path(const wxFileName& filepath);
    void CreateDefault(const wxFileName& filepath);

public:
    CTagsdSettings();
    ~CTagsdSettings();

    void Load(const wxFileName& filepath);
    void Save(const wxFileName& filepath);

    void SetLimitResults(size_t limit_results) { this->m_limit_results = limit_results; }
    size_t GetLimitResults() const { return m_limit_results; }
    void SetCodeliteIndexer(const wxString& codelite_indexer) { this->m_codelite_indexer = codelite_indexer; }
    void SetFileMask(const wxString& file_mask) { this->m_file_mask = file_mask; }
    void SetIgnoreSpec(const wxString& ignore_spec) { this->m_ignore_spec = ignore_spec; }
    void SetSearchPath(const wxArrayString& search_path) { this->m_search_path = search_path; }
    void SetTokens(const std::vector<std::pair<wxString, wxString>>& tokens) { this->m_tokens = tokens; }
    const wxString& GetCodeliteIndexer() const { return m_codelite_indexer; }
    const wxString& GetFileMask() const { return m_file_mask; }
    const wxString& GetIgnoreSpec() const { return m_ignore_spec; }
    const wxArrayString& GetSearchPath() const { return m_search_path; }
    const std::vector<std::pair<wxString, wxString>>& GetTokens() const { return m_tokens; }
    void SetTypes(const std::vector<std::pair<wxString, wxString>>& types) { this->m_types = types; }
    const std::vector<std::pair<wxString, wxString>>& GetTypes() const { return m_types; }
    const wxString& GetSettingsDir() const { return m_settings_dir; }
    wxStringMap_t GetMacroTable() const;
};

#endif // SETTINGS_HPP
