#include "clDockerImage.h"
#include <wx/arrstr.h>
#include <wx/tokenzr.h>

clDockerImage::clDockerImage() {}

clDockerImage::~clDockerImage() {}

#if 0
#define CHOP_STRING(s) ((s.length() > 20) ? s.Mid(0, 12) : s)
#else
#define CHOP_STRING(s) (s)
#endif

bool clDockerImage::Parse(const wxString& line)
{
    wxString L = line;
    L.Trim().Trim(false);
    wxArrayString cells = ::wxStringTokenize(L, "|", wxTOKEN_RET_EMPTY_ALL);
    if(cells.size() != 5) return false;

    m_id = CHOP_STRING(cells[0]);
    m_repository = CHOP_STRING(cells[1]);
    m_tag = cells[2];
    m_created = CHOP_STRING(cells[3]);
    m_size = cells[4];
    return true;
}
