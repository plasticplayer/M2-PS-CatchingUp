#ifndef l_row_h
#define l_row_h

#include <sys/types.h>

//#if defined(HAS_MYSQL)
#include <mysql/mysql.h>
#define T_RESULT MYSQL_RES
#define T_ROW    MYSQL_ROW
//#endif

class Row
{
private:
    T_RESULT *result;
    T_ROW fields;

public:
    Row(T_RESULT *, T_ROW);
    ~Row();

    char *GetField(int);
    int GetFieldCount();
    int IsClosed();
    void Close();
};

#endif // l_row_h
