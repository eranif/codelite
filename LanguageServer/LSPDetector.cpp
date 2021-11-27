#include "LSPDetector.hpp"

LSPDetector::LSPDetector(const wxString& name)
    : m_name(name)
{
}

LSPDetector::~LSPDetector() {}

void LSPDetector::GetLanguageServerEntry(LanguageServerEntry& entry)
{
    entry.SetLanguages(GetLangugaes());
    entry.SetCommand(GetCommand());
    entry.SetEnabled(IsEnabled());
    entry.SetDisaplayDiagnostics(true);
    entry.SetConnectionString(GetConnectionString());
    entry.SetPriority(GetPriority());
    entry.SetName(GetName());
    entry.SetEnv(m_env);
}

bool LSPDetector::Locate()
{
    DoClear();
    return DoLocate();
}

void LSPDetector::DoClear()
{
    m_command.Clear();
    m_langugaes.Clear();
    m_connectionString.Clear();
    m_priority = 50;
}