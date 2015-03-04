// Read an INI file into easy-to-access name/value pairs.

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include "ini.h"
#include "INIReader.h"

using namespace std;

INIReader::INIReader(string filename)
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

int INIReader::ParseError()
{
    return _error;
}

string INIReader::Get(string section, string name, string default_value)
{
    string key = MakeKey(section, name);
    return _values.count(key) ? _values[key] : default_value;
}

long INIReader::GetInteger(string section, string name, long default_value)
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
}

double INIReader::GetReal(string section, string name, double default_value)
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    double n = strtod(value, &end);
    return end > value ? n : default_value;
}

bool INIReader::GetBoolean(string section, string name, bool default_value)
{
    string valstr = Get(section, name, "");
    // Convert to lower case to make string comparisons case-insensitive
    std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
        return true;
    else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
        return false;
    else
        return default_value;
}

string INIReader::MakeKey(string section, string name)
{
    string key = section + "." + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name,
                            const char* value)
{
    INIReader* reader = (INIReader*)user;
    string key = MakeKey(section, name);
    if (reader->_values[key].size() > 0)
        reader->_values[key] += "\n";
    reader->_values[key] += value;
    return 1;
}

void INIReader::updateValue(std::string section, std::string name, std::string newValue){
    string key = MakeKey(section, name);
    
    std::map<string,string>::iterator it;
    it = _values.find(key);
    
    if ( it == _values.end() ){
        //Key not found
        _values.insert( it, std::pair<string,string> (key,newValue)) ;
    }
    else{
        // Update
        it->second = newValue;
    }
}

void INIReader::writeIniFile(const char* filename){
    FILE *f = fopen( filename, "w+");
    if ( f == NULL ){
        cout << "Error: Can't open file" << endl;
        return;
    }
    
    std::string lastCat = "";
    
    for (std::map<string,string>::iterator it=_values.begin(); it!=_values.end(); ++it){
        size_t pos = it->first.find('.');
        
        if ( pos != std::string::npos){
            std::string name = it->first;
            std::string config = name.substr(0,pos);
            
            if ( config.compare(lastCat) != 0 ){
                lastCat = config;
                char *S = new char[config.length() + 1];
                std::strcpy(S,config.c_str());
                fprintf(f, "[%s]\n", S);
            }
            std::string param = name.substr(pos+1, name.length() - pos - 1 );
            char *S = new char[param.length() + 1];
            std::strcpy(S,param.c_str());
            fprintf(f, "%s=", S);
            S = new char[it->second.length() + 1];
            std::strcpy(S,it->second.c_str());
            fprintf(f, "%s\n", S);
        }
    }
    fclose(f);
}
