#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoUtil.hpp"

// Create a nullptr-terminated string by reading the provided file
std::string readFileToString(const char* filePath)
{

	std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return std::string();
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}
