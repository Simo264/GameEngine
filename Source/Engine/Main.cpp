#include "Engine/Engine.hpp"
#include "Utils/Logger.hpp"

int main()
{
  auto engine = Engine{};
  try
  {
    engine.Initialize();
    engine.Run();
    engine.Cleanup();
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