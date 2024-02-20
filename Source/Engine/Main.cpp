#include "Engine/Engine.hpp"

int main()
{
  Engine engine;
  engine.Initialize();
  engine.Run();
  engine.ShutDown();
  return 0;
}