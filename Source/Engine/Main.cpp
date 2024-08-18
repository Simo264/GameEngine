#include "Engine/Engine.hpp"

int main()
{
  Engine engine = Engine();
  engine.Initialize();
  engine.Run();
  engine.CleanUp();
  return 0;
}