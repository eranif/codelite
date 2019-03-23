#include "LSPStartupInfo.h"

LSPStartupInfo::LSPStartupInfo() {}

LSPStartupInfo::LSPStartupInfo(const LSPStartupInfo& other) { *this = other; }

LSPStartupInfo& LSPStartupInfo::operator=(const LSPStartupInfo& other)
{
    m_lspServerCommand = other.m_lspServerCommand;
    m_workingDirectory = other.m_workingDirectory;
    m_connectioString = other.m_connectioString;
    m_flags = other.m_flags;
    return *this;
}

LSPStartupInfo::~LSPStartupInfo() {}