#ifndef SELECTFILETYPESDIALOG_HPP
#define SELECTFILETYPESDIALOG_HPP

#include "SelectFileTypesDialogBase.hpp"
#include "codelite_exports.h"
#include "fileextmanager.h"

#include <unordered_map>

class WXDLLIMPEXP_SDK SelectFileTypesDialog : public SelectFileTypesDialogBase
{
    std::unordered_map<wxString, FileExtManager::FileType> m_all;

private:
    void InitializeList();
    void Search(const wxString& filter);

public:
    SelectFileTypesDialog(wxWindow* parent);
    virtual ~SelectFileTypesDialog();

    /**
     * @brief return the selected items
     */
    std::unordered_map<wxString, FileExtManager::FileType> GetValue() const;
};

#endif // SELECTFILETYPESDIALOG_HPP
