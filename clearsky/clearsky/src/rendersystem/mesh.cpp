/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/mesh.h"

namespace clearsky
{
	MOFMesh::MOFMesh():m_meshes(NULL),
					 m_numMeshes(0),
					 m_file(NULL),
					 m_baseFileAddr(NULL)
	{
		this->m_id = rand64();
		this->m_name = "Mesh";
	}

	MOFMesh::~MOFMesh()
	{
		this->release();
	}

	void MOFMesh::release()
	{
		for(int i=0; i<this->m_numMeshes; ++i)
		{
			SAFE_DELETE(this->m_meshes[i]);
		}

		SAFE_ARRAY_DELETE(this->m_meshes);
	}

	void MOFMesh::draw()
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			draw(i);

	}

	void MOFMesh::draw(int meshNum)
	{
		if(meshNum<this->m_numMeshes)
		{
			//printf("DRAW MESH %i",meshNum);
			MOFSubmesh *mesh = this->m_meshes[meshNum];

			mesh->draw();
		}
	}

	void MOFMesh::rotateY(float deg)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->rotateY(deg);
	}

	void MOFMesh::rotateX(float deg)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->rotateX(deg);
	}

	void MOFMesh::rotateZ(float deg)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->rotateZ(deg);
	}

	void MOFMesh::translate(float x, float y, float z)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->translate(x,y,z);
	}

	void MOFMesh::scale(float factorX, float factorY, float factorZ)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->scale(factorX, factorY, factorZ);
	}

	void MOFMesh::transform(Matrix *transformMat)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->transform(transformMat);
	}

	void MOFMesh::setRotation(float degX, float degY, float degZ)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->setRotation(degX, degY, degZ);
	}

	void MOFMesh::setPosition(float x, float y, float z)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->setPosition(x, y, z);
	}

	void MOFMesh::setSize(float factorX, float factorY, float factorZ)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->setSize(factorX, factorY, factorZ);
	}

	void MOFMesh::resetTransform()
	{
		for(int i=0; i<this->m_numMeshes; ++i)
			this->m_meshes[i]->resetTransform();
	}

	RETURN_VALUE MOFMesh::load(const char* filename)
	{
		//printf("===============MOF: Load file==================\n");
		printf("\nRead MOF File: %s\n",filename);

		fopen_s(&this->m_file,filename, "rb");
		if(!this->m_file)
			return RETURN_ERROR;
		
		this->m_filename = std::string(filename);
		//extract base addr from filename
		this->m_baseFileAddr = new char[strlen(filename)+1];
		strncpy_s(this->m_baseFileAddr,strlen(filename)+1,filename,strlen(filename)+1);
		bool foundSlash = false;
		for(int i=strlen(this->m_baseFileAddr)-1; i>=0; --i)
		{
			if(this->m_baseFileAddr[i]=='\\' || (this->m_baseFileAddr[i]=='/'))
			{
				this->m_baseFileAddr[i+1] = '\0';
				foundSlash = true;
				break;
			}
		}

		int readBytes;
		MOF_HEADER header;
		readBytes = fread(&header.id,1,1,this->m_file);
		readBytes = fread(&header.version,1,1,this->m_file);
		readBytes = fread(&header.endianess,1,1,this->m_file);
		readBytes = fread(&header.numMeshes,4,1,this->m_file);
		//if(readBytes!=sizeof(MOF_HEADER))
		//	return RETURN_ERROR;

		/*printf("id: %x\n", header.id);
		printf("version: %x\n", header.version);
		printf("endianess: %x\n", header.endianess);
		printf("numMeshes: %x\n", header.numMeshes);
		*/
		//printf("\nMOF HEADER\n");
			//printf("  id: %x \n  version: %x\n  numMeshes: %x\n",header.id,header.version,header.numMeshes);

		if((header.id!=MOF_FILE_ID)||(header.version!=MOF_CURRENT_VERSION))
			return RETURN_ERROR;

		this->m_meshes = new MOFSubmesh*[header.numMeshes];
		this->m_numMeshes = header.numMeshes;

		MOF_MESH_CHUNK_HEADER meshHeader;
		//for(int i=0; i<header.numMeshes; ++i)
		//while(!feof(this->m_file))
		{
			//fread(&meshHeader.id,1,1,this->m_file);
			//fread(&meshHeader.size,4,1,this->m_file);

			//printf("read mesh[%d]\n",i, meshHeader.id, meshHeader.size);

			//if(meshHeader.id == MOF_MESH_CHUNK)
			//{
				//printf("\nMOF READ MESH[%d]\n",i);
				//	printf("  id: %x \n  size: %x\n\n",meshHeader.id,meshHeader.size);

				if(this->readMesh(&header, &meshHeader, -1)!=RETURN_OK)
					return RETURN_ERROR;
			//}
		}

		//printf("\n=======================MOF OBJECT Loaded!!=======================\n");
		fclose(this->m_file);

		return RETURN_OK;
	}

	RETURN_VALUE MOFMesh::readMesh(MOF_HEADER *header, MOF_MESH_CHUNK_HEADER *meshHeader, int meshNum)
	{
		//this->m_meshes[meshNum] = new MOFSubmesh;

		int readBytes=0;

		MOF_CHUNK_HEADER chunkHeader;
		fread(&chunkHeader.id,1,1,this->m_file);
		fread(&chunkHeader.size,4,1,this->m_file);

		while(!feof(this->m_file))
		{
			//if(feof(this->m_file))
			//	break;

			//printf("\nMOF read chunk header\n");
			//printf("  MOF chunk header id: %d[hex: %x]\n  size: %x\n",chunkHeader.id,chunkHeader.id,chunkHeader.size);

			int byteCount=0;
			switch(chunkHeader.id)
			{
				case MOF_MESH_CHUNK:
				{
					++meshNum;
					this->m_meshes[meshNum] = new MOFSubmesh;

					char nameLength;
					byteCount += fread(&nameLength,1,1,this->m_file);
					printf("  Mesh name Length: %d\n",nameLength);
						
					//TODO: check case where namelength==0
					char *textureName = new char[nameLength];
					byteCount += fread(textureName,1,nameLength,this->m_file);
					if(nameLength)
						textureName[nameLength] = '\0';

					this->m_meshes[meshNum]->setName(textureName);
					printf("  Mesh name: %s\n",this->m_meshes[meshNum]->getName());
					//SAFE_ARRAY_DELETE(textureName);
				}break;
				case MOF_VERTEX_CHUNK_ID:
				{
					fread(&chunkHeader.num,4,1,this->m_file);
					byteCount = this->loadVertexChunk(&chunkHeader, meshNum);
					if(byteCount<0)
						return RETURN_ERROR;

					readBytes += byteCount;
				}break;
				case MOF_INDEX_CHUNK_ID:
				{
					fread(&chunkHeader.num,4,1,this->m_file);
					byteCount = this->loadIndexChunk(&chunkHeader, meshNum);
					if(byteCount<0)
						return RETURN_ERROR;

					readBytes += byteCount;
					
				}break;
				case MOF_MATERIAL_CHUNK_ID:
				{
					fread(&chunkHeader.num,4,1,this->m_file);
					byteCount = this->loadMaterialChunk(&chunkHeader, meshNum);
					readBytes += byteCount;
				}break;
				default:
				{
					//printf("\nUNKNOWN CHUNK\n");
					fseek(this->m_file,chunkHeader.size,SEEK_CUR);
					readBytes += chunkHeader.size;
				}break;
			}

			fread(&chunkHeader.id,1,1,this->m_file);
			fread(&chunkHeader.size,4,1,this->m_file);
		}

		return RETURN_OK;
	}

	int MOFMesh::loadVertexChunk(MOF_CHUNK_HEADER *header,int meshNum)
	{
		if(!this->m_file)
			return RETURN_ERROR;

		int byteCount;

		char numElements=0;
		byteCount = fread(&numElements,1,1,this->m_file);
			//return RETURN_ERROR;

		char topology=0;
		byteCount = fread(&topology,1,1,this->m_file);

		char windingOrder=0;
		byteCount = fread(&windingOrder,1,1,this->m_file);
		//if(header->num<=0)
		//	return RETURN_ERROR;

		MOFSubmesh *mesh = this->m_meshes[meshNum];

		if(windingOrder)
			mesh->setFrontOrder(true);
		else
			mesh->setFrontOrder(false);

		mesh->setNumVertices(header->num);
		mesh->setNumElementsVertex(numElements);

		Vertex *verts = new Vertex[header->num];
		//int count=sizeof(header->id)+sizeof(header->num)+sizeof(header->size)+sizeof(numElements);
		//int readBytes=0;

		//printf("\nMOF_VERTEX_CHUNK: \n");
		//printf("  numVertices: %d(hex: %x)\n  numElements %d(hex: %x)\n",header->num,header->num,numElements,numElements);

		char typeId;
		for(int i=0; i<header->num; ++i)
		{
			for(int j=0; j<numElements; ++j)
			{
				byteCount += fread(&typeId,1,1,this->m_file);

				switch(typeId)
				{
					case MOF_VERTEX_POSITION:
					{
						float vertVals[3];
				
						byteCount += fread(&vertVals,4,3,this->m_file);
						
						/*printf("\nMOF read %d Position: %f %f %f\n",i,vertVals[0],
								vertVals[1],
								vertVals[2]);*/

						Vector3D position(vertVals);
						verts[i].position = position;
					}break;
					case MOF_VERTEX_NORMAL:
					{
						float normVals[3];

						byteCount += fread(&normVals,4,3,this->m_file);
		
						Vector3D normal(normVals);
						D3DXVec3Normalize(&normal, &normal);
						verts[i].normal = normal;

						/*printf("\nMOF read %d Normals: %f %f %f\n",i,verts[i].normal.x,
								verts[i].normal.y,
								verts[i].normal.z);*/
					}break;
					case MOF_VERTEX_TANGENT:
					{
						float tangentVals[3];

						byteCount += fread(&tangentVals,4,3,this->m_file);
		
						Vector3D tangent(tangentVals);
						D3DXVec3Normalize(&tangent, &tangent);
						verts[i].tangent = tangent;

						//printf("\nMOF read %d Tangent: %f %f %f\n",i,verts[i].tangent.x,
						//		verts[i].tangent.y,
						//		verts[i].tangent.z);
					}break;
					case MOF_VERTEX_TEX:
					{
						float texVals[2];

						byteCount += fread(&texVals,4,2,this->m_file);

						Vector2D tex(texVals);
						//tex.x = 1-tex.x;
						tex.y = 1-tex.y;
						verts[i].texCoord0 = tex;

						//printf("\nMOF read %d Texture Coords: %f %f\n",i,texVals[0],texVals[1]);

					}break;
					case MOF_VERTEX_COLOR:
					{
						float colVals[4];

						byteCount += fread(&colVals,4*4,1,this->m_file);

						Color color(colVals);
						//verts[i].color = color;
					}break;
					default:
					{
					};

				}//end switch
					
			}//end for numelements
		}//end for num

		//TODO: improve codepaths to avoid multiple SAFE_DELETE stuff

		IBuffer *vertexBuffer = GLOBALS->getRenderer()->createBuffer();
		if(!vertexBuffer)
		{
			SAFE_DELETE(vertexBuffer);
			SAFE_ARRAY_DELETE(verts);
			return -1;
		}

		//mesh.setVertexBuffer(vertexBuffer);
		if(vertexBuffer->create(verts,sizeof(Vertex),header->num)!=RETURN_OK)
		{
			SAFE_DELETE(vertexBuffer);
			SAFE_ARRAY_DELETE(verts);
			return -1;
		}

		mesh->setVertexBuffer(vertexBuffer);

		//generate bounding box for the submesh
		AABB submeshAABB;
		submeshAABB.loadFromPosition(verts, header->num);
		mesh->setBoundingBox(submeshAABB);

		SAFE_ARRAY_DELETE(verts);
		return byteCount;
	}

	int MOFMesh::loadIndexChunk(MOF_CHUNK_HEADER *header, int meshNum)
	{
		//printf("MOF Load Index Chunk\n");

		int byteCount=0;
		MOFSubmesh *mesh = this->m_meshes[meshNum];

		mesh->setNumIndices(header->num);

		//printf("MOF num indices %i\n",header->num);

		char indexType = 0;
		byteCount += fread(&indexType,1,1,this->m_file);

		int *indexData = new int[header->num];

		for(int i=0;i<header->num;++i)
		{
			int index;
			int num;
			//if(indexType==1)
			num = fread(&index,1,4,this->m_file);
			//else if(indexType==0)
			//	num = fread(&index,1,2,this->m_file);

			byteCount +=num; 
			if(num!=4)
			{
				SAFE_ARRAY_DELETE(indexData);
				return RETURN_ERROR;
			}

			indexData[i] = index;

			//printf("MOF index[%i] %i\n",i,index);
		}


		IBuffer *indexBuffer = GLOBALS->getRenderer()->createBuffer();
		if(!indexBuffer)
		{
			SAFE_ARRAY_DELETE(indexData);
			return -1;
		}

		/*if(indexType==0)
		{
			printf("16!!\n");
			if(indexBuffer->create(indexData,2,header->num,BT_INDEX, FORMAT_R32_UINT)!=RETURN_OK)
				return -1;
		}
		else if(indexType==1)
		{
			printf("32!!\n");*/
		if(indexBuffer->create(indexData,4,header->num,BT_INDEX)!=RETURN_OK)
		{
			SAFE_ARRAY_DELETE(indexData);
			return -1;
		}
		//}

		mesh->setIndexBuffer(indexBuffer);

		SAFE_ARRAY_DELETE(indexData);
		return byteCount;
	}

	int MOFMesh::loadMaterialChunk(MOF_CHUNK_HEADER *header, int meshNum)
	{
		//printf("\nMesh%i Load Material Chunk\n", meshNum);

		int byteCount = 0;

		MOFSubmesh *mesh = this->m_meshes[meshNum];

		//printf("   num Materials: %d\n",header->num);

		
		for(int i=0; i<header->num; ++i)
		{
			Material *material=new Material;

			byteCount += fread(&material->id,1,1,this->m_file);
			byteCount += fread(&material->startFace,4,1,this->m_file);
			byteCount += fread(&material->numFaces,4,1,this->m_file);

			//printf("  Material%d StartFace=%d NumFaces=%d\n",material->id, material->startFace, material->numFaces);

			char typeId = 0;
			while(typeId!=MOF_MATERIAL_END_ENUMERATION)
			{
				byteCount += fread(&typeId,1,1,this->m_file);
				//printf("  Type: %d\n",typeId);
				switch(typeId)
				{
					case MOF_MATERIAL_TEXTURE:
					{
						char nameLength;
						byteCount += fread(&nameLength,1,1,this->m_file);
						//printf("  Diffuse Texturename length: %x\n",nameLength);
						
						char *textureName = NULL;
						textureName = new char[nameLength];
						byteCount += fread(textureName,1,nameLength,this->m_file);
						textureName[nameLength] = '\0';
						//printf("  bla");
						//append base directory
						char fileDir[256];
						strncpy_s(fileDir,this->m_baseFileAddr,120);
						strncat_s(fileDir,textureName, 120);
						//printf("  bla2");

						/*int strSize = MultiByteToWideChar(CP_ACP,0,fileDir ,strlen(fileDir)+1,NULL,0);
						wchar_t *newTexFilename = new wchar_t[strSize];
						MultiByteToWideChar(CP_ACP,0,fileDir,strlen(fileDir)+1,newTexFilename,strSize);
						*/
						//material->texture = GLOBALS->getRenderer()->createTexture2D();
						//printf("  bla3 %s\n",fileDir);
						material->texture = TextureResource->loadTexture2D(fileDir);
						/*if(material->texture->loadFile(fileDir)!=RETURN_OK)
							material->texture = NULL;
						else
							mesh->addTexture2D(material->texture);*/

						if(material->texture)
							mesh->addTexture2D(material->texture);

						//strncpy_s(material->textureName,textureName,49);
						printf("  Diffuse Texture file: %s\n",fileDir);

						//SAFE_ARRAY_DELETE(textureName);

					}break;
					//TODO: remove code dublication
					case MOF_MATERIAL_SPECULAR_TEXTURE:
					{
						char nameLength;
						byteCount += fread(&nameLength,1,1,this->m_file);
						//printf("  Specular Texturename length: %x\n",nameLength);
						
						char *textureName = NULL;
						textureName = new char[nameLength];
						byteCount += fread(textureName,1,nameLength,this->m_file);
						textureName[nameLength] = '\0';
						//printf("  bla");
						//append base directory
						char fileDir[256];
						strncpy_s(fileDir,this->m_baseFileAddr,120);
						strncat_s(fileDir,textureName, 120);
						//printf("  bla2");

						material->specularTexture = GLOBALS->getRenderer()->createTexture2D();
						//printf("  bla3 %s\n",fileDir);
						
						//TODO: use resource loader, to prevent texture dublication
						if(material->specularTexture->loadFile(fileDir)!=RETURN_OK)
							material->specularTexture = NULL;
						else
							mesh->addSpecTexture2D(material->specularTexture);

						//strncpy_s(material->textureName,textureName,49);
						printf("  Specular Texture file: %s\n",fileDir);

						//SAFE_ARRAY_DELETE(textureName);

					}break;
					case MOF_MATERIAL_HEIGHT_TEXTURE:
					{
						char nameLength;
						byteCount += fread(&nameLength,1,1,this->m_file);
						//printf("  Height Texturename length: %x\n",nameLength);
						
						char *textureName = NULL;
						textureName = new char[nameLength];
						byteCount += fread(textureName,1,nameLength,this->m_file);
						textureName[nameLength] = '\0';
						//printf("  bla");
						//append base directory
						char fileDir[256];
						strncpy_s(fileDir,this->m_baseFileAddr,120);
						strncat_s(fileDir,textureName, 120);
						//printf("  bla2");

						//material->heightmapTexture = GLOBALS->getRenderer()->createTexture2D();
						//printf("  bla3 %s\n",fileDir);
						
						//TODO: use resource loader, to prevent texture dublication
						material->heightmapTexture = TextureResource->loadTexture2D(fileDir);
						/*if(material->heightmapTexture->loadFile(fileDir)!=RETURN_OK)
							material->heightmapTexture = NULL;
						else
							mesh->addHeightTexture2D(material->heightmapTexture);*/
						if(material->heightmapTexture)
							mesh->addHeightTexture2D(material->heightmapTexture);

						//strncpy_s(material->textureName,textureName,49);
						printf("  heightmap Texture file: %s\n",fileDir);

						//SAFE_ARRAY_DELETE(textureName);

					}break;
					case MOF_MATERIAL_DIFFUSE:
					{
						float diffuseColor[4];

						byteCount += fread(diffuseColor,4,4,this->m_file);

						//printf("  Diffuse Color: %f %f %f %f\n",diffuseColor[0], diffuseColor[1], diffuseColor[2],diffuseColor[3]);

						//set transparent value
						//diffuseColor[3]=1;

						Color color(diffuseColor);
						material->diffuse = color;
					}break;
					case MOF_MATERIAL_SPECULAR:
					{
						float specularColor[4];

						byteCount += fread(specularColor,4,4,this->m_file);

						//printf("  Specular Color: %f %f %f %f\n",specularColor[0], specularColor[1], specularColor[2],specularColor[3]);

						//set transparent value
						specularColor[3]=1;

						Color color(specularColor);
						material->specular = specularColor;
					}break;
					case MOF_MATERIAL_AMBIENT:
					{
						float ambientColor[4];

						byteCount += fread(ambientColor,4,4,this->m_file);

						//printf("  Ambient Color: %f %f %f %f\n",ambientColor[0], ambientColor[1], ambientColor[2],ambientColor[3]);

						//set transparent value
						ambientColor[3]=1;

						Color color(ambientColor);
						material->ambient = ambientColor;
					}break;
					case MOF_MATERIAL_END_ENUMERATION:
					{
						//printf("end of enum\n");
						break;
					}break;
					
				}//end switch

				//byteCount += fread(&typeId,1,1,this->m_file);
			}//end while

			mesh->addMaterial(material);
			
		}//end for
		//char typeId=0;
		//byteCount += fread(&typeId,1,1,this->m_file);
		return 0;
	}

	RETURN_VALUE MOFMesh::save(const char* filename)
	{
		FILE* output;
		
		fopen_s(&output,filename,"wb");
		if(!output)
			return RETURN_ERROR;

		MOF_HEADER header;
		header.id		= MOF_FILE_ID;
		header.version	= MOF_CURRENT_VERSION;
		header.numMeshes = this->m_numMeshes;

		//printf("\nMOF Before header\n");
		fwrite(&header,sizeof(MOF_HEADER),1,output);
		//printf("\nMOF after\n");

		RETURN_VALUE result = RETURN_OK;
		for(int i=0; i<this->m_numMeshes; ++i)
		{
			result = writeMesh(output, i);
			if(result != RETURN_OK)
				return result;
		}

		fclose(output);
		return RETURN_OK;
	}

	RETURN_VALUE MOFMesh::writeMesh(FILE *file, int meshNum)
	{
		RETURN_VALUE result;
		result = writeVertexChunk(file, meshNum);
		if(result!=RETURN_OK)
			return result;

		result = writeIndexChunk(file, meshNum);
		if(result!=RETURN_OK)
			return result;

		result = writeMaterialChunk(file, meshNum);
		if(result!=RETURN_OK)
			return result;
	
		return RETURN_OK;
	}

	RETURN_VALUE MOFMesh::writeVertexChunk(FILE *file, int meshNum)
	{
		MOFSubmesh *mesh = this->m_meshes[meshNum];

		//printf("\nMOF writeVertexChunk\n");
		MOF_CHUNK_HEADER vertexHeader;
		vertexHeader.id  = MOF_VERTEX_CHUNK_ID;
		vertexHeader.num = mesh->getNumVertices();
		//TODO enable flexible formats
		vertexHeader.size = vertexHeader.num*sizeof(Vertex);
		
		fwrite(&vertexHeader,sizeof(MOF_CHUNK_HEADER),1,file);

		//save number of vertex type elements
		char numElements = 4;
		fwrite(&numElements, 1,1,file);
		//printf("\nMOF Num Elements written\n");

		Vertex *vertexData = NULL;//mesh->getVertexData();

		if(!vertexData)
			return RETURN_ERROR;

		//Vertex* vertexData = 0;
		//this->m_vertexBuffer->map((void**)&vertexData);
		//if(!vertexData)
		//	return RETURN_ERROR;

		//printf("\nMOF 1\n");
		for(int i=0; i<mesh->getNumVertices(); ++i)
		{
			//printf("Write Vertex %d\n",i);

			//save position
			char typeId = MOF_VERTEX_POSITION;
			fwrite(&typeId,1,1,file);
			float position[3] = {vertexData[i].position.x,
								vertexData[i].position.y,
								vertexData[i].position.z};
			fwrite(position,3*4,1,file);


			typeId = MOF_VERTEX_NORMAL;
			fwrite(&typeId,1,1,file);
			float normals[3] = {vertexData[i].normal.x,
								vertexData[i].normal.y,
								vertexData[i].normal.z};
			fwrite(&normals,3*4,1,file); 

			typeId = MOF_VERTEX_TEX;
			fwrite(&typeId,1,1,file);
			float texCoord0[2] = {vertexData[i].texCoord0.x,
								vertexData[i].texCoord0.y};
			fwrite(&texCoord0,2*4,1,file); 

			typeId = MOF_VERTEX_COLOR;
			fwrite(&typeId,1,1,file);
			///fwrite(&vertexData[i].color,4*4,1,file); 

		}

		//this->m_vertexBuffer->unmap();

		return RETURN_OK;
	}

	RETURN_VALUE MOFMesh::writeIndexChunk(FILE *file, int meshNum)
	{
		MOFSubmesh *mesh = this->m_meshes[meshNum];

		if(!mesh->getIndexBuffer())
			return RETURN_ERROR;

		//printf("\nMOF writeIndexChunk\n");
		MOF_CHUNK_HEADER indexHeader;
		indexHeader.id  = MOF_INDEX_CHUNK_ID;
		indexHeader.num = mesh->getNumIndices();
		indexHeader.size = indexHeader.num*sizeof(int);
		
		fwrite(&indexHeader,sizeof(MOF_CHUNK_HEADER),1,file);

		//fwrite(mesh->getIndexData(),sizeof(int),mesh->getNumIndices(),file);

		return RETURN_OK;
	}

	RETURN_VALUE MOFMesh::writeMaterialChunk(FILE *file, int meshNum)
	{

		return RETURN_OK;
	}

	MOFSubmesh *MOFMesh::getSubmesh(int meshNum)
	{
		if(meshNum>this->m_numMeshes)
			return NULL;

		return this->m_meshes[meshNum];
	}

	AABB MOFMesh::getBoudingBox()
	{
		AABB aabb;
		Vector3D min;
		Vector3D max;

		if(this->m_numMeshes>0)
			aabb = this->getSubmesh(0)->getBoudingBox();

		for(int i=0; i<this->m_numMeshes; ++i)
		{
			MOFSubmesh *smesh;
			smesh = this->getSubmesh(i);

			AABB sAABB = smesh->getBoudingBox();
			aabb.load(sAABB);
		}

		return aabb;
	}

	bool MOFMesh::rayCollision(const Vector3D rayOrigin, const Vector3D rayDirection)
	{
		for(int i=0; i<this->m_numMeshes; ++i)
		{
			if(this->m_meshes[i]->rayCollision(rayOrigin, rayDirection))
				return true;
		}

		return false;
	}

	void MOFMesh::setName(std::string name)
	{
		this->m_name = name;
	}

	std::string MOFMesh::getName()
	{
		return this->m_name;
	}

	std::string MOFMesh::getFilename()
	{
		return this->m_filename;
	}

	void MOFMesh::setId(unsigned long long id)
	{
		this->m_id = id;
	}

	unsigned long long MOFMesh::getId()
	{
		return this->m_id; 
	}

}
