#include "Transform.hpp"

#include "Core/Core.hpp"
#include "Core/Math/Ext.hpp"
#include "Core/Log/Logger.hpp"


namespace Components
{
	Mat4F Transform::GetTransformation()
	{
		// The matrix represents an affine transformation composed of rotation, scaling, and translation:
		//     M = T * RS
		//
		// The transformation matrix RS (rotation * scaling) is obtained as:
		//     RS = R * S
		//
		// 1. The scaling matrix S is a 3x3 diagonal matrix:
		//     | sx  0   0  |
		//     | 0   sy  0  |
		//     | 0   0   sz |
		auto S = Mat3F{
			scale.x, 0,				0,
			0,			 scale.y, 0,
			0,			 0,				scale.z,
		};

		// 2. The rotation matrix R is obtained by combining 3 elementary rotations around the axes,
		//     R = Rz * Ry * Rx
		//
		//  Where:
		//     Rx = rotation around the X axis (roll)
		//     Ry = rotation around the Y axis (pitch)
		//     Rz = rotation around the Z axis (yaw)
		auto radians = Vec3F(glm::radians(eulerAngles));
		auto R = Mat3F(glm::eulerAngleXYZ(
			radians.x,
			radians.y,
			radians.z
		));
		auto RS = Mat3F(R * S);


		// M = 
		//     | rs11  rs12  rs13  xt |
		//     | rs21  rs22  rs23  yt |
		//     | rs31  rs32  rs33  zt |
		//     | 0     0     0     1  |
		// 
		// - The 3x3 matrix represents the transformation (rotation and scaling).
		// - xt, yt, zt are the components of the translation vector.
		// - The last row is fixed to maintain homogeneous coordinates.
		//
		// OpenGL and GLM store matrices in column-major order, so we need to 
		// create the transposed matrix M:
		//     | rs11  rs21  rs31  0 |
		//     | rs12  rs22  rs32  0 |
		//     | rs13  rs23  rs33  0 |
		//     | xt    yt    zt		 1 |
		auto M = Mat4F{
			Vec4F(RS[0], 0.0f),
			Vec4F(RS[1], 0.0f),
			Vec4F(RS[2], 0.0f),
			Vec4F(position, 1.0f)
		};
		return M;
	}
}


