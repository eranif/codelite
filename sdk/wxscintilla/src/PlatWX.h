#ifndef PLATWX_H // [CHANGED]
#define PLATWX_H // [CHANGED]

#ifdef SCI_NAMESPACE
namespace Scintilla
{
#endif
wxRect wxRectFromPRectangle(PRectangle prc);
PRectangle PRectangleFromwxRect(wxRect rc);
wxColour wxColourFromCA(const ColourAllocated& ca);
#ifdef SCI_NAMESPACE
}
#endif
#endif // [CHANGED]
