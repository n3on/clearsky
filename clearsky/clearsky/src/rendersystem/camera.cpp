/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/camera.h"

namespace clearsky
{
	Camera::Camera()
	{
		this->position = Vector3D(0.0f,0.0f,0.0f);
		this->lookDir  = Vector3D(0.0f,0.0,1.0f);
		this->up		 = Vector3D(0,1,0);
	}

	Camera::Camera(Vector3D newPos, Vector3D newDir, Vector3D newUp)
	{
		this->position	 = newPos;
		this->lookDir	 = newDir;
		this->up		 = newUp;
	}

	Camera::~Camera()
	{

	}

	void Camera::setPosition(float x, float y, float z)
	{
		this->position = Vector3D(x,y,z);
	}

	void Camera::setUp(float x, float y, float z)
	{
		this->up = Vector3D(x,y,z);
	}

	void Camera::setLookDir(float x, float y, float z)
	{
		this->lookDir = Vector3D(x,y,z);
	}

	void Camera::move(CAM_MOVE dir, float val)
	{
		double delta = GLOBALS->getDeltaTime();

		switch(dir)
		{
			case CM_FORWARD:
			{
				Vector3D look = this->lookDir-this->position;
				D3DXVec3Normalize(&look, &look);

				this->position += look*val*(float)delta;
				this->lookDir  += look*val*(float)delta;
			}break;
			case CM_BACK:
			{
				Vector3D look = this->lookDir-this->position;
				D3DXVec3Normalize(&look, &look);

				this->position -= look*val*(float)delta;
				this->lookDir  -= look*val*(float)delta;
			}break;
			case CM_LEFT:
			{
				Vector3D look = this->lookDir-this->position;
				D3DXVec3Normalize(&look, &look);

				Vector3D localX;
				D3DXVec3Cross(&localX, &look, &this->up);

				this->position += localX*val*(float)delta;
				this->lookDir  += localX*val*(float)delta;
			}break;
			case CM_RIGHT:
			{
				Vector3D look = this->lookDir-this->position;
				D3DXVec3Normalize(&look, &look);

				Vector3D localX;
				D3DXVec3Cross(&localX, &look, &this->up);

				this->position += localX*-val*(float)delta;
				this->lookDir  += localX*-val*(float)delta;
			}break;
			case CM_UP:
			{
				//this->position.x += float(val*(float)delta);
				this->position += this->up*val*(float)delta;
				this->lookDir  += this->up*val*(float)delta;
			}break;
			case CM_DOWN:
			{
				//this->position.x -= float(val*(float)delta);
				this->position -= this->up*val*(float)delta;
				this->lookDir  -= this->up*val*(float)delta;
			}break;
			case CM_TURN_RIGHT:
			{
				Vector3D look = this->lookDir-this->position;
				D3DXVec3Normalize(&look, &look);

				Matrix rotation;
				D3DXMatrixRotationAxis(&rotation, &this->up, val*(float)delta);
				
				D3DXVec3Transform((Vector4D*)&this->lookDir, &look, &rotation);

				this->lookDir += this->position;
				//D3DXVec3Normalize(&this->lookDir, &this->lookDir);
				
				//this->lookDir = (Vector3D)rotatedVec;
			}break;
			case CM_TURN_LEFT:
			{
				Vector3D look = this->lookDir-this->position;
				D3DXVec3Normalize(&look, &look);

				Matrix rotation;
				D3DXMatrixRotationAxis(&rotation, &this->up, -val*(float)delta);
				D3DXVec3Transform((Vector4D*)&this->lookDir, &look, &rotation);
				
				this->lookDir += this->position;
				//D3DXVec3Normalize(&this->lookDir, &this->lookDir);
			}break;
			case CM_TURN_UP:
			{
				Vector3D look = this->lookDir-this->position;
				D3DXVec3Normalize(&look, &look);

				Vector3D localX;
				D3DXVec3Cross(&localX, &look, &this->up);
				D3DXVec3Normalize(&localX, &localX);
				
				Matrix rotation;
				//D3DXMatrixRotationX(&rotation, -val*delta);
				D3DXMatrixRotationAxis(&rotation,&localX,val*(float)delta);
				D3DXVec3Transform((Vector4D*)&this->lookDir, &look, &rotation);

				this->lookDir += this->position;
			}break;
			case CM_TURN_DOWN:
			{
				Vector3D look = this->lookDir-this->position;
				D3DXVec3Normalize(&look, &look);

				Vector3D localX;
				D3DXVec3Cross(&localX, &look, &this->up);
				D3DXVec3Normalize(&localX, &localX);

				Matrix rotation;
				D3DXMatrixRotationAxis(&rotation, &localX, -val*(float)delta);

				D3DXVec3Transform((Vector4D*)&this->lookDir, &look, &rotation);

				this->lookDir += this->position;
			}break;
		}//end switch dir

		//TODO: cleanup this ugly class
		/*Vector3D look = this->lookDir-this->position;
		D3DXVec3Normalize(&look, &look);
		//D3DXVec3Cross(&this->rightDir, &this->lookDir, &this->up);
		//D3DXVec3Normalize(&this->rightDir, &this->rightDir);
		this->lookDir = look;

		Vector3D right;
		D3DXVec3Cross(&right, &look, &this->up);
		D3DXVec3Normalize(&right, &right);
		this->rightDir = right;
		*/

		/*Vector3D newUp;
		D3DXVec3Cross(&newUp, &this->rightDir, &this->lookDir);
		D3DXVec3Normalize(&newUp, &newUp);
		this->up = newUp;*/
	}//end move func

	void Camera::move(Vector3D moveDir, float val)
	{
		double delta = GLOBALS->getDeltaTime();

		Vector3D look = moveDir-this->position;
		D3DXVec3Normalize(&look, &look);

		this->position += look*val*(float)delta;
		this->lookDir  += look*val*(float)delta;
	}

	void Camera::move(Vector3D moveDir)
	{


		double delta = GLOBALS->getDeltaTime();

		this->position += moveDir*(float)delta;
		this->lookDir  += moveDir*(float)delta;
	}

	Vector3D Camera::getLookDir()
	{
		Vector3D look = this->lookDir-this->position;
		D3DXVec3Normalize(&look, &look);

		return look;
	}

	Matrix Camera::getRotationMatrix()
	{		
		Vector3D look = this->lookDir-this->position;
		D3DXVec3Normalize(&look, &look);
		//this->lookDir = look;

		Vector3D right;
		D3DXVec3Cross(&right, &look, &this->up);
		D3DXVec3Normalize(&right, &right);
		this->rightDir = right;

		Vector3D newUp;
		D3DXVec3Cross(&newUp, &right, &look);
		D3DXVec3Normalize(&newUp, &newUp);
		//this->up = newUp;

		D3DXVec3Normalize(&this->up, &this->up);

		Matrix rotationMatrix(right.x, right.y, right.z,0,
								newUp.x, newUp.y, newUp.z,0,
								look.x, look.y, look.z,0,
								0,0,0,1);

		/*Matrix rotationMatrix(right.x, this->up.x,look.x, 0,
							  right.y, this->up.y, look.y,0,
							  right.z, this->up.z, look.z,0,
							  0,0,0,1);*/
		
		return rotationMatrix;
	}

}