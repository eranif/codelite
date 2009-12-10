#ifndef SVNTREEDATA_H
#define SVNTREEDATA_H

#include <wx/treectrl.h>

class SvnTreeData : public wxTreeItemData
{
public:
	enum SvnNodeType {
		SvnNodeTypeInvalid = -1,
		SvnNodeTypeRoot,
		SvnNodeTypeModifiedRoot,
		SvnNodeTypeUnversionedRoot,
		SvnNodeTypeDeletedRoot,
		SvnNodeTypeAddedRoot,
		SvnNodeTypeConflictRoot,
		SvnNodeTypeFile
	};

	SvnNodeType m_type;
	wxString    m_filepath;

public:
	SvnTreeData(SvnNodeType type, const wxString &filepath)
			: m_type(type)
			, m_filepath(filepath) {
	}

	virtual ~SvnTreeData() {
	}

	void SetType(const SvnNodeType& type) {
		this->m_type = type;
	}
	const SvnNodeType& GetType() const {
		return m_type;
	}
	void SetFilepath(const wxString& filepath) {
		this->m_filepath = filepath;
	}
	const wxString& GetFilepath() const {
		return m_filepath;
	}
};

#endif // SVNTREEDATA_H
