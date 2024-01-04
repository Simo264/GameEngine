#include "Plane.hh"

#include "../../ObjectLoader.hh"

Plane::Plane() : StaticMesh()
{
  ObjectLoader loader("Shapes/Plane/Plane.obj");
  loader.LoadStaticMesh(this);
}