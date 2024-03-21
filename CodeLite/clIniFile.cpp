#include "clIniFile.h"

#include "cl_standard_paths.h"

clIniFile::clIniFile(const wxFileName& iniFullPath)
    : wxFileConfig("", "", iniFullPath.GetFullPath(), "", wxCONFIG_USE_LOCAL_FILE)
{
}

clIniFile::clIniFile(const wxString& fullname)
    : wxFileConfig("", "",
                   clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
                       wxFileName::GetPathSeparator() + fullname,
                   "", wxCONFIG_USE_LOCAL_FILE)
{
}

clIniFile::~clIniFile() { Flush(); }
