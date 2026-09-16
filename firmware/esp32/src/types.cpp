#include "types.h"
#include <string.h>

bool isManualMode(const SystemSettings& settings) {
  return strcmp(settings.mode, "manual") == 0;
}
