#include "my_sftp_tree_model.h"

MySFTPTreeModel::MySFTPTreeModel()
{
}

MySFTPTreeModel::~MySFTPTreeModel()
{
}

wxString MySFTPTreeModel::GetColumnType(unsigned int col) const
{
    wxUnusedVar(col);
#ifdef __WXGTK__
    return "string";
#else
    return SFTPTreeModel::GetColumnType(col);
#endif
}
