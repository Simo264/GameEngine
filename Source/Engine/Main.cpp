#include "Engine/Engine.hpp"
#include "Core/Log/Logger.hpp"

int main()
{
  Engine engine;
  engine.Initialize();
  engine.Run();
  engine.CleanUp();
  return 0;
}