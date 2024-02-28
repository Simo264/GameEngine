#include "Engine/Engine.hpp"

int main()
{
  Engine engine;
  engine.Initialize();
  engine.Run();
  engine.CleanUp();
  return 0;
}