#if 0
#include <wx/dcmemory.h>
#include "overlaytool.h"
#include <wx/xrc/xmlres.h>
#include <wx/settings.h>

wxBitmap OverlayTool::ms_bmpOK;
wxBitmap OverlayTool::ms_bmpConflict;
wxBitmap OverlayTool::ms_bmpModified;

OverlayTool::OverlayTool()
{
	ms_bmpOK       = wxXmlResource::Get()->LoadBitmap(wxT("overlay_ok"));
	ms_bmpModified = wxXmlResource::Get()->LoadBitmap(wxT("overlay_modified"));
	ms_bmpConflict = wxXmlResource::Get()->LoadBitmap(wxT("overlay_conflict"));
}

OverlayTool::~OverlayTool()
{
}

wxBitmap OverlayTool::AddConflictIcon(wxBitmap& bmp)
{
	wxMemoryDC dcMem;

#if defined(__WXGTK__)|| defined(__WXMAC__)
	wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	wxBitmap bitmap(16, 16);
	dcMem.SelectObject(bitmap);
	dcMem.SetPen( wxPen(col) );
	dcMem.SetBrush( wxBrush(col) );
	dcMem.DrawRectangle(wxPoint(0, 0), wxSize(16, 16));
	dcMem.DrawBitmap(bmp, wxPoint(0, 0), true);
#else
	wxBitmap bitmap(bmp);
	dcMem.SelectObject(bitmap);
#endif

	dcMem.DrawBitmap(ms_bmpConflict, wxPoint(0, 0), true);
	dcMem.SelectObject(wxNullBitmap);

	return bitmap;
}

wxBitmap OverlayTool::AddModifiedIcon(wxBitmap& bmp)
{
	wxMemoryDC dcMem;

#if defined(__WXGTK__)|| defined(__WXMAC__)
	wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	wxBitmap bitmap(16, 16);
	dcMem.SelectObject(bitmap);
	dcMem.SetPen( wxPen(col) );
	dcMem.SetBrush( wxBrush(col) );
	dcMem.DrawRectangle(wxPoint(0, 0), wxSize(16, 16));
	dcMem.DrawBitmap(bmp, wxPoint(0, 0), true);
#else
	wxBitmap bitmap(bmp);
	dcMem.SelectObject(bitmap);
#endif

	dcMem.DrawBitmap(ms_bmpModified, wxPoint(0, 0), true);
	dcMem.SelectObject(wxNullBitmap);

	return bitmap;
}

wxBitmap OverlayTool::AddOKIcon(wxBitmap& bmp)
{
	wxMemoryDC dcMem;

#if defined(__WXGTK__)|| defined(__WXMAC__)
	wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	wxBitmap bitmap(16, 16);
	dcMem.SelectObject(bitmap);
	dcMem.SetPen( wxPen(col) );
	dcMem.SetBrush( wxBrush(col) );
	dcMem.DrawRectangle(wxPoint(0, 0), wxSize(16, 16));
	dcMem.DrawBitmap(bmp, wxPoint(0, 0), true);
#else
	wxBitmap bitmap(bmp);
	dcMem.SelectObject(bitmap);
#endif

	dcMem.DrawBitmap(ms_bmpOK, wxPoint(0, 0), true);
	dcMem.SelectObject(wxNullBitmap);

	return bitmap;
}

OverlayTool& OverlayTool::Get()
{
	static OverlayTool theTool;
	return theTool;
}

#endif
