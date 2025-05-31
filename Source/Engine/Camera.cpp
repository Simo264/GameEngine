#include "Engine/Camera.hpp"

#include "Core/Math/Ext.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Managers/WindowManager.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
 */

Camera::Camera()
{
	position = Vec3F(0.f);
	eulerAngles = Vec3F(0.f);
}

Mat4f Camera::GetViewMatrix() const
{
	// Construct the camera matrix using the camera's right (u), up (v), and forward (w) vectors.
	// The matrix represents the camera's position and orientation in world space.
	// We take its inverse to transform world coordinates into the camera's local space.
	// M_cam = 
	//  | ux vx wx ex |				| ux uy uz 0 |	 | 1 0 0 -ex |
	//  | uy vy wy ey |				| vx vy vz 0 |	 | 0 1 0 -ey |
	//  | uz vz wz ez |^-1 =	| wx wy wz 0 | * | 0 0 1 -ez |
	//  | 0  0  0   1 |				| 0  0  0  1 |	 | 0 0 0   1 |
	Vec3F forward = GetForwardVector();
	Vec3F right = GetRightVector();
	Vec3F up = GetUpVector();

	Vec3F w = glm::normalize(-forward);						// z
	Vec3F u = glm::normalize(glm::cross(up, w));	// x
	Vec3F v = glm::normalize(glm::cross(w, u));   // y
	Mat4f M_cam = glm::inverse(glm::mat4{
		u.x, v.x, w.x, position.x,
		u.y, v.y, w.y, position.y,
		u.z, v.z, w.z, position.z,
		0, 0, 0, 1
	});
	M_cam = glm::transpose(M_cam); // from row-major to column-major order
	return M_cam;
}

Mat4f Camera::GetPerspectiveProjection(f32 fovy, f32 aspect, f32 n, f32 f) const
{
	// Constructs a matrix that projects the view volume (frustum)
	// defined by fovy, aspect, near, and far into the OpenGL canonical volume [-1, 1]^3. 
	//
	// M_per =
	//   | (2n)/(r-l),  0,          (l+r)/(l-r), 0           |
	//   | 0,           (2n)/(t-b), (b+t)/(b-t), 0           |
	//   | 0,           0,          (f+n)/(n-f), (2fn)/(f-n) |
	//   | 0,           0,          -1,          0           |
	//
	// The near plane is located at z = -n and the far plane at z = -f 
	// in the OpenGL camera reference system. The projection matrix for OpenGL is:
	// M_per =
	//   | (2|n|)/(r-l),  0,						(r+l)/(r-l),					0										|
	//   | 0,							(2|n|)/(t-b), (t+b)/(t-b),					0										|
	//   | 0,							0,						(|n|+|f|)/(|n|-|f|),	(2|f||n|)/(|n|-|f|) |
	//   | 0,							0,						-1,										0										|

	n = glm::abs(n);
	f = glm::abs(f);

	f32 t = tan(fovy / 2.0f) * n;
	f32 b = -t;
	f32 r = t * aspect;
	f32 l = -r;
	Mat4f M_per = {
		(2*n)/(r-l), 0,						(r+l)/(r-l), 0,
		0,					 (2*n)/(t-b), (t+b)/(t-b), 0,
		0,					 0,						(n+f)/(n-f), (2*f*n)/(n-f),
		0,					 0,						-1,					 0
	};
	M_per = glm::transpose(M_per); // from row-major to column-major
	return M_per;
}

Mat4f Camera::GetOrthographicProjection(f32 l, f32 r,
																				f32 b, f32 t,
																				f32 n, f32 f) const
{
	// Constructs a matrix that projects the orthographic volume 
	// defined by left, right, bottom, top, near, and far into the OpenGL canonical volume [-1, 1]^3.
	// Unlike perspective projection, objects here do not undergo perspective 
	// (they remain parallel, with no depth effect).
	// M_orth = 
	//   | 2/(r-l), 0,       0,       -(r+l)/(r-l) |
	//   | 0,       2/(t-b), 0,       -(t+b)/(t-b) |
	//   | 0,       0,       2/(f-n), -(f+n)/(f-n) |
	//   | 0,       0,       0,       1            |
	//
	// Note:
	//   OpenGL looks along the −Z axis:
	//     - near -> closer to the camera (e.g., −0.1)
	//     - far  -> farther from the camera (e.g., −100)
	//
	// M_orth = 
	//   | 2/(r-l), 0,       0,        -(r+l)/(r-l) |
	//   | 0,       2/(t-b), 0,        -(t+b)/(t-b) |
	//   | 0,       0,       -2/(f-n), -(f+n)/(f-n) |
	//   | 0,       0,       0,        1            |


	Mat4f M_orth = {
		2.f/(r-l),  0,          0,          -(r+l)/(r-l),
		0,          2.f/(t-b),  0,          -(t+b)/(t-b),
		0,          0,         -2.f/(f-n),  -(f+n)/(f-n),
		0,          0,          0,          1
	};
	M_orth = glm::transpose(M_orth); // row-major to column-major order
	return M_orth;
}

void Camera::ProcessKeyboard(f32 delta, f32 movementSpeed)
{
	WindowManager& windowManager = WindowManager::GetInstance();
	const f32 velocity = movementSpeed * delta;
	
	InputAction press = InputAction::Press;
	InputKey keyW = InputKey::W;
	InputKey keyA = InputKey::A;
	InputKey keyS = InputKey::S;
	InputKey keyD = InputKey::D;
	InputKey keySpace = InputKey::Space;
	InputKey keyLCTRL = InputKey::LeftControl;

	Vec3F forward = GetForwardVector();
	Vec3F right = GetRightVector();
	Vec3F up = GetUpVector();

	// W-S
	if (windowManager.GetKey(keyW) == press) { position += forward * velocity; }
	else if (windowManager.GetKey(keyS) == press) { position -= forward * velocity; }
	// A-D
	if (windowManager.GetKey(keyA) == press) { position -= right * velocity; }
	else if (windowManager.GetKey(keyD) == press) { position += right * velocity; }
	// SPACE-LCTRL
	if (windowManager.GetKey(keySpace) == press) { position += up * velocity; }
	else if (windowManager.GetKey(keyLCTRL) == press) { position -= up * velocity;}
}

void Camera::ProcessMouse(f32 delta, f32 mouseSensitivity)
{
	WindowManager& windowManager = WindowManager::GetInstance();

	InputAction press = InputAction::Press;
	InputMouseKey mouseRight = InputMouseKey::Right;
	if (windowManager.GetMouseKey(mouseRight) == press)
	{
		Vec2D mousePos = windowManager.GetCursorPosition();
		Vec2I windowSize = windowManager.GetWindowSize();
		static f64 lastX = static_cast<f32>(windowSize.x) / 2.0f;
		static f64 lastY = static_cast<f32>(windowSize.y) / 2.0f;
		static bool firstMouse = true;
		if (firstMouse)
		{
			lastX = mousePos.x;
			lastY = mousePos.y;
			firstMouse = false;
		}

		const f32 xoffset = static_cast<f32>(lastX - mousePos.x);
		const f64 yoffset = lastY - mousePos.y;
		eulerAngles.y += glm::radians(xoffset) * delta * mouseSensitivity;
		eulerAngles.x += glm::radians(yoffset) * delta * mouseSensitivity;

		lastX = mousePos.x;
		lastY = mousePos.y;
	}
}
