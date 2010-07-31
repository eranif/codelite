#ifndef CRAWLER_INCLUDE_H
#define CRAWLER_INCLUDE_H

#include <vector>
#include <wx/string.h>

/**
 * @brief the main interface to the PP API.
 * @param filePath
 * @return 
 */
extern int PPScan( const wxString &filePath );

#endif
