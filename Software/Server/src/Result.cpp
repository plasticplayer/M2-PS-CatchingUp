#include <stdio.h>
#include <stdlib.h>

#include "Result.h"
#include "MyException.h"

Result::Result(T_RESULT * res)
{
    if( res == (T_RESULT *)NULL )
    {
        return;
    }
    result = res;
    row_count = -1;
    current_row = NULL;
}


int Result::Next()
{
    T_ROW row;

    if( result == (T_RESULT *)NULL )
    {
        throw ResultException(1,"Result set closed.");
    }
    row = mysql_fetch_row(result);
    if( !row )
    {
        current_row = (Row *)NULL;
        return 0;
    }
    else
    {
        current_row = new Row(result, row);
        return 1;
    }
}

Row *Result::GetCurrentRow()
{
    if( result == (T_RESULT *)NULL )
    {
        throw ResultException(1,"Result set closed.");
    }
    return current_row;
}

void Result::Close()
{
    if( result == (T_RESULT *)NULL )
    {
        return;
    }
    mysql_free_result(result);
    result = (T_RESULT *)NULL;
}

int Result::GetRowCount()
{
    if( result == (T_RESULT *)NULL )
    {
        throw ResultException(1,"Result set closed.");
    }
    if( row_count > -1 )
    {
        return row_count;
    }
    else
    {
        row_count = mysql_num_rows(result);
        return row_count;
    }
}
