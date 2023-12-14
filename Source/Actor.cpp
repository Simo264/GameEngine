#include "Actor.hh"

Actor::Actor()
{
  position  = vec3f(0.0f, 0.0f, 0.0f);
  scaling   = vec3f(1.0f, 1.0f, 1.0f);
  rotation  = vec3f(1.0f, 1.0f, 1.0f);
  rotationAngle = 0.0f;
}

mat4f Actor::Model()
{
  mat4f model = mat4f(1.0f);
  model = glm::scale(model, scaling);
  model = glm::rotate(model, glm::radians(rotationAngle), rotation);
  model = glm::translate(model, position);
  return model;
}