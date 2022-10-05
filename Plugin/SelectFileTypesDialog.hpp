#ifndef SELECTFILETYPESDIALOG_HPP
#define SELECTFILETYPESDIALOG_HPP

#include "SelectFileTypesDialogBase.hpp"
#include "codelite_exports.h"
#include "fileextmanager.h"

#include <unordered_map>

class WXDLLIMPEXP_SDK SelectFileTypesDialog : public SelectFileTypesDialogBase
{
    std::unordered_map<wxString, std::vector<FileExtManager::FileType>> m_all;

private:
    void InitializeList(const wxArrayString& selected_items);
    void Search(const wxString& filter);

public:
    SelectFileTypesDialog(wxWindow* parent, const wxArrayString& initial_selection);
    virtual ~SelectFileTypesDialog();

    /**
     * @brief return the selected items
     */
    wxArrayString GetValue() const;

protected:
    void OnSearch(wxCommandEvent& event) override;
};

#endif // SELECTFILETYPESDIALOG_HPP
