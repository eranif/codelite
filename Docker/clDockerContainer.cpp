#include "clDockerContainer.h"
#include <wx/tokenzr.h>

clDockerContainer::clDockerContainer() {}

clDockerContainer::~clDockerContainer() {}

#if 0
#define CHOP_STRING(s) ((s.length() > 20) ? s.Mid(0, 12) : s)
#else
#define CHOP_STRING(s) (s)
#endif

bool clDockerContainer::Parse(const wxString& line)
{
    wxString L = line;
    L.Trim().Trim(false);
    wxArrayString cells = ::wxStringTokenize(L, "|", wxTOKEN_RET_EMPTY_ALL);
    if(cells.size() != 7) return false;

    m_id = CHOP_STRING(cells[0]);
    m_image = CHOP_STRING(cells[1]);
    m_command = cells[2];
    m_created = CHOP_STRING(cells[3]);
    m_status = cells[4];
    m_ports = CHOP_STRING(cells[5]);
    m_name = cells[6];
    return true;
}
