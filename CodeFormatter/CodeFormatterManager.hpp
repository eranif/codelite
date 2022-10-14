#ifndef CODEFORMATTERMANAGER_HPP
#define CODEFORMATTERMANAGER_HPP

#include "GenericFormatter.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

class CodeFormatterManager
{
    std::vector<std::shared_ptr<GenericFormatter>> m_formatters;

protected:
    void push_back(GenericFormatter* formatter);
    void initialize_defaults();
    void clear();

public:
    CodeFormatterManager();
    virtual ~CodeFormatterManager();

    std::shared_ptr<GenericFormatter> GetFormatter(const wxString& filepath) const;
    std::shared_ptr<GenericFormatter> GetFormatterByName(const wxString& name) const;
    std::shared_ptr<GenericFormatter> GetFormatterByContent(const wxString& content) const;
    size_t GetAllNames(wxArrayString* names) const;

    // do we have a formatter that can handle `filepath`?
    bool CanFormat(const wxString& filepath) const;
    // can we format this content?
    bool CanFormatByContent(const wxString& content) const;

    // serialization API
    void Load();
    void Save();

    // discard all the settings and load the defaults
    void RestoreDefaults();

    /// Remove all remote commands set in the formatters
    void ClearRemoteCommands();
};

#endif // CODEFORMATTERMANAGER_HPP
