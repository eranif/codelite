#ifndef STDTOWX_H
#define STDTOWX_H

#include <wx/string.h>
#include <string>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL StdToWX
{
public:
    /**
     * @brief remove from str string from offset 'from' with len
     */
    static void Remove(std::string& str, size_t from, size_t len);
    
    /**
     * @brief return true if str starts with what
     */
    static bool StartsWith(const std::string& str, const std::string& what);
    
    /**
     * @brief return true if str ends with what
     */
    static bool EndsWith(const std::string& str, const std::string& what);
    
    /**
     * @brief 
     */
    static void Trim(std::string& str, bool fromRight = true);

    /**
     * @brief remove last count bytes
     */
    static void RemoveLast(std::string& str, size_t count);
};

#endif // STDTOWX_H
