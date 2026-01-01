#ifndef PHPDOCPARAM_H
#define PHPDOCPARAM_H

#include "PHPSourceFile.h"
#include "codelite_exports.h"

#include <vector>
#include <wx/string.h>

class WXDLLIMPEXP_CL PHPDocParam
{
public:
    using Vec_t = std::vector<std::pair<wxString, wxString>>;

protected:
    PHPSourceFile& m_sourceFile;
    const wxString& m_comment;
    PHPDocParam::Vec_t m_params;

public:
    PHPDocParam(PHPSourceFile& sourceFile, const wxString& comment);
    ~PHPDocParam() = default;

    /**
     * @brief parse comment and map of params found in it
     */
    const PHPDocParam::Vec_t& Parse();
};

#endif // PHPDOCPARAM_H
