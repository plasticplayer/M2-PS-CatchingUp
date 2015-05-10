#ifndef l_result_h
#define l_result_h

#include <sys/time.h>

//#if defined(HAS_MYSQL)
#include <mysql/mysql.h>
//#endif

#include "Row.h"

class Result {
private:
    int row_count;
    T_RESULT *result;
    Row *current_row;

public:
    Result(T_RESULT *);
    ~Result();

    void Close();
    Row *GetCurrentRow();
    int GetRowCount();
    int Next();
};

#endif // l_result_h
