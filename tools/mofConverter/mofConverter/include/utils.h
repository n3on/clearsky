/*
*(C)2011 Symplatonix Software
*/

#ifndef _UTILS_H_
#define _UTILS_H_

#include <sstream>
#include <string>
#include <math.h>

using namespace std;

#define BACKSLASH 0x5C

struct Vector3D
{
	Vector3D()
	{
		x=y=z=0;
	}

	bool equal(const Vector3D *vec, float error=0.001f)
	{
		return (fabs(vec->x-this->x)<error) &&
			   (fabs(vec->y-this->y)<error) &&
			   (fabs(vec->z-this->z)<error);
	}

	void set(const Vector3D *vec)
	{
		this->x = vec->x;
		this->y = vec->y;
		this->z = vec->z;
	}

	std::string toString()
	{
		std::ostringstream vectorStr;
		vectorStr << x;
		vectorStr <<",";
		vectorStr <<y;
		vectorStr <<",";
		vectorStr <<z;
		vectorStr <<":";

		/*std::string vertexStr;
		vertexStr = (char)x;
		vertexStr +=",";
		vertexStr +=(char)y;
		vertexStr +=",";
		vertexStr +=(char)z;
		vertexStr +=":";*/

		return vectorStr.str();
	}

	float x;
	float y;
	float z;
};

struct Vertex
{
	Vertex()
	{

	}

	void setPosition(Vector3D *position)
	{
		this->pos.set(position);
	}

	void setNormal(Vector3D *norm)
	{
		this->normal.set(norm);
	}
	
	void setTexUV(Vector3D *texUV)
	{
		this->texUV.set(texUV);
	}

	bool equal(Vertex *vertex)
	{
		return (this->pos.equal(&vertex->pos) && 
				this->normal.equal(&vertex->normal) && 
				this->texUV.equal(&vertex->texUV));
	}

	std::string toString()
	{
		return pos.toString()+normal.toString()+texUV.toString();
	}

	Vector3D pos;
	Vector3D normal;
	Vector3D texUV;
	Vector3D tangent;
};

struct Face
{
	Face():index1(0),
			index2(0),
			index3(0),
			matId(-1)
	{
	}

	//indices of face triangle
	unsigned int index1;
	unsigned int index2;
	unsigned int index3;

	/*//compares material ids, need for material sorting
	bool operator<(Face face) 
	{ 
		return (this->matId < face.matId); 
	}*/

	int matId; //material
};

struct Color
{
	Color()
	{
		r=g=b=0.0f;
		a=1.0f;
	}

	Color(float red, float green, float blue, float alpha)
	{
		r=red;
		g=green;
		b=blue;
		a=alpha;
	}

	float r;
	float g;
	float b;
	float a;
};

struct Material
{
	Material()
	{
		startFace = 0;
		numFaces  = 0;
		id=-1;
	}

	Color diffuse; //global scattered light
	Color specular;
	Color ambient;	
	Color emissive; //color that emits the object itself

	char id;
	string name;
	int startFace;
	int numFaces;
	string textureName;
	string specTexName;
	string normalTexName;
	string heightTexName;
};

#endif
