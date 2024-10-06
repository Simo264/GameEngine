#include "Engine/Engine.hpp"
#include "Core/Log/Logger.hpp"

int main()
{
  Engine engine;
  try
  {
    engine.Initialize();
    engine.Run();
    engine.CleanUp();
  }
  catch(std::runtime_error& e)
  {
    CONSOLE_CRITICAL("runtime_error exception: {}", e.what());
    return 1;
  }
  catch (std::out_of_range& e)
  {
    CONSOLE_CRITICAL("out_of_range exception: {}", e.what());
    return 1;
  }
  
  return 0;
}