#ifndef SVN_OVERLAY_TOOL_H
#define SVN_OVERLAY_TOOL_H

#include "precompiled_header.h"

class SvnOverlayTool {

	static wxBitmap ms_bmpOK;
	static wxBitmap ms_bmpConflict;
	static wxBitmap ms_bmpModified;
	static wxBitmap ms_bmpLocked;
	static wxBitmap ms_bmpUnversioned;
	static wxBitmap ms_bmpNew;
	static wxBitmap ms_bmpDeleted;
	
private:
	SvnOverlayTool();
	virtual ~SvnOverlayTool();
	
protected:
	wxBitmap DoAddBitmap(const wxBitmap& bmp, const wxBitmap &overlayBmp);
	
public:
	static SvnOverlayTool& Get();

	wxBitmap OKIcon         (const wxBitmap& bmp);
	wxBitmap ConflictIcon   (const wxBitmap& bmp);
	wxBitmap ModifiedIcon   (const wxBitmap& bmp);
	wxBitmap NewIcon        (const wxBitmap& bmp);
	wxBitmap LockedIcon     (const wxBitmap& bmp);
	wxBitmap UnversionedIcon(const wxBitmap& bmp);
	wxBitmap DeletedIcon    (const wxBitmap& bmp);
	
};

#endif // SVN_OVERLAY_TOOL_H
