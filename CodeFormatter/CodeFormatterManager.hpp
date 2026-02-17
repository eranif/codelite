#ifndef CODEFORMATTERMANAGER_HPP
#define CODEFORMATTERMANAGER_HPP

#include "GenericFormatter.hpp"

#include <memory>
#include <vector>

class CodeFormatterManager
{
    std::vector<std::shared_ptr<GenericFormatter>> m_formatters;

protected:
    void push_back(std::shared_ptr<GenericFormatter> formatter);
    void initialize_defaults();
    void clear();

public:
    CodeFormatterManager() = default;
    virtual ~CodeFormatterManager() = default;

    std::shared_ptr<GenericFormatter> GetFormatter(const wxString& filepath) const;
    std::shared_ptr<GenericFormatter> GetFormatterByName(const wxString& name) const;
    std::shared_ptr<GenericFormatter> GetFormatterByContent(const wxString& content) const;
    size_t GetAllNames(wxArrayString* names) const;

    /// Add new custom formatter
    bool AddCustom(std::shared_ptr<GenericFormatter> formatter);

    /// Delete formatter by name
    bool DeleteFormatter(const wxString& name);

    // do we have a formatter that can handle `filepath`?
    bool CanFormat(const wxString& filepath) const;

    // serialization API
    void Load();
    void Save();

    // discard all the settings and load the defaults
    void RestoreDefaults();

    /// Remove all remote commands set in the formatters
    void ClearRemoteCommands();
};

#endif // CODEFORMATTERMANAGER_HPP
