/*
* Copyright (C) 2011-2013 Klaralavdalens Datakonsult AB, a KDAB Group company, info@kdab.com
* Author: Volker Krause <volker.krause@kdab.com>
*/
#ifndef SQLQUERYBUILDERBASE_H
#define SQLQUERYBUILDERBASE_H
#include "SqlQuery.h"

#include "SqlCondition.h"
#include "sqlate_export.h"
#include "SqlGlobal.h"

/** Abstract base class for SQL query builders. All builders should inherit from this class.
 */
class SQLATE_EXPORT SqlQueryBuilderBase
{
public:
    /// Create a new query builder for the given database
    explicit SqlQueryBuilderBase( const QSqlDatabase &db = QSqlDatabase::database() );
    virtual ~SqlQueryBuilderBase();
    /// ... FROM @p tableName ...
    void setTable( const QString &tableName );
    void setTable( const QLatin1String &tableName ) { setTable( QString( tableName ) ); }
    template <typename Table>
    void setTable( const Table & = Table() )
    {
        setTable( Table::sqlName() );
    }

    /// Creates the query object and executes the query. The method throws an SqlException on error.
    virtual void exec();

    /// Returns the created query object, when called first, the query object is assembled and prepared
    /// Subclasses must implement this method. The method throws an SqlException if there is an error preparing
    /// the query.
    virtual SqlQuery& query() = 0;

    /// Resets the internal status to "not assembled", meaning the query() call will assemble the query again.
    /// This makes possible to modify an already existing builder object after query() was used.
    void invalidateQuery();

protected:
    /** Binds @p value to placeholder @p placeholderIndex in m_query.
     *  Unlike the similar methods in QSqlQuery this also applies transformations to handle types not supported
     *  by QtSQL, such as UUID.
     *  @note This assumes the use of named bindings in the form ":&lt;index&gt;".
     */
    void bindValue( int placeholderIndex, const QVariant &value );

    /** Returns the SQL expression returning the current date/time on the server depending on the used database backend. */
    QString currentDateTime() const;

    /** Returns a prepared query for the given @p sqlStatement.
     *  Unlike calling SqlQuery::prepare() manually, this will re-use cached prepared queries.
     *  @throw SqlException if query preparation failed
     */
    SqlQuery prepareQuery( const QString &sqlStatement );

protected:
    friend class SelectQueryBuilderTest;
    friend class SelectTest;
    QSqlDatabase m_db;
    QString m_table;
    SqlQuery m_query;
    QString m_queryString; // hold the assembled query string, used for unit testing
    bool m_assembled;
};

#endif
