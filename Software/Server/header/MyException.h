#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H
#include <exception>
#include <string>

using namespace std;

class ConnectionExistException: public exception
{
public:
    ConnectionExistException(int numero=0, string const& phrase="") throw()
         :m_numero(numero),m_phrase(phrase)
    {}

     virtual const char* what() const throw()
     {
         return m_phrase.c_str();
     }

    virtual ~ConnectionExistException() throw()
    {}

private:
    int m_numero;               //Numéro de l'erreur
    string m_phrase;            //Description de l'erreur
};

class ConnectionException: public exception
{
public:
    ConnectionException(int numero=0, string const& phrase="") throw()
         :m_numero(numero),m_phrase(phrase)
    {}

     virtual const char* what() const throw()
     {
         return m_phrase.c_str();
     }

    virtual ~ConnectionException() throw()
    {}

private:
    int m_numero;               //Numéro de l'erreur
    string m_phrase;            //Description de l'erreur
};

class MySQLException: public exception
{
public:
    MySQLException(int numero=0, string const& phrase="") throw()
         :m_numero(numero),m_phrase(phrase)
    {}

     virtual const char* what() const throw()
     {
         return m_phrase.c_str();
     }

    virtual ~MySQLException() throw()
    {}

private:
    int m_numero;               //Numéro de l'erreur
    string m_phrase;            //Description de l'erreur
};

class ResultException: public exception
{
public:
    ResultException(int numero=0, string const& phrase="") throw()
         :m_numero(numero),m_phrase(phrase)
    {}

     virtual const char* what() const throw()
     {
         return m_phrase.c_str();
     }

    virtual ~ResultException() throw()
    {}

private:
    int m_numero;               //Numéro de l'erreur
    string m_phrase;            //Description de l'erreur
};

class RowException: public exception
{
public:
    RowException(int numero=0, string const& phrase="") throw()
         :m_numero(numero),m_phrase(phrase)
    {}

     virtual const char* what() const throw()
     {
         return m_phrase.c_str();
     }

    virtual ~RowException() throw()
    {}

private:
    int m_numero;               //Numéro de l'erreur
    string m_phrase;            //Description de l'erreur
};



#endif // MYEXCEPTION_H
