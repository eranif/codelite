#ifndef PHPDOCPARAM_H
#define PHPDOCPARAM_H

#include "PHPSourceFile.h"
#include "codelite_exports.h"
#include "wxStringHash.h"
#include <vector>
#include <wx/string.h>

class WXDLLIMPEXP_CL PHPDocParam
{
public:
    typedef std::vector<std::pair<wxString, wxString> > Vec_t;

protected:
    PHPSourceFile& m_sourceFile;
    const wxString& m_comment;
    PHPDocParam::Vec_t m_params;

public:
    PHPDocParam(PHPSourceFile& sourceFile, const wxString& comment);
    ~PHPDocParam();

    /**
     * @brief parse comment and map of params found in it
     */
    const PHPDocParam::Vec_t& Parse();
};

#endif // PHPDOCPARAM_H
