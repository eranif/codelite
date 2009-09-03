#ifndef __overlaytool__
#define __overlaytool__

#include <wx/bitmap.h>

/**
 * @class OverlayTool a simple tool which allows creating new images with overlay icons
 * @author eran
 * @date 08/31/09
 * @file overlaytool.h
 * @brief
 */
class OverlayTool {

	static wxBitmap ms_bmpOK;
	static wxBitmap ms_bmpConflict;
	static wxBitmap ms_bmpModified;

private:
	OverlayTool();
	virtual ~OverlayTool();

public:
	static OverlayTool& Get();

	wxBitmap AddOKIcon      (wxBitmap& bmp);
	wxBitmap AddConflictIcon(wxBitmap& bmp);
	wxBitmap AddModifiedIcon(wxBitmap& bmp);
};
#endif // __overlaytool__
