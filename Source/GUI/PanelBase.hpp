#pragma once

class Scene;

class PanelBase
{
public:
  PanelBase() : _scene{ nullptr } {}

  void SetScene(Scene* scene) { _scene = scene; }

protected:
  Scene* _scene;
};