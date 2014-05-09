/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/skeletonanim.h"

namespace clearsky
{
	Bone::Bone():sibling(NULL),
				 child(NULL),
				 name(NULL)
	{

	}

	Bone::~Bone()
	{
		SAFE_DELETE(sibling);
		SAFE_DELETE(child);
		SAFE_ARRAY_DELETE(name);
	}

	void Bone::calculatedMatrix()
	{
		//iterate through all sibling and set the matrices
		Bone *curBone = this;
		while(curBone && curBone->sibling)
		{
			MatrixMultiply(&curBone->sibling->combinedTranformation, &curBone->combinedTranformation, &curBone->sibling->localTransform);

			curBone = curBone->sibling;
		}

		//iterate through all children and set the matrices
		curBone = this;
		while(curBone && curBone->child)
		{
			MatrixMultiply(&curBone->child->combinedTranformation, &curBone->combinedTranformation, &curBone->sibling->localTransform);
			curBone = curBone->child;
		}
	}
}