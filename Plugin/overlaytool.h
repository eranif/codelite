#ifndef __overlaytool__
#define __overlaytool__

#include <wx/bitmap.h>
#include "codelite_exports.h"

/**
 * @class OverlayTool a simple tool which allows creating new images with overlay icons
 * @author eran
 * @date 08/31/09
 * @brief
 */
class WXDLLIMPEXP_SDK OverlayTool
{

    static wxBitmap ms_bmpOK;
    static wxBitmap ms_bmpConflict;
    static wxBitmap ms_bmpModified;
    
public:
    enum BmpType {
        Bmp_NoChange = -1,
        Bmp_OK       =  0, 
        Bmp_Modified =  1, 
        Bmp_Conflict =  2, 
    };
    
private:
    OverlayTool();
    virtual ~OverlayTool();
    
    wxBitmap DoAddBitmap(const wxBitmap& bmp, const wxColour& colour) const;
    
public:
    static OverlayTool& Get();
    
    wxBitmap CreateBitmap(const wxBitmap& orig, OverlayTool::BmpType type) const;
};

#endif // __overlaytool__
