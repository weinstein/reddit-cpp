#include <string>

namespace reddit {

bool DoesFileExist(const std::string& fname);
void DeleteFile(const std::string& fname);
void WriteStringToFile(const std::string& fname, const std::string& str);
void AppendStringToFile(const std::string& fname, const std::string& str);
std::string ReadFileToString(const std::string& fname);

};  // namespace reddit
