#include "Engine/Engine.hpp"

int main()
{
  Engine* engine = new Engine;
  engine->Initialize();
  engine->Run();
  engine->CleanUp();
  delete engine;
  return 0;
}