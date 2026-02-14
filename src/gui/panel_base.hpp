#pragma once

class Scene;

class PanelBase
{
public:
  PanelBase() : scene_{ nullptr } {}

  void set_scene(Scene* scene) { scene_ = scene; }

protected:
  Scene* scene_;
};