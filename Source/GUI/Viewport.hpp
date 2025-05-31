#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

void GUI_Viewport(StringView windowName,
                  class Texture2D textureImage,
                  class GameObject& objSelected,
                  i32 gizmode,
                  const Mat4f& view,
                  const Mat4f& proj);
