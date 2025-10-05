//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : constraint.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <wx/wx.h>
#include <wx/dblayer/include/DatabaseResultSet.h>
#include <wx/wxxmlserializer/XmlSerializer.h>
#include "IDbAdapter.h"

/*! \brief Class representing one table constraint. */
class Constraint : public xsSerializable {

public:
	/*! \brief Constraint type enum */
	enum constraintType {
		primaryKey,
		foreignKey,
		noKey
	};
	/*! \brief Constraint action enum */
	enum constraintAction {
		restrict,
		cascade,
		setNull,
		noAction
	};

	XS_DECLARE_CLONABLE_CLASS(Constraint);
	/*! \brief Default constructors */
	Constraint();
	Constraint(const Constraint& obj);
	Constraint(const wxString& name, const wxString& localColumn, constraintType type, constraintAction onDelete, constraintAction onUpdate);
	/*! \brief Default destructors */
	virtual ~Constraint() = default;
	
	/*! \brief Set local column name */
	void SetLocalColumn(const wxString & localColumn) {
		this->m_localColumn = localColumn;
	}
	/*! \brief Set constraint name  */
	void SetName(const wxString & name) {
		this->m_name = name;
	}
	/*! \brief Set reference column name */
	void SetRefCol(const wxString & refCol) {
		this->m_refCol = refCol;
	}
	/*! \brief Set reference table name */
	void SetRefTable(const wxString & refTable) {
		this->m_refTable = refTable;
	}
	/*! \brief Set constraint type */
	void SetType(constraintType type) {
		this->m_type = type;
	}
	/*! \brief Set action OnDelete */
	void SetOnDelete(constraintAction onDelete) {
		this->m_onDelete = onDelete;
	}
	/*! \brief Set action OnUpdate */
	void SetOnUpdate(constraintAction onUpdate) {
		this->m_onUpdate = onUpdate;
	}
	/*! \brief Get action OnDelete */
	constraintAction GetOnDelete() const {
		return m_onDelete;
	}
	/*! \brief Get action OnUpdate */
	constraintAction GetOnUpdate() const {
		return m_onUpdate;
	}
	/*! \brief Get local column name */
	const wxString & GetLocalColumn() const {
		return m_localColumn;
	}
	/*! \brief Get constraint name */
	const wxString & GetName() const {
		return m_name;
	}
	/*! \brief Get reference column name */
	const wxString & GetRefCol() const {
		return m_refCol;
	}
	/*! \brief Get reference table name */
	const wxString & GetRefTable() const {
		return m_refTable;
	}
	/*! \brief Get constraint type */
	constraintType GetType() const {
		return m_type;
	}

protected:
	constraintType m_type;
	wxString m_name;
	wxString m_localColumn;
	wxString m_refTable;
	wxString m_refCol;
	constraintAction m_onDelete;
	constraintAction m_onUpdate;

	void InitSerializable();
};

#endif // CONSTRAINT_H
