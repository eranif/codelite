#ifndef MYSFTPTREEMODEL_H
#define MYSFTPTREEMODEL_H

#include "sftptreemodel.h" // Base class: SFTPTreeModel

class MySFTPTreeModel : public SFTPTreeModel
{
public:
    virtual wxString GetColumnType(unsigned int col) const;

    MySFTPTreeModel();
    virtual ~MySFTPTreeModel();


};

#endif // MYSFTPTREEMODEL_H
