#include <fstream>
#include <iterator>
#include "File.hpp"
#include "Error.hpp"
#include <iostream>
#include <sstream>
const std::string File::open(const char* file_name) {
   std::ifstream file(file_name);
   if(errno == ENOENT) shusha_panic("No such file or directory\n");
   std::ostringstream ss;
   ss << file.rdbuf();
   const std::string s = ss.str();
   file.close();
   return s;

}


