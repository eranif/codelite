#ifndef PP_INCLUDE_H
#define PP_INCLUDE_H

#include <vector>
#include <wx/string.h>

/**
 * @brief the main interface to the PP API.
 * @param filePath
 * @return 
 */
extern int PPScan      ( const wxString &filePath );
/**
 * @brief scan input string
 */
extern int PPScanString( const wxString &inputString );

#endif
