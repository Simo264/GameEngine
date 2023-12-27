#include "Actor.hh"

Actor::Actor()
{
  position  = vec3f(0.0f, 0.0f, 0.0f);
  scaling   = vec3f(1.0f, 1.0f, 1.0f);
  rotation  = vec3f(1.0f, 1.0f, 1.0f);
  rotationAngle = 0.0f;
  
  actorName.reserve(50); // Pre-allocation memory 
  actorName.append("Actor_");
  actorName.append(std::to_string(++numActors).c_str());
}

mat4f Actor::GetModel()
{
  mat4f model = mat4f(1.0f);
  model = glm::rotate(model, glm::radians(rotationAngle), rotation);
  model = glm::translate(model, position);
  model = glm::scale(model, scaling);
  return model;
}