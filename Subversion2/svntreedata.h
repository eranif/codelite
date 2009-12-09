#ifndef SVNTREEDATA_H
#define SVNTREEDATA_H

#include <wx/treectrl.h>

class SvnTreeData : public wxTreeItemData
{
public:
	enum SvnNodeType {
		SvnNodeTypeModifiedRoot,
		SvnNodeTypeUnversionedRoot,
		SvnNodeTypeDeletedRoot,
		SvnNodeTypeAddedRoot,
		SvnNodeTypeConflictRoot
	};

	SvnNodeType m_type;

public:
	SvnTreeData(SvnNodeType type) : m_type(type) {
	}

	virtual ~SvnTreeData() {
	}

	void SetType(const SvnNodeType& type) {
		this->m_type = type;
	}
	const SvnNodeType& GetType() const {
		return m_type;
	}
};

#endif // SVNTREEDATA_H
