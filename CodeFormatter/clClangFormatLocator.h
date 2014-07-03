#ifndef CLCLANGFORMATLOCATOR_H
#define CLCLANGFORMATLOCATOR_H

#include <wx/string.h>

class clClangFormatLocator
{
public:
    clClangFormatLocator();
    virtual ~clClangFormatLocator();

    /**
     * @brief locate clang-format executable
     * @return
     */
    bool Locate(wxString &clangFormat);
};

#endif // CLCLANGFORMATLOCATOR_H
