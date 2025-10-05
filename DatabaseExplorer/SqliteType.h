//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SqliteType.h
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

#ifndef SQLITETYPE_H
#define SQLITETYPE_H

#include <wx/wxxmlserializer/XmlSerializer.h>
#include "IDbType.h" // Base class: IDbType

class SqliteType : public IDbType {

	public:
		XS_DECLARE_CLONABLE_CLASS(SqliteType);
		SqliteType();
		SqliteType(const SqliteType& obj);
		SqliteType(const wxString& typeName, long propertyFlags, UNIVERSAL_TYPE universalType);
		virtual ~SqliteType() = default;
		void InitSerialize();

	public:
		virtual bool GetAutoIncrement()			{
			return false;
		}
		virtual bool GetNotNull()				{
			return m_notNull;
		}
		virtual bool GetPrimaryKey()			{
			return false;
		}
		virtual long GetPropertyFlags()			{
			return m_dbtPropertyFlags;
		}
		virtual long GetSize()					{
			return 0;
		}
		virtual long GetSize2()					{
			return 0;
		}
		virtual wxString GetTypeName()			{
			return m_typeName;
		}
		virtual bool GetUnique()				{
			return false;
		}
		virtual bool HaveAutoIncrement() 					{
			return (m_dbtPropertyFlags & dbtAUTO_INCREMENT);
		}
		virtual bool HaveNotNull() 							{
			return (m_dbtPropertyFlags & dbtNOT_NULL);
		}
		virtual bool HavePrimaryKey() 						{
			return (m_dbtPropertyFlags & dbtPRIMARY_KEY);
		}
		virtual bool HaveSize() 							{
			return (m_dbtPropertyFlags & dbtSIZE);
		}
		virtual bool HaveSize2() {
			return (m_dbtPropertyFlags & dbtSIZE_TWO);
		}
		virtual bool HaveUnique() 							{
			return (m_dbtPropertyFlags & dbtUNIQUE);
		}
		virtual void SetAutoIncrement(bool autoIncrement) 	{
			m_autoIncrement = autoIncrement;
		}
		virtual void SetNotNull(bool notNull) 				{
			m_notNull = notNull;
		}
		virtual void SetPrimaryKey(bool primaryKey) 		{
			m_primaryKey = primaryKey;
		}
		virtual void SetSize(long size) 					{
			m_size = size;
		}
		virtual void SetSize2(long size) 					{
			m_size2 = size;
		}
		virtual void SetUnique(bool unique) 				{
			m_unique = unique;
		}

		virtual void SetPropertyFlags(long flags) 			{
			m_dbtPropertyFlags = flags;
		}

		virtual wxString ReturnSql();


		virtual IDbType::UNIVERSAL_TYPE  GetUniversalType();


	protected:

		wxString m_typeName;
		long m_dbtPropertyFlags;
		long m_universalType;

		long m_size;
		long m_size2;
		bool m_unique;
		bool m_primaryKey;
		bool m_notNull;
		bool m_autoIncrement;
};

#endif // SQLITETYPE_H
