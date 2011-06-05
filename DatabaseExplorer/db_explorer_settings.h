#ifndef DBEXPLORERSETTINGS_H
#define DBEXPLORERSETTINGS_H

#include "serialized_object.h"

class DbExplorerSettings : public SerializedObject
{
protected:
	wxArrayString m_recentFiles;


public:
	DbExplorerSettings();
	virtual ~DbExplorerSettings();

	void SetRecentFiles(const wxArrayString& recentFiles);
	const wxArrayString& GetRecentFiles() const {
		return m_recentFiles;
	}
	virtual void DeSerialize(Archive& arch);
	virtual void Serialize(Archive& arch);
};

#endif // DBEXPLORERSETTINGS_H
