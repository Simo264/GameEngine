#include "Cylinder.hh"

#include "../../ObjectLoader.hh"

Cylinder::Cylinder() : StaticMesh()
{
  ObjectLoader loader("Shapes/Cylinder/Cylinder.obj");
  loader.LoadStaticMesh(this);
}
