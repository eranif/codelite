#ifndef __XVARIABLE_H__
#define __XVARIABLE_H__

#include <wx/string.h>
#include <wx/clntdata.h>
#include <list>

class wxXmlNode;
struct XVariable : public wxClientData {
    typedef std::list<XVariable> List_t;

    wxString name;
    wxString fullname;
    wxString classname;
    wxString value;
    wxString type;
    int      numchildren;
    XVariable::List_t children;

    /**
     * @brief construct XVariable object from XML
     */
    void FromXML(wxXmlNode* xmlProp, bool useDoubleQoutesOnStrings);
    
    /**
     * @brief return a string repsresentation for this variable
     */
    wxString ToString() const;
    
    /**
     * @brief default ctor
     */
    XVariable() : numchildren(0) {}
    /**
     * @brief construct XVariable from XDebug XML response
     */
    XVariable(wxXmlNode* xmlProp, bool useDoubleQoutesOnStrings = true);
    
    /**
     * @brief does this variable has children?
     * Note that this may return positive number while HasChildren return false
     * When this happens it means that codelite should query XDebug for the children
     */
    bool GetNumChildren() const {
        return numchildren;
    }
    
    /**
     * @brief contains the actual children of this variable
     */
    bool HasChildren() const {
        return !children.empty();
    }
    
    /**
     * @brief return true if a fake node should be created for this XVariable
     */
    bool GetCreateFakeNode() const {
        return GetNumChildren() && !HasChildren();
    }
};


#endif // 
