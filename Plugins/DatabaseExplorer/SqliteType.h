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

#include "IDbType.h" // Base class: IDbType
#include "wx/wxxmlserializer/XmlSerializer.h"

class SqliteType : public IDbType
{

public:
    XS_DECLARE_CLONABLE_CLASS_OVERRIDE(SqliteType);
    SqliteType();
    SqliteType(const SqliteType& obj);
    SqliteType(const wxString& typeName, long propertyFlags, UNIVERSAL_TYPE universalType);
    ~SqliteType() override = default;
    void InitSerialize();

public:
    bool GetAutoIncrement() override { return false; }
    bool GetNotNull() override { return m_notNull; }
    bool GetPrimaryKey() override { return false; }
    long GetPropertyFlags() override { return m_dbtPropertyFlags; }
    long GetSize() override { return 0; }
    long GetSize2() override { return 0; }
    wxString GetTypeName() override { return m_typeName; }
    bool GetUnique() override { return false; }
    bool HaveAutoIncrement() override { return (m_dbtPropertyFlags & dbtAUTO_INCREMENT); }
    bool HaveNotNull() override { return (m_dbtPropertyFlags & dbtNOT_NULL); }
    bool HavePrimaryKey() override { return (m_dbtPropertyFlags & dbtPRIMARY_KEY); }
    bool HaveSize() override { return (m_dbtPropertyFlags & dbtSIZE); }
    bool HaveSize2() override { return (m_dbtPropertyFlags & dbtSIZE_TWO); }
    bool HaveUnique() override { return (m_dbtPropertyFlags & dbtUNIQUE); }
    void SetAutoIncrement(bool autoIncrement) override { m_autoIncrement = autoIncrement; }
    void SetNotNull(bool notNull) override { m_notNull = notNull; }
    void SetPrimaryKey(bool primaryKey) override { m_primaryKey = primaryKey; }
    void SetSize(long size) override { m_size = size; }
    void SetSize2(long size) override { m_size2 = size; }
    void SetUnique(bool unique) override { m_unique = unique; }

    void SetPropertyFlags(long flags) override { m_dbtPropertyFlags = flags; }

    wxString ReturnSql() override;

    IDbType::UNIVERSAL_TYPE GetUniversalType() override;

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
