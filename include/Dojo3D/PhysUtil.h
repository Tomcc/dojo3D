#pragma once

#include "common_header.h"

namespace Phys
{
	inline const Vector& asVector(const btVector3& v)
	{
		return *((const Vector*)&v); //the layout of the first 3 elements is actually the same!
	}

	inline const Quaternion& asQuaternion(const btQuaternion& q)
	{
		return *((const Quaternion*)&q); //the layout is exactly the same
	}

	inline const btVector3& asBtVector(const Vector& v)	{
		return *((const btVector3*)&v);
	}

	inline const btQuaternion& asBtQuaternion(const Quaternion& v)
	{
		return *((const btQuaternion*)&v);
	}

	inline const void convertBtTransform(const btTransform& in, Matrix& out)
	{
		in.getOpenGLMatrix(glm::value_ptr(out));
	}

	inline const void convertMatrix(const Matrix& in, btTransform & out)
	{
		out.setFromOpenGLMatrix(glm::value_ptr(in));
	}
}



