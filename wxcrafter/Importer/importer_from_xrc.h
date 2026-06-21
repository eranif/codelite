#ifndef IMPORTERFROMXRC_H
#define IMPORTERFROMXRC_H

#include "import_dlg.h"
#include "wxc_widget.h"

class wxWindow;

namespace ImportFromXrc
{
	
class Importer
{
public:
    Importer(wxWindow* parent);
    ~Importer() = default;

    bool ImportProject(ImportDlg::ImportFileData& data) const;

protected:
    bool ParseFile(wxXmlDocument& doc, wxcWidget::List_t& toplevels) const;

private:
    wxWindow* m_Parent;
};

} // namespace ImportFromXrc

#endif // IMPORTERFROMXRC_H
