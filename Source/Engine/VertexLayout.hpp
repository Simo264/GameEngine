#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Engine/Graphics/VertexArray.hpp"

struct Position
{
  Vec3F data;
  static constexpr auto GetVertexFormat(size_t offset)
  {
    return VertexFormat{ 3, VertexAttribType::FLOAT, false, static_cast<i32>(offset) };
  }
  static constexpr auto GetSize() { return sizeof(Vec3F); }
};
struct Normal
{
  Vec3F data;
  static constexpr auto GetVertexFormat(size_t offset)
  {
    return VertexFormat{ 3, VertexAttribType::FLOAT, false, static_cast<i32>(offset) };
  }
  static constexpr auto GetSize() { return sizeof(Vec3F); }
};
struct TextureCoord
{
  Vec2F data;
  static constexpr auto GetVertexFormat(size_t offset)
  {
    return VertexFormat{ 2, VertexAttribType::FLOAT, false, static_cast<i32>(offset) };
  }
  static constexpr auto GetSize() { return sizeof(Vec2F); }
};
struct Tangent
{
  Vec3F data;
  static constexpr auto GetVertexFormat(size_t offset)
  {
    return VertexFormat{ 3, VertexAttribType::FLOAT, false, static_cast<i32>(offset) };
  }
  static constexpr auto GetSize() { return sizeof(Vec3F); }
};


// Template helper per calcolare gli offset
template<typename... Types>
struct OffsetCalculator;

template<typename First>
struct OffsetCalculator<First>
{
  static constexpr auto GetOffset(size_t index)
  {
    return (index == 0) ? 0 : First::GetSize();
  }
};

template<typename First, typename... Rest>
struct OffsetCalculator<First, Rest...>
{
  static constexpr auto GetOffset(size_t index)
  {
    return (index == 0) ? 0 : First::GetSize() + OffsetCalculator<Rest...>::GetOffset(index - 1);
  }
};


template<typename... Attributes>
struct VertexLayout
{
  static void SetupVertexArray(VertexArray& vertexArray)
  {
    __SetupAttributesImpl<0, Attributes...>(vertexArray);
  }

private:
  template<size_t Index, typename First, typename... Rest>
  static void __SetupAttributesImpl(VertexArray& vertexArray)
  {
    constexpr auto offset = OffsetCalculator<Attributes...>::GetOffset(Index);
    constexpr auto format = First::GetVertexFormat(offset);
    vertexArray.SetupVertexAttribute(Index, 0, format);

    if constexpr (sizeof...(Rest) > 0)
    {
      __SetupAttributesImpl<Index + 1, Rest...>(vertexArray);
    }
  }
};

template<>
struct VertexLayout<Position>
{
  Position position;

  static void SetupVertexArray(VertexArray& vertexArray)
  {
    constexpr auto format0 = Position::GetVertexFormat(0);
    vertexArray.SetupVertexAttribute(0, 0, format0);
  }
};

template<>
struct VertexLayout<Position, TextureCoord>
{
  Position position;
  TextureCoord tc;

  static void SetupVertexArray(VertexArray& vertexArray)
  {
    constexpr auto format0 = Position::GetVertexFormat(0);
    constexpr auto format1 = TextureCoord::GetVertexFormat(sizeof(Position));
    vertexArray.SetupVertexAttribute(0, 0, format0);
    vertexArray.SetupVertexAttribute(1, 0, format1);
  }
};

template<>
struct VertexLayout<Position, Normal>
{
  Position position;
  Normal normal;

  static void SetupVertexArray(VertexArray& vertexArray)
  {
    constexpr auto format0 = Position::GetVertexFormat(0);
    constexpr auto format1 = Normal::GetVertexFormat(sizeof(Position));
    vertexArray.SetupVertexAttribute(0, 0, format0);
    vertexArray.SetupVertexAttribute(1, 0, format1);
  }

};

template<>
struct VertexLayout<Position, Normal, TextureCoord, Tangent>
{
  Position position;
  Normal normal;
  TextureCoord tc;
  Tangent tangent;

  static void SetupVertexArray(VertexArray& vertexArray)
  {
    constexpr auto format0 = Position::GetVertexFormat(0);
    constexpr auto format1 = Normal::GetVertexFormat(sizeof(Position));
    constexpr auto format2 = TextureCoord::GetVertexFormat(sizeof(Position) + sizeof(Normal));
    constexpr auto format3 = Tangent::GetVertexFormat(sizeof(Position) + sizeof(Normal) + sizeof(TextureCoord));
    vertexArray.SetupVertexAttribute(0, 0, format0);
    vertexArray.SetupVertexAttribute(1, 0, format1);
    vertexArray.SetupVertexAttribute(2, 0, format2);
    vertexArray.SetupVertexAttribute(3, 0, format3);
  }
};


// Exaple usage:
// auto& mesh = object.AddComponent<StaticMesh>();
// mesh.Create();
// 
// using Vertex = VertexLayout<Position, Normal, TextureCoord, Tangent>;
// Vertex::SetupVertexArray(mesh.vertexArray);
// 
// auto loader = StaticMeshLoader{};
// loader.LoadDataFromFile(absolute, mesh);
