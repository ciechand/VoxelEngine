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

glm::vec3 translate1DPos(unsigned int index, unsigned int size){
    glm::vec3 tempVec(index%size, floor((index/size)%size),  floor((index/size)/size));
    return tempVec;
}

unsigned int translate3DPos(glm::vec3 position, unsigned int size){
    return (unsigned int)((position.z*size*size)+(position.y*size)+position.x);
}

unsigned int translate3DPos(int x,int y,int z, unsigned int size){
    return (unsigned int)((z*size*size)+(y*size)+x);
}

void print4x4Matrix(glm::mat4 matrix){
    std::cerr << "Matrix Printed Below:" << std::endl; 
    for(int i=0; i<4; i++){
        for(int j=0;j<4;j++){
            std::cerr <<  matrix[i][j] << " ";
        }
        std::cerr << std::endl;
    } 


}
