#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

class Texture2D;
class GameObject;

void GUI_Viewport(StringView windowName,
                  Texture2D textureImage,
                  GameObject objSelected,
                  i32 gizmode,
                  const Mat4F& view,
                  const Mat4F& proj);
