#ifndef STRING_HPP
#define STRING_HPP
#include <memory>
#include <string>
#include <stdexcept>
 
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; 
    if( size <= 0 ){ shusha_panic("Error during formatting."); }
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); 
}
#endif
