#include "PathManagerInterface.h"
#include "pathManager.hpp"

static pathManager pathMng;

void PathManagerInterfaceExecute(void) {
    pathMng.execute();
}