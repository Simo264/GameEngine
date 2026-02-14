#pragma once

namespace Components
{
  struct Material;
}
struct Program;
class Scene;

class RenderSystem
{
public:
  RenderSystem() : _program{ nullptr } {}

  void SetProgram(Program& program){ _program = &program; }
  void Render(Scene& scene);

private:
  void __BindMaterial(const Components::Material& material);
  void __UnbindMaterial();

  Program* _program;
};