#pragma once

#include "Core/Strings.hpp"

namespace Components
{
  template<typename T>
  constexpr StringView GetTypeName() { return "UnknownComponent"; }

  #define DECLARE_COMPONENT(ClassName) \
      struct ClassName; \
      template<> constexpr StringView GetTypeName<Components::ClassName>() { return #ClassName; } \
      struct ClassName
}
