#include <malloc.h>
#include "Row.h"
#include "MyException.h"

#include <stdio.h>
#include <stdlib.h>


Row::Row(T_RESULT *res, T_ROW row)
{
    fields = row;
    result = res;
}

Row::~Row()
{
    if( !IsClosed() )
    {
        Close();
    }
}

void Row::Close()
{
    if( IsClosed() )
    {
        throw RowException(1,"Row closed.");
    }
    fields = (T_ROW)NULL;
    result = (T_RESULT *)NULL;
}

int Row::GetFieldCount()
{
    if( IsClosed() )
    {
        throw RowException(1,"Row closed.");
    }
    return mysql_num_fields(result);
}

// Caller should be prepared for a possible NULL
// return value from this method.
char *Row::GetField(int field)
{
    if( IsClosed() )
    {
        throw RowException(1,"Row closed.");
    }
    if( field < 1 || field > GetFieldCount() )
    {
        throw RowException(2,"Field index out of bounds.");
    }
    return fields[field-1];
}

int Row::IsClosed()
{
    return (fields == (T_ROW)NULL);
}
