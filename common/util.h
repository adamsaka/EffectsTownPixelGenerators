#pragma once

#include <string>

//A helper to format a pretty string, and throw it as an exception
void ThrowNullPointer(const std::string& file, const std::string& function, int line);
void ThrowPrettyException(const std::string& file, const std::string& function, int line, const std::string& message);


//Check if a value is null, and throw it it is.
#define check_null(EXPRESSION) if(!(EXPRESSION)) { ThrowNullPointer( __FILE__,__FUNCTION__, __LINE__); }
#define assert(EXPRESSION, MESSAGE) if(!(EXPRESSION)) { ThrowPrettyException( __FILE__,__FUNCTION__, __LINE__, MESSAGE); }