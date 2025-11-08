#ifndef PHPDOCPROPERTY_H
#define PHPDOCPROPERTY_H

#include "PHPSourceFile.h"
#include "codelite_exports.h"
#include "wxStringHash.h"
#include <tuple>
#include <vector>

class WXDLLIMPEXP_CL PHPDocProperty
{
public:
    using Tuple_t = std::vector<std::tuple<wxString, wxString, wxString>>;

protected:
    PHPSourceFile& m_sourceFile;
    const wxString& m_comment;
    PHPDocProperty::Tuple_t m_params;

public:
    PHPDocProperty(PHPSourceFile& sourceFile, const wxString& comment);
    ~PHPDocProperty() = default;

    /**
     * @brief parse comment for @params
     */
    const PHPDocProperty::Tuple_t& ParseParams();
    /**
     * @brief parse comment for @method
     */
    const PHPDocProperty::Tuple_t& ParseMethods();
};

#endif // PHPDOCPROPERTY_H
