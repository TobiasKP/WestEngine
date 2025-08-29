#include <cstdint>

#include "Constants/CoreConstants.h"
#include "CoreHeaders/EngineManager.h"
#include <WestLogger.h>

int main() {
  WestLogger &westLogger = WestLogger::getLoggerInstance();
  EngineManager *engineManager = new EngineManager(&westLogger);
  std::int32_t success = 0;

  success = engineManager->startup();
  if (success != 0)
    return 1;
  westLogger.writeInfo("Successfull startup\n");

  success = engineManager->init();
  if (success != 0)
    return 1;
  westLogger.writeInfo("Successfull initialization\n");

  while (!engineManager->shouldExit()) {
    engineManager->update();
  }

  engineManager->shutdown();
  westLogger.closeFileStreams();

  return 0;
}
