#include "Camera.hpp"

#include "Core/Math/Transform.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Managers/WindowManager.hpp"

namespace Components
{
	Mat4F Camera::CalculateViewMatrix() const
	{
		// Construct the camera matrix using the camera's right (u), up (v), and forward (w) vectors.
		// The matrix represents the camera's position and orientation in world space.
		// We take its inverse to transform world coordinates into the camera's local space.
		// M_cam = 
		//  | ux vx wx ex |				| ux uy uz 0 |	 | 1 0 0 -ex |
		//  | uy vy wy ey |				| vx vy vz 0 |	 | 0 1 0 -ey |
		//  | uz vz wz ez |^-1 =	| wx wy wz 0 | * | 0 0 1 -ez |
		//  | 0  0  0   1 |				| 0  0  0  1 |	 | 0 0 0   1 |
		auto forward = CalculateForwardVector();
		auto right = CalculateRightVector();
		auto up = CalculateUpVector();

		auto w = glm::normalize(-forward);						// z
		auto u = glm::normalize(glm::cross(up, w));	// x
		auto v = glm::normalize(glm::cross(w, u));   // y
		auto M_cam = glm::inverse(Mat4F{
			u.x, v.x, w.x, position.x,
			u.y, v.y, w.y, position.y,
			u.z, v.z, w.z, position.z,
			0, 0, 0, 1
															});
		M_cam = glm::transpose(M_cam); // from row-major to column-major order
		return M_cam;
	}
	Mat4F Camera::CalculatePerspectiveMatrix(f32 aspect) const
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

		auto fovY = 2.0f * glm::atan(glm::tan(glm::radians(fovH) * 0.5f) / aspect);
		auto t = tan(fovY / 2.0f) * nearClip;
		auto b = -t;
		auto r = t * aspect;
		auto l = -r;
		auto M_per = Mat4F{
			(2 * nearClip) / (r - l),  0, (r + l) / (r - l), 0,
			0, (2 * nearClip) / (t - b), (t + b) / (t - b),	0,
			0, 0, (nearClip + farClip) / (nearClip - farClip),	(2 * farClip * nearClip) / (nearClip - farClip),
			0, 0,	-1, 0
		};
		M_per = glm::transpose(M_per); // from row-major to column-major
		return M_per;
	}
	Mat4F Camera::CalculateOrthographicMatrix(f32 l, f32 r, f32 b, f32 t) const
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

		auto n = nearClip;
		auto f = farClip;
		auto M_orth = Mat4F{
			2.f / (r - l), 0, 0, -(r + l) / (r - l),
			0, 2.f / (t - b), 0, -(t + b) / (t - b),
			0, 0, -2.f / (f - n), -(f + n) / (f - n),
			0, 0,	0, 1
		};
		M_orth = glm::transpose(M_orth); // row-major to column-major order
		return M_orth;
	}

	Vec3F Camera::CalculateForwardVector() const
	{
		return Quat(glm::radians(eulerAngles)) * Vec3F(0.f, 0.f, -1.f);
	}
	Vec3F Camera::CalculateRightVector() const
	{
		return Quat(glm::radians(eulerAngles)) * Vec3F(1.f, 0.f, 0.f);
	}
	Vec3F Camera::CalculateUpVector() const
	{
		return Quat(glm::radians(eulerAngles)) * Vec3F(0.f, 1.f, 0.f);
	}

	void Camera::ProcessKeyboard(f32 delta, f32 movementSpeed)
	{
		auto& windowManager = WindowManager::GetInstance();
		auto velocity = movementSpeed * delta;

		auto press = InputAction::Press;
		auto keyW = InputKey::W;
		auto keyA = InputKey::A;
		auto keyS = InputKey::S;
		auto keyD = InputKey::D;
		auto keySpace = InputKey::Space;
		auto keyLCTRL = InputKey::LeftControl;

		auto forward = CalculateForwardVector();
		auto right = CalculateRightVector();
		auto up = CalculateUpVector();

		// W-S
		if (windowManager.GetKey(keyW) == press)
		{
			position += forward * velocity;
		}
		else if (windowManager.GetKey(keyS) == press)
		{
			position -= forward * velocity;
		}
		// A-D
		if (windowManager.GetKey(keyA) == press)
		{
			position -= right * velocity;
		}
		else if (windowManager.GetKey(keyD) == press)
		{
			position += right * velocity;
		}
		// SPACE-LCTRL
		if (windowManager.GetKey(keySpace) == press)
		{
			position += up * velocity;
		}
		else if (windowManager.GetKey(keyLCTRL) == press)
		{
			position -= up * velocity;
		}
	}
	void Camera::ProcessMouseMovement(f32 mouseSensitivity)
	{
		auto& windowManager = WindowManager::GetInstance();
		auto press = InputAction::Press;
		auto rightMouseButton = InputMouseKey::Right;

		// Variabili static condivise
		static bool firstMouse = true;
		static auto lastX = 0.0f;
		static auto lastY = 0.0f;

		// Solo se il tasto destro del mouse è premuto
		if (windowManager.GetMouseKey(rightMouseButton) == press)
		{
			// Ottieni la posizione attuale del mouse
			auto cursorPos = windowManager.GetCursorPosition();

			if (firstMouse)
			{
				lastX = cursorPos.x;
				lastY = cursorPos.y;
				firstMouse = false;
				return; // Esci senza calcolare offset la prima volta
			}

			// Calcola l'offset del movimento del mouse
			auto xOffset = cursorPos.x - lastX;
			auto yOffset = cursorPos.y - lastY;

			lastX = cursorPos.x;
			lastY = cursorPos.y;

			// Applica la sensibilità con segno invertito per correggere la direzione
			xOffset *= -mouseSensitivity;
			yOffset *= -mouseSensitivity;

			// Aggiorna gli angoli di Eulero
			eulerAngles.y += xOffset; // Yaw (rotazione orizzontale)
			eulerAngles.x += yOffset; // Pitch (rotazione verticale)

			// Opzionale: limita il pitch per evitare il gimbal lock
			if (eulerAngles.x > 89.0f) eulerAngles.x = 89.0f;
			if (eulerAngles.x < -89.0f) eulerAngles.x = -89.0f;
		}
		else
		{
			// Reset del flag quando rilasciamo il tasto destro
			firstMouse = true;
			// Aggiorna anche la posizione del mouse per la prossima volta
			auto cursorPos = windowManager.GetCursorPosition();
			lastX = cursorPos.x;
			lastY = cursorPos.y;
		}
	}
}
