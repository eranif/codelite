#include "LSPStartupInfo.h"

LSPStartupInfo::LSPStartupInfo() {}

LSPStartupInfo::LSPStartupInfo(const LSPStartupInfo& other) { *this = other; }

LSPStartupInfo& LSPStartupInfo::operator=(const LSPStartupInfo& other)
{
    m_helperCommand = other.m_helperCommand;
    m_lspServerCommand = other.m_lspServerCommand;
    m_lspServerCommandWorkingDirectory = other.m_lspServerCommandWorkingDirectory;
    m_flags = other.m_flags;
    return *this;
}
