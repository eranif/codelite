#include "FilePath.hpp"

LSP::FilePath::FilePath(const wxString& path)
    : m_path(path)
{
#ifdef __WXMSW__
    if(!m_path.StartsWith("file://") && !m_path.StartsWith("/")) {
        m_path.Replace("/", "\\");
    }
#endif
}

LSP::FilePath::FilePath() {}

LSP::FilePath::~FilePath() {}
