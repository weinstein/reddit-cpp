#include "file_utils.h"

#include <string>
using std::string;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <stdio.h>

namespace reddit {

bool DoesFileExist(const string& fname) {
   ifstream in_stream(fname);
   return in_stream;
}

void DeleteFile(const string& fname) {
   remove(fname.c_str());
}

void WriteStringToFile(const std::string& fname, const std::string& str) {
   ofstream out_stream(fname, ofstream::out | ofstream::binary | ofstream::trunc);
   out_stream.write(str.data(), str.size());
   out_stream.close();
}

void AppendStringToFile(const std::string& fname, const std::string& str) {
   ofstream out_stream(fname, ofstream::out | ofstream::binary | ofstream::app);
   out_stream.write(str.data(), str.size());
   out_stream.close();
}

string ReadFileToString(const string& fname) {
   ifstream in_stream(fname, ifstream::in | ifstream::binary);
   if (in_stream) {
      string contents;
      in_stream.seekg(0, std::ios::end);
      contents.resize(in_stream.tellg());
      in_stream.seekg(0, std::ios::beg);
      in_stream.read(&contents[0], contents.size());
      in_stream.close();
      return contents;
   } else {
      return string();
   }
}

};  // namespace reddit
