#include "clTempDC.hpp"

#include "drawingutils.h"

#include <wx/bitmap.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/defs.h>

clTempDC::clTempDC()
{
    m_tmpBmp = wxBitmap(1, 1);
    m_memDC = new wxMemoryDC(m_tmpBmp);
    m_gcdc = new wxGCDC();
    DrawingUtils::GetGCDC(*m_memDC, *m_gcdc);
}

clTempDC::~clTempDC()
{
    wxDELETE(m_gcdc);
    wxDELETE(m_memDC);
}
