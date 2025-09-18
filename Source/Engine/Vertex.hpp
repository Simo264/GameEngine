#pragma once

#include "Core/Math/Base.hpp"

struct Position { Vec3F data; };
struct Normal { Vec3F data; };
struct TextureCoord { Vec2F data; };
struct Tangent { Vec3F data; };

template<typename... Attributes>
struct Vertex {};

template<>
struct Vertex<Position>
{
  Position position;
};

template<>
struct Vertex<Position, TextureCoord>
{
  Position position;
  TextureCoord tc;
};

template<>
struct Vertex<Position, Normal>
{
  Position position;
  Normal normal;
};

template<>
struct Vertex<Position, Normal, TextureCoord, Tangent>
{
  Position position;
  Normal normal;
  TextureCoord tc;
  Tangent tangent;
};
