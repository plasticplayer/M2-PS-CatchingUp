#include "Connection.h"
#include "MyException.h"
#include <stdio.h>
#include <stdlib.h>
Connection::Connection(char *host, char *db)
{
    connection = (MYSQL *)NULL;
    Connect(host, db, (char *)NULL, (char *)NULL);
}

Connection::Connection(char *host, char *db, char *uid, char *pw)
{
    connection = (MYSQL *)NULL;
    Connect(host, db, uid, pw);
}

void Connection::Connect(char *host, char *db, char *uid, char *pw)
{
    if( IsConnected() )
    {
        throw ConnectionExistException(1,"Connection has already been established.");
    }
    mysql_init(&mysql);
    connection = mysql_real_connect(&mysql, host,uid, pw, db, 0, 0, 0);
    if( !IsConnected() )
    {
        throw ConnectionException(2,(char *)mysql_error(&mysql));
    }
}

Connection::~Connection()
{
    if( IsConnected() )
    {
        Close();
    }
}

void Connection::Close()
{
    if( !IsConnected() )
    {
        return;
    }
    mysql_close(connection);
    connection = (MYSQL *)NULL;
}

Result *Connection::Query(char *sql)
{
    T_RESULT *res;
    int state;

    // if not connectioned, there is nothing we can do
    if( !IsConnected() )
    {
        throw ConnectionException(2,"Not connected.");
    }
    // execute the query
    state = mysql_query(connection, sql);
    // an error occurred
    if( state < 0 )
    {
        throw MySQLException(mysql_errno(&mysql),(char *)mysql_error(&mysql));
    }
    // grab the result, if there was any
    res = mysql_store_result(connection);
    // if the result was null, it was an update or an error occurred
    // NOTE: mSQL does not throw errors on msqlStoreResult()
    if( res == (T_RESULT *)NULL )
    {
        // just set affected_rows to the return value from msqlQuery()
        // field_count != 0 means an error occurred

        if(mysql_errno(connection))
        {
            throw MySQLException(mysql_errno(connection),mysql_error(connection));;
        }
        else
        {
            // store the affected_rows
            affected_rows = mysql_affected_rows(connection);
        }
        // return NULL for updates
        return (Result *)NULL;
    }
    // return a Result instance for queries
    return new Result(res);
}

int Connection::GetAffectedRows()
{
    return affected_rows;
}

int Connection::IsConnected()
{
    return !(!connection);
}


