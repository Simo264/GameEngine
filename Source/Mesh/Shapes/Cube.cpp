#include "Cube.hh"

#include "../../ObjectLoader.hh"

Cube::Cube() : StaticMesh()
{
  ObjectLoader loader("Shapes/Cube/Cube.obj");
  loader.LoadStaticMesh(this);
}
