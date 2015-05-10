#ifndef l_connection_h
#define l_connection_h

#include <sys/time.h>

//#if defined(HAS_MYSQL)
#include <mysql/mysql.h>
//#endif

#include "Result.h"

class Connection {
private:
    int affected_rows;
//#if defined(HAS_MYSQL)
    MYSQL mysql;
    MYSQL *connection;
//#else
//    #error No database defined.
//#endif

public:
    Connection(char *, char *);
    Connection(char *, char *, char *, char *);
    ~Connection();

    void Close();
    void Connect(char *host, char *db, char *uid, char *pw);
    int GetAffectedRows();
    char *GetError();
    int IsConnected();
    Result *Query(char *);
};

#endif // l_connection_h
