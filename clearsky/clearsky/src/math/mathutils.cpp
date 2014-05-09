/*
*
*/

#include "core/precompiled.h"
#include "math/mathutils.h"

namespace clearsky
{
	Matrix* MatrixIdentity(Matrix *out)
	{
		return D3DXMatrixIdentity(out);
	}

	Matrix* MatrixMultiply(Matrix *out, const Matrix *matrix1, const Matrix *matrix2)
	{
		return D3DXMatrixMultiply(out, matrix1, matrix2);
	}

	Matrix* MatrixTranslation(Matrix *out, float x, float y, float z)
	{
		return D3DXMatrixTranslation(out, x, y, z);
	}

	Matrix* MatrixRotation(Matrix *out, float x, float y, float z)
	{
		return D3DXMatrixRotationYawPitchRoll(out,x,y,z);
	}

	Matrix* MatrixScale(Matrix *out, float x, float y, float z)
	{
		return D3DXMatrixScaling(out,x,y,z);
	}

	Vector3D* VectorTransform(Vector3D *out, const Vector3D *in, const Matrix *matrix)
	{
		return D3DXVec3TransformCoord(out,in,matrix);
	}

}
