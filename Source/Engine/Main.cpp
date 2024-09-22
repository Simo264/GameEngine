#include "Engine/Engine.hpp"
#include "Core/Log/Logger.hpp"

int main()
{
  Engine engine = Engine();
  engine.Initialize();
  try
  {
    engine.Run();
  }
  catch (const std::exception& err)
  {
    CONSOLE_CRITICAL(err.what());
    return EXIT_FAILURE;
  }
  engine.CleanUp();
  
  return EXIT_SUCCESS;
}