#include "clClangFormatLocator.h"

#include "Platform.hpp"
#include "asyncprocess.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>

clClangFormatLocator::clClangFormatLocator() {}

clClangFormatLocator::~clClangFormatLocator() {}

bool clClangFormatLocator::Locate(wxString& clangFormat)
{
    return PLATFORM::WhichWithVersion("clang-format", { 12, 13, 14, 15, 16, 17, 18, 19, 20 }, &clangFormat);
}
