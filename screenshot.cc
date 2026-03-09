#include <cstddef>
#include <cstdio>
#include <array>
#include <string>
#include <vector>
#include <iostream>

// runs an arbitrary command and returns its stdout output
std::string run_command(const std::string& cmd){
    std::array<char, 128> buffer;
    std::string result; //result of running the command

    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return nullptr;

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    pclose(pipe);

    if(!result.empty() && result.back() == '\n')
        result.pop_back();

    return result;
}

std::vector<unsigned char> run_grim(const std::string geometry){
    std::string cmd = "";
    if(geometry.empty())
        cmd += "grim -";
    else
        cmd += "grim -g \"" + geometry + "\" -";
    std::vector<unsigned char> data; // this will store the binary data of the screenshot taken

    // std::cout << cmd.c_str();
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return data;

    std::array<unsigned char, 4096> buffer;

    while(true){
        size_t bytes = fread(buffer.data(), 1, buffer.size(), pipe); // basically read the binary output byte by byte
        if(bytes == 0) break; // no more stuff to read
        data.insert(data.end(), buffer.begin(), buffer.begin() + bytes); // append the contents of the most recent buffer to the data vector
    }

    pclose(pipe);

    return data;

}