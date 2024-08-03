#include "LSPDetector.hpp"

LSPDetector::LSPDetector(const wxString& name)
    : m_name(name)
{
}

LSPDetector::~LSPDetector() {}

void LSPDetector::GetLanguageServerEntry(LanguageServerEntry& entry)
{
    entry.SetLanguages(GetLanguages());
    entry.SetCommand(GetCommand());
    entry.SetEnabled(IsEnabled());
    entry.SetDisaplayDiagnostics(true);
    entry.SetConnectionString(GetConnectionString());
    entry.SetInitOptions(GetInitialiseOptions());
    entry.SetName(GetName());
}

bool LSPDetector::Locate()
{
    DoClear();
    return DoLocate();
}

void LSPDetector::DoClear()
{
    m_command.Clear();
    m_languages.Clear();
    m_connectionString.Clear();
    m_priority = 50;
}