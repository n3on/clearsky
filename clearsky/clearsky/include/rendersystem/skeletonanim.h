/*
*
*/

#ifndef _CLEARSKY_JOINT_H_
#define _CLEARSKY_JOINT_H_

#include "core/utils.h"
#include "math/mathutils.h"

namespace clearsky
{
	class CLEARSKY_API Bone
	{
		public:
			Bone();
			~Bone();

			//calculate accumulatedworld matrix for child
			void calculatedMatrix();

			const char* name;
			int parentIndex;
			Bone *sibling;
			Bone *child;
			Matrix localTransform;
			Matrix combinedTranformation;
			Matrix globalTransform;

	};

	class CLEARSKY_API Skeleton
	{
		public:
			std::vector<Bone*> bones;
	};
}

#endif