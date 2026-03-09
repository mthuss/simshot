#include <cstdio>
#include <string>
#include <vector>

enum class AreaType{
    Full,
    Selection,
};
enum class SaveType{
    File,
    Clipboard,
};

std::string run_command(const std::string& cmd);
// std::vector<unsigned char> run_grim(const std::string& geometry);
std::vector<unsigned char> run_grim(const std::string geometry = "");