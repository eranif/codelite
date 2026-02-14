#include "clTempFile.hpp"

#include "StringUtils.h"
#include "cl_standard_paths.h"
#include "fileutils.h"

clTempFile::clTempFile(const wxString& ext)
{
    wxFileName::Mkdir(clStandardPaths::Get().GetTempDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    m_filename = FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), "cltmp", ext);
}

clTempFile::clTempFile(const wxString& folder, const wxString& ext)
{
    m_filename = FileUtils::CreateTempFileName(folder, "cltmp", ext);
}

clTempFile::~clTempFile()
{
    if (m_deleteOnDestruct) {
        FileUtils::RemoveFile(m_filename);
    }
}

bool clTempFile::Write(const wxString& content, wxMBConv& conv)
{
    return FileUtils::WriteFileContent(GetFileName(), content, conv);
}

wxString clTempFile::GetFullPath(bool wrapped_with_quotes) const
{
    wxString fullpath = GetFileName().GetFullPath();
    if (wrapped_with_quotes) {
        return StringUtils::WrapWithDoubleQuotes(fullpath);
    }
    return fullpath;
}

void clTempFile::Persist() { m_deleteOnDestruct = false; }
