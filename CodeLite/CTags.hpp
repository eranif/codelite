#ifndef CTAGSGENERATOR_HPP
#define CTAGSGENERATOR_HPP

#include "codelite_exports.h"
#include <vector>
#include <wx/filename.h>

class WXDLLIMPEXP_CL CTags
{
protected:
    wxString WrapSpaces(const wxString& file) const;
    bool DoGenerate(const wxString& filesContent, const wxFileName& outputFile);

public:
    CTags();
    virtual ~CTags();

    /**
     * @brief give list of files, generate an output tags file
     */
    bool Generate(const std::vector<wxFileName>& files, const wxFileName& outputFile);
    bool Generate(const std::vector<std::string>& files, const wxFileName& outputFile);
};

#endif // CTAGSGENERATOR_HPP
