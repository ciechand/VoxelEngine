#include "../Headers/CryoBase.hpp"

Ray::Ray(){
	Origin = glm::vec3();
	Direction = glm::vec3();
	InverseDirection = glm::vec3(1/Direction.x,1/Direction.y,1/Direction.z);
	sign[0] = (InverseDirection.x < 0);
	sign[1] = (InverseDirection.y < 0);
	sign[2] = (InverseDirection.z < 0);
	times[0] = 0.0f;
	times[1] = 1.0f;

}

Ray::Ray(glm::vec3 O, glm::vec3 D, float t0, float t1){
	Origin = O;
	Direction = D;
	InverseDirection = glm::vec3(1/Direction.x,1/Direction.y,1/Direction.z);
	sign[0] = (InverseDirection.x < 0);
	sign[1] = (InverseDirection.y < 0);
	sign[2] = (InverseDirection.z < 0);
	times[0] = t0;
	times[1] = t1;
}

CollisionResult::CollisionResult(){
	side = -1;
	minCol = -1.0f;
	maxCol = -1.0f;
}

CollisionResult::CollisionResult(int s, float n, float x){
	side = s;
	minCol = n;
	maxCol = x;
}

BoundingBox::BoundingBox(){
	Position = glm::vec3(-1.0f);
	prevPos = glm::vec3(-1.0f);
	Limbs = glm::vec3(-1.0f);
}

glm::vec3 BoundingBox::getPrevPos(){
	return prevPos;	
}

void BoundingBox::setPos(glm::vec3 pos){
	if(Position == glm::vec3(-1.0f) && prevPos == glm::vec3(-1.0f))
		prevPos = pos;
	else
		prevPos = Position;
	Position = pos;
} 

glm::vec3 BoundingBox::getPos(){
	return Position;
}

void BoundingBox::setLimbs(glm::vec3 limb){
	Limbs = limb;
}

glm::vec3 BoundingBox::getLimbs(){
	return Limbs;
}

glm::vec3 BoundingBox::getMin(){
	return Position-Limbs;
}

glm::vec3 BoundingBox::getMax(){
	return Position+Limbs;
}


ColRes Intersection(BBox bounding, Ray r){
	int side = -1;
	float tmin, tmax, tymin, tymax, tzmin,tzmax;
	ColRes cr;
	if(bounding.getLimbs() == glm::vec3(-1.0f))
		return cr;
	glm::vec3 bounds[2] = {bounding.getMin(),bounding.getMax()};
	//~~~~~~~~~~~~X
	tmin = (bounds[r.sign[0]].x - r.Origin.x) * r.InverseDirection.x;
	tmax = (bounds[1-r.sign[0]].x - r.Origin.x) * r.InverseDirection.x;
	side = 0;
	//~~~~~~~~~~~~Y
	tymin = (bounds[r.sign[1]].y - r.Origin.y) * r.InverseDirection.y;
	tymax = (bounds[1-r.sign[1]].y - r.Origin.y) * r.InverseDirection.y;
	if((tmin > tymax) || (tymin > tmax))
		return cr;
	if(tymin > tmin){
		tmin = tymin;
		side = 1;
	}
	if(tymax < tmax){
		tmax = tymax;
		//side = 1;
	}
	//~~~~~~~~~~~~Z
	tzmin = (bounds[r.sign[2]].z - r.Origin.z) * r.InverseDirection.z;
	tzmax = (bounds[1-r.sign[2]].z - r.Origin.z) * r.InverseDirection.z;

	if((tmin > tzmax) || (tzmin > tmax))
		return cr;
	if(tzmin > tmin){
		tmin = tzmin;
		side = 2;
	}
	if(tzmax < tmax){
		tmax = tzmax;
		//side = 2;
	}
	//~~~~~~~~~~~~Return
	if ((tmin < r.times[1]) && (tmax > r.times[0])){
		int finalSide = (1-r.sign[side])+(side*2);
		cr = ColRes(finalSide, tmin, tmax);
		return cr;
	}else
		return cr;
}

ColRes Intersection(BBox a, BBox b){

	float u0, u1;

	glm::vec3 dif = b.getPrevPos()-a.getPrevPos();
	glm::vec3 al = a.getLimbs();
	glm::vec3 bl = b.getLimbs(); 
	ColRes cr;
	//std::cout << "BlockPos: \n\tX: "<<b.getPos().x <<  "\n\tY: "<<b.getPos().y <<  "\n\tZ: "<<b.getPos().z << std::endl;
	//std::cout << "Dif: \n\tX: "<< dif.x <<  "\n\tY: "<< dif.y <<  "\n\tZ: "<< dif.z << std::endl;
	//std::cout << "difCompare: \n\tX: "<< al.x+bl.x <<  "\n\tY: "<< al.y+bl.y <<  "\n\tZ: "<< al.z+bl.z << std::endl;
	if(fabs(dif.x)<=al.x+bl.x && fabs(dif.y)<=al.y+bl.y && (fabs(dif.z)<=al.z+bl.z)){
		cr = ColRes(0, 0.0f, 0.0f);
		return cr;
	}

	glm::vec3 av = a.getPos()-a.getPrevPos();
	glm::vec3 bv = b.getPos()-b.getPrevPos();
	glm::vec3 u_0(0.0f,0.0f,0.0f);
	glm::vec3 u_1(1.0f,1.0f,1.0f);
	glm::vec3 v = bv-av;

	glm::vec3 amin = a.getMin()-glm::vec3(0.01f);
	glm::vec3 amax = a.getMax()+glm::vec3(0.01f);
	glm::vec3 bmin = b.getMin()-glm::vec3(0.01f);
	glm::vec3 bmax = b.getMax()+glm::vec3(0.01f);

	for(int i=0; i<3; i++){
		if(amax[i] < bmin[i] && v[i] < 0)
			u_0[i] = (amax[i] - bmin[i] ) / v[i];
		else if(bmax[i] < amin[i] && v[i] > 0)
			u_0[i] = (amin[i] - bmax[i] ) / v[i];
		if(bmax[i] > amin[i] && v[i] < 0)
			u_1[i] = (amin[i] - bmax[i] ) / v[i];
		else if(amax[i] > bmin[i] && v[i] > 0)
			u_1[i] = (amax[i] - bmin[i] ) / v[i];
	}
	u0 = fmaxf(u_0.x,fmaxf(u_0.y,u_0.z));
	u1 = fminf(u_1.x,fminf(u_1.y,u_1.z));

	//std::cout << "MinTime: " << u0 << std::endl;
	//std::cout << "MaxTime: " << u1 << std::endl;
	if(u0 == 0.0f && u1 == 1.0f)
		return cr;
	if(u0<=u1){
		cr = ColRes(0,u0,u1);
		return cr;
	}
	else
		return cr;
	
}

glm::vec3 playerCollideTerrain(BoundingBox playerBox, glm::vec3 moveDir, int & axis){
	float deltaTime = State.getdTime();
	//std::cout << "Starting Collide Check" << std::endl;
	std::vector<Block*> viableBlockList;
	std::vector<ColRes> colTimes;

	BoundingBox movedBox = playerBox;
	movedBox.setPos((movedBox.getPos()+moveDir));
	//playerBox.setPos(playerBox.getPos());

	glm::vec3 playerMin = playerBox.getMin();
	glm::vec3 playerMax = playerBox.getMax();
	//glm::vec3 pLimbs =  playerBox.getLimbs();
	glm::vec3 movedMin = movedBox.getMin();
	glm::vec3 movedMax = movedBox.getMax();

	glm::vec3 minCorner = {fminf(playerMin.x,movedMin.x),fminf(playerMin.y,movedMin.y),fminf(playerMin.z,movedMin.z)};
	glm::vec3 maxCorner = {fmaxf(playerMax.x,movedMax.x),fmaxf(playerMax.y,movedMax.y),fmaxf(playerMax.z,movedMax.z)};

	glm::vec2 finalChunk = glm::vec2(floor(maxCorner.x/(CHUNKDIMS*BLOCKSCALE)), floor(maxCorner.z/(CHUNKDIMS*BLOCKSCALE)));
	glm::vec2 startChunk = glm::vec2(floor(minCorner.x/(CHUNKDIMS*BLOCKSCALE)), floor(minCorner.z/(CHUNKDIMS*BLOCKSCALE)));

	glm::vec2 chunkDif = finalChunk-startChunk;
	std::vector<glm::vec2> CheckedChunks;
	CheckedChunks.push_back(startChunk);
	//std::cout << "Pushed Back Start Chunk: " << startChunk.x << ": " << startChunk.y << std::endl;
	for(int cx=fminf(startChunk.x,finalChunk.x); cx<=fmaxf(startChunk.x,finalChunk.x); cx++){
		for(int cy=fminf(startChunk.y,finalChunk.y); cy<=fmax(startChunk.y,finalChunk.y); cy++){
			//std::cout << "Pushed Back Chunk: " << startChunk.x << ": " << startChunk.y << std::endl;
			if(startChunk != glm::vec2(cx,cy))
				CheckedChunks.push_back(glm::vec2(cx,cy));
		}
	}
	//This needs to scope out an area in between minCorner and maxCorner, need to control signs Correctly so this doesnt exit automatically. Also, make sure to use the dimentions of the chunk as boundries. (fminf(),fmaxf)
	//std::cout << "Chunkmath Done, Checking " << CheckedChunks.size() <<" world chunks." << std::endl;
	Chunk * tempChunk = nullptr;
	glm::vec2 chunkPos;
	ColRes col;

	for(int c=0; c<CheckedChunks.size(); c++){
		for(int i=0; i<World.size(); i++){
			chunkPos = World[i]->getPos();
			if(chunkPos == CheckedChunks[c]){
				tempChunk = World[i];
				break;
			}
		}
		if(tempChunk == nullptr) {
			std::cerr << "tempChunk is nullptr, WTF?!" << std::endl;
		//std::cout << "tempChunk is nullptr, WTF?!" << std::endl;
			return glm::vec3(-1.0f);
		}

		int Chunkx = (int)CheckedChunks[c].x*CHUNKDIMS*BLOCKSCALE;
		int Chunkz = (int)CheckedChunks[c].y*CHUNKDIMS*BLOCKSCALE;
		float beginBoundx = fmaxf(minCorner.x, Chunkx);
		float endBoundx = fminf(maxCorner.x, (Chunkx+((CHUNKDIMS-1)*BLOCKSCALE)));
		float beginBoundz = fmaxf(minCorner.z, Chunkz); 
		float endBoundz = fminf(maxCorner.z, (Chunkz+((CHUNKDIMS-1)*BLOCKSCALE)));
		glm::vec2 Boundx = glm::vec2(beginBoundx-Chunkx,endBoundx-Chunkx);
		glm::vec2 Boundz = glm::vec2(beginBoundz-Chunkz,endBoundz-Chunkz);
		Boundx = Boundx/BLOCKSCALE;
		Boundz = Boundz/BLOCKSCALE;
		//std::cout << "ChunkBegins: \n\tX: " << Chunkx << " -> " << Chunkx+((CHUNKDIMS-1)*BLOCKSCALE) << "\n\tZ: " << Chunkz << " -> " << Chunkz+((CHUNKDIMS-1)*BLOCKSCALE) <<std::endl;
		//std::cout << "Starting Block Check in Chunk: \n\tX:" << chunkPos.x << "\n\tZ:" << chunkPos.y << std::endl;
		//std::cout << "Bounds:" << std::endl;
		//std::cout << "X Bounds: \n\tMin: " << Boundx.x << "\n\tMax: " << Boundx.y <<std::endl;
		//std::cout << "Y Bounds: \n\tMin: " << fmaxf(minCorner.y/BLOCKSCALE,0) << "\n\tMax: " << fminf(maxCorner.y/BLOCKSCALE,CHUNKHEIGHT-1) <<std::endl;
		//std::cout << "Z Bounds: \n\tMin: " << Boundz.x << "\n\tMax: " << Boundz.y <<std::endl;
		//std::cout << "Blocks:" << std::endl;
		for(int i=floor(fmaxf(minCorner.y/BLOCKSCALE,0)); i<=ceil(fminf(maxCorner.y/BLOCKSCALE,CHUNKHEIGHT-1)); i++){
			for(int j=floor(Boundz.x); j<=ceil(Boundz.y); j++){
				for(int k=floor(Boundx.x); k<=ceil(Boundx.y); k++){
					col = ColRes();
					int curBlock = (i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k;
					IOBJ * tempOBJ = Renderer.getIObject(tempChunk->grid[curBlock].first);
					Block * tempB = tempOBJ->getBlocks(tempChunk->grid[curBlock].second);
					if(tempB!=nullptr)
						col = Intersection(movedBox, tempB->getBounds());
					//std::cout << "\tk: " << k << "\n\ti: " << i << "\n\tj: " << j << std::endl;
					if(col.side!=-1.0f){
						viableBlockList.push_back(tempB);
						colTimes.push_back(col);
					}
				}
			}
		}
	}
	if(viableBlockList.size() == 0 || colTimes.size() == 0)
		return glm::vec3(1.0f);
	//std::cout << "Sides :" << std::endl;	
	int signs[2] = {1,-1};
	float minTime = 1.0f;
	glm::vec3 minAxisTime(1.0f,1.0f,1.0f);


	glm::vec3 ppos = playerBox.getPos();
	for(int i=0; i<colTimes.size(); i++){
		Ray r(ppos, viableBlockList[i]->getPos()-ppos, 0.0f, glm::length(viableBlockList[i]->getPos()-ppos));
		//std::cout << "Before Block Inter:" << std::endl;
		ColRes cr = Intersection(viableBlockList[i]->getBounds(), r);
		axis = cr.side/2;
		//std::cout << "BlockPos: \n\tX: "<< viableBlockList[i]->getPos().x <<  "\n\tY: "<< viableBlockList[i]->getPos().y <<  "\n\tZ: "<< viableBlockList[i]->getPos().z << std::endl;
		//std::cout << "ColTime " << i << ": " << colTimes[i] << std::endl;
		//std::cout << "side: " << side << std::endl;

		//THIS IS WRONG AND NEEDS TO BE FIXED FOR JUMPING!
		if(colTimes[i].minCol < minAxisTime[axis] && glm::dot(glm::normalize(DirectionalVectors[cr.side]),glm::normalize(moveDir)) <= PI/8){
			minAxisTime[axis] = colTimes[i].minCol;
		}
	}

	//WTF is this shit... Add a part here that takes removes the stickiness of the character.

	return minAxisTime;
}

std::pair<Block *, int> pickBlock(Ray sight){
	std::vector<Block*> viableBlockList;
	std::vector<ColRes> viableSides;

	glm::vec3 origin = sight.Origin;
	glm::vec3 destination = origin+(glm::normalize(sight.Direction)*sight.times[1]);

	if(origin.y/BLOCKSCALE >= (CHUNKHEIGHT) && destination.y/BLOCKSCALE >= (CHUNKHEIGHT) )
		return std::make_pair(nullptr,-1);
	if (origin.y/BLOCKSCALE <= 0.0f && destination.y/BLOCKSCALE <= 0.0f)
		return std::make_pair(nullptr,-1);

	glm::vec2 startChunk = glm::vec2(floor(origin.x/(CHUNKDIMS*BLOCKSCALE)), floor(origin.z/(CHUNKDIMS*BLOCKSCALE)));
	glm::vec2 finalChunk = glm::vec2(floor(destination.x/(CHUNKDIMS*BLOCKSCALE)), floor(destination.z/(CHUNKDIMS*BLOCKSCALE)));

	glm::vec2 chunkDif = finalChunk-startChunk;
	std::vector<glm::vec2> CheckedChunks;
	CheckedChunks.push_back(startChunk);
	//std::cout << "Pushed Back Start Chunk: " << startChunk.x << ": " << startChunk.y << std::endl;
	for(int cx=fminf(startChunk.x,finalChunk.x); cx<=fmaxf(startChunk.x,finalChunk.x); cx++){
		for(int cy=fminf(startChunk.y,finalChunk.y); cy<=fmaxf(startChunk.y,finalChunk.y); cy++){
			//std::cout << "Pushed Back Chunk: " << startChunk.x << ": " << startChunk.y << std::endl;
			if(startChunk != glm::vec2(cx,cy))
				CheckedChunks.push_back(glm::vec2(cx,cy));
		}
	}
	//This needs to scope out an area in between origin and destination, need to control signs Correctly so this doesnt exit automatically. Also, make sure to use the dimentions of the chunk as boundries. (fminf(),fmaxf)
	//std::cout << "Chunkmath Done, Checking " << CheckedChunks.size() <<" world chunks." << std::endl;
	Chunk * tempChunk = nullptr;
	glm::vec2 chunkPos;
	ColRes inter;

	for(int c=0; c<CheckedChunks.size(); c++){
		for(int i=0; i<World.size(); i++){
			chunkPos = World[i]->getPos();
			//std::cout << "Chunk\n\tX: " << chunkPos.x << " \n\tY: " << chunkPos.y << std::endl; 
			if(chunkPos == CheckedChunks[c]){
				tempChunk = World[i];
				break;
			}
		}
		if(tempChunk == nullptr) {
			std::cerr << "tempChunk is nullptr, WTF?!" << std::endl;
			return std::make_pair(nullptr,-1);
		}
		int Chunkx = CheckedChunks[c].x*CHUNKDIMS*BLOCKSCALE;
		int Chunkz = CheckedChunks[c].y*CHUNKDIMS*BLOCKSCALE;
		float beginBoundx = fmaxf(fminf(origin.x, destination.x), Chunkx);
		float endBoundx = fminf(fmaxf(origin.x, destination.x), (Chunkx+((CHUNKDIMS-1)*BLOCKSCALE)));
		float beginBoundz = fmaxf(fminf(origin.z, destination.z), Chunkz); 
		float endBoundz = fminf(fmaxf(origin.z, destination.z), (Chunkz+((CHUNKDIMS-1)*BLOCKSCALE)));
		glm::vec2 Boundx;
		glm::vec2 Boundz;
		Boundx = glm::vec2(beginBoundx-Chunkx,endBoundx-Chunkx);
		Boundz = glm::vec2(beginBoundz-Chunkz,endBoundz-Chunkz);
		Boundx = Boundx/BLOCKSCALE;
		Boundz = Boundz/BLOCKSCALE;

		for(int i=floor(fmaxf(fminf(origin.y/BLOCKSCALE,destination.y/BLOCKSCALE),0.0f)); i<=ceil(fminf(fmaxf(origin.y/BLOCKSCALE,destination.y/BLOCKSCALE),CHUNKHEIGHT-1)); i++){
			for(int j=floor(Boundz.x); j<=ceil(Boundz.y); j++){
				for(int k=floor(Boundx.x); k<=ceil(Boundx.y); k++){
					inter = ColRes();
					//std::cout << "\tk: " << k << "\n\ti: " << i << "\n\tj: " << j << std::endl;
					int curBlock = (i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k;
					IOBJ * tempOBJ = Renderer.getIObject(tempChunk->grid[curBlock].first);
					Block * tempB = tempOBJ->getBlocks(tempChunk->grid[curBlock].second);
					if(tempB!=nullptr){
						inter = Intersection(tempB->getBounds(), sight);
						if(inter.side!=-1){
							viableBlockList.push_back(tempB);
							viableSides.push_back(inter);
						}
					}
				}
			}
		}
	}
	//std::cout << "After Intersection." << std::endl;
	if(viableBlockList.size() == 0 || viableSides.size() == 0)
		return std::make_pair(nullptr,-1);

	float finalDist = (float)std::numeric_limits<float>::max();
	int minBlock = -1;
	for(int i=0; i<viableSides.size(); i++){
		if(viableSides[i].minCol*sight.times[1] < finalDist){
			finalDist = viableSides[i].minCol*sight.times[1];
			minBlock = i;
		}
	}
	if(minBlock == -1)
		return std::make_pair(nullptr,-1);
	
	return std::make_pair(viableBlockList[minBlock], viableSides[minBlock].side);
}

// Create a nullptr-terminated string by reading the provided file
std::string readFileToString(const char* filePath)
{

	std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

void readOBJFile(IOBJ * Object, const char * fileName){
	//Make sure the object file has a name already assigned
	if (fileName == nullptr){
		perror("Have not set a filename, Please do so before trying to load an obj file again.");
		return;
	}
//std::cout << "Loading in Asset as : " << fileName << std::endl;
	//opening the OBJ file for reading
	FILE * objFile = fopen(fileName, "r");
	if (objFile == nullptr){
		perror("THAT FILE DOES NOT EXIST");
		exit(EXIT_FAILURE);
	}
	//creating a buffer to read the file into
	char fileBuffer[128];
	std::vector<GLuint> vertexIndices;
	std::vector<GLuint> normalIndices;
	std::vector<GLuint> textureIndices;
	while (fgets(fileBuffer, 128, objFile) != nullptr){
		//checking which identifier you are currently reading and assigning appropriately.
		char identifier[3];
		sscanf(fileBuffer, "%s", identifier);
		if (strcmp(identifier, "#") == 0){
			continue;
		} else if (strcmp(identifier, "v") == 0){
			glm::vec4 verts;
			sscanf(fileBuffer, "%*s %f %f %f", &verts.x, &verts.y, &verts.z);
			verts.w = 1.0;
			Object->addVertices(verts);
		} else if (strcmp(identifier, "vn") == 0){
			glm::vec4 norm;
			sscanf(fileBuffer, "%*s %f %f %f", &norm.x, &norm.y, &norm.z);
			norm.w = 0.0;
			Object->addVertexNormals(norm);
		} else if (strcmp(identifier, "vt") == 0){
			glm::vec2 tex;
			sscanf(fileBuffer, "%*s %f %f", &tex.x, &tex.y);
			Object->addTextureCoords(tex);
		} else if (strcmp(identifier, "f") == 0){
			Object->setDrawType(GL_TRIANGLES);
			GLuint indices[9];
			sscanf(fileBuffer, "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &indices[0], &indices[1], &indices[2], &indices[3], &indices[4], &indices[5], &indices[6], &indices[7], &indices[8]);
			for (int j = 0; j < 3; j++){
				vertexIndices.push_back(indices[(3 * j)] - 1);
				normalIndices.push_back(indices[(3 * j) + 2] - 1);
				textureIndices.push_back(indices[(3 * j) + 1] - 1);
			}
		} else if (strcmp(identifier, "l") == 0){
			Object->setDrawType(GL_LINES);
			GLuint indices[4];
			sscanf(fileBuffer, "%*s %d/%d %d/%d", &indices[0], &indices[1], &indices[2], &indices[3]);
			for (int j = 0; j < 2; j++){
				vertexIndices.push_back(indices[2*j] - 1);
				normalIndices.push_back(0);
				textureIndices.push_back(indices[(2*j)+1] - 1);
			}
			Object->addTextureCoords(glm::vec2());
			Object->addVertexNormals(glm::vec4());
		}else{
			perror("Incorrect Identifier!"); //WTF KINDA OBJ FILE ARE YOU READING?!
		}
	}

	fclose(objFile);
	std::map<vertexInfo, GLuint> indexMap;
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> tex;

	//Fix all the indices so that it doesnt have to be done later.
	//This needs to be done since open gl only allows one index array for each object so, you need to reorganize the indices that are read in from the OBJ file.

	for (size_t i = 0; i < vertexIndices.size(); i++){
		struct vertexInfo v = {Object->getVertices(vertexIndices[i]), Object->getVertexNormals(normalIndices[i]), Object->getTextureCoords(textureIndices[i])};
		bool exists = false;
		GLuint index = 0;
		//Checking for unique (vertex,normal,texture) triplets.
		std::map<vertexInfo, GLuint>::iterator it = indexMap.find(v);
		 
		if (it == indexMap.end()){
			exists = false;
		} else{
			index = it->second;
			exists = true;
		}
		if (exists){
			Object->addIndices(index);
		} else{
			verts.push_back(Object->getVertices(vertexIndices[i]));
			norms.push_back(Object->getVertexNormals(normalIndices[i]));
			tex.push_back(Object->getTextureCoords(textureIndices[i]));
			index = (GLuint)verts.size() - 1;
			Object->addIndices(index);
			indexMap[v] = index;
		}
	};

	//The vectors must be re-applied so that the order is correct in correspondence with the new index array.
	Object->setVertices(verts);
	Object->setVertexNormals(norms);
	Object->setTextureCoords(tex);
//std::cout << "Finnished Loading in Asset of : " << fileName << std::endl;
}

void loadBMP(const char * fileName, GLuint * textureID){
	//header for testing if it is a png
	png_byte header[8];

	//open file as binary
	FILE *fp = fopen(fileName, "rb");
	if (!fp) {
		
	}

	//read the header
	fread(header, 1, 8, fp);

	//test if png
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) {
		fclose(fp);
		perror("TEXTURE LOAD ERROR 1");
	}

	//create png struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
	nullptr, nullptr);
	if (!png_ptr) {
		fclose(fp);
		perror("TEXTURE LOAD ERROR 2");
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) nullptr, (png_infopp) nullptr);
		fclose(fp);
		perror("TEXTURE LOAD ERROR 3");
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) nullptr);
		fclose(fp);
		perror("TEXTURE LOAD ERROR 4");
	}

	//png error stuff, not sure libpng man suggests this.
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		perror("TEXTURE LOAD ERROR 5");
	}

	//init png reading
	png_init_io(png_ptr, fp);

	//let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	//variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
	nullptr, nullptr, nullptr);

	//update width and height based on png info
	int width = twidth;
	int height = theight;

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte *image_data = new png_byte[rowbytes * height];
	if (!image_data) {
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		perror("TEXTURE LOAD ERROR 6");
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[height];
	if (!row_pointers) {
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		fclose(fp);
		perror("TEXTURE LOAD ERROR 7");
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	for (int i = 0; i < height; ++i)
		row_pointers[height - 1 - i] = image_data + i * rowbytes;

	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	//Now generate the OpenGL texture object
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexStorage2D(GL_TEXTURE_2D, 3, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	fclose(fp);
 
}

// Create a GLSL program object from vertex and fragment shader files
GLuint createShadersProgram(const char* vsFile, const char* fsFile){
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read shaders
    std::string vertShaderStr = readFileToString(vsFile);
    std::string fragShaderStr = readFileToString(fsFile);
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = GL_FALSE;
    int logLength;

    // Compile vertex shader
   //std::cout << "Compiling vertex shader." << std::endl;
    glShaderSource(vertShader, 1, &vertShaderSrc, nullptr);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(vertShader, logLength, nullptr, &vertShaderError[0]);
    if(logLength > 1) std::cout << "vert Errors: " << &vertShaderError[0] << std::endl;

    // Compile fragment shader
   	std::cerr << "Compiling fragment shader." << std::endl;
    glShaderSource(fragShader, 1, &fragShaderSrc, nullptr);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(fragShader, logLength, nullptr, &fragShaderError[0]);
    if(logLength > 1) std::cout << "Frag Errors: " << &fragShaderError[0] << std::endl;

   	std::cerr << "Linking program" << std::endl;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError( (logLength > 1) ? logLength : 1 );
    glGetProgramInfoLog(program, logLength, nullptr, &programError[0]);
    if(logLength > 1) std::cout << "Prog Errors: " <<&programError[0] << std::endl;

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
	glUseProgram(program);
    return program;
}

bool compBlockDist(glm::mat4 a, glm::mat4 b){
	glm::vec3 camPos = Options.getCamRot();
	float dista = sqrt((a[3][0]-camPos[0])+(a[3][1]-camPos[1])+(a[3][2]-camPos[2]));
	float distb = sqrt((b[3][0]-camPos[0])+(b[3][1]-camPos[1])+(b[3][2]-camPos[2]));
	return (dista>distb)?true:false;

}

//Below are functions for printing matrices.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printMatrix(glm::mat4 matrix){
	using namespace std;
	cout << "4X4 Matrix: " << endl;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			cout << matrix[j][i]  << " | ";
		}
		cout << endl;
	}	
	cout << endl;
	cout.flush();
}

void printMatrix(glm::mat3 matrix){
	using namespace std;
	cout << "4X4 Matrix: " << endl;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			cout << matrix[j][i] << " | ";
		}
		cout << endl;
	}	
	cout << endl;
	cout.flush();
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
   		std::cerr << "Printing " << infologLength << " Shader Problems: " << infoLog << std::endl;
        free(infoLog);
    }else {
    	std::cerr << "No Shader Problems." << std::endl;
    }
}
 
void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    	std::cerr << "Printing " << infologLength << " Program Problems: " << infoLog << std::endl;
        free(infoLog);
    }else{
    	std::cerr << "No Program Problems." << std::endl;
    }
}

float clamp(float min, float max, float f){
	if(f < min)
		f = min;
	if(f > max)
		f = max;
	return f;
}

double clamp(double min, double max, double f){
	if(f < min)
		f = min;
	if(f > max)
		f = max;
	return f;
}

int clamp(int min, int max, int f){
	if(f < min)
		f = min;
	if(f > max)
		f = max;
	return f;
}

int modelNamesFind(std::string s){
	int found = -1;
	for(int i=0; i<BlockNames.size(); i++){
		if(s.compare(BlockNames[i])==0)
			found = i;
	}
	if(found == -1)
		std::cerr << "Model Name Not Found!" << std::endl;
	return found;
}

int textureNamesFind(std::string s){
	int found = -1;
	for(int i=0; i<TextureNames.size(); i++){
		if(s.compare(TextureNames[i])==0)
			found = i;
	}
	if(found == -1)
		std::cerr << "Texture Name Not Found!" << std::endl;
	return found;
}


glm::mat4 alignVec(glm::vec3 a, glm::vec3 b){
	if(a == b || a == -b)
		return glm::mat4();
	a = glm::normalize(a);
	b = glm::normalize(b);
	glm::vec3 cross = glm::cross(a,b);
	float sinAngle = glm::length(cross);
	float cosAngle = glm::dot(a,b);
	glm::mat3 skew = {0.0f,-(cross.z),(cross.y),
					  (cross.z),0.0f,-(cross.x),
					  -(cross.y),(cross.x),0.0f};
	glm::mat3 rot = glm::mat3()+skew+(skew*skew)*(float)((1.0f-cosAngle)/pow(sinAngle,2));
	return glm::mat4(rot);
}