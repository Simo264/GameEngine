#include "engine/engine.hpp"
#include "utils/logger.hpp"

#include <exception>

int main()
{
  auto engine = Engine{};
  try
  {
    engine.run();
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