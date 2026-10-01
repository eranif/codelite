#ifndef THREADSTACK_HPP
#define THREADSTACK_HPP

#include "codelite_exports.h"

#include <wx/string.h>

/**
 * @brief capture and return a formatted backtrace of the calling thread, as of the point
 * of the call.
 *
 * Supported on Windows (x86_64 only) and Linux. On any other platform, or on an
 * unsupported Windows architecture, an empty string is returned.
 */
WXDLLIMPEXP_CL wxString DumpCurrentThreadStack();

#endif // THREADSTACK_HPP
