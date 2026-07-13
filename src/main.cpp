#include "Core.h"

int main(int argc, char *argv[]) {
  Core core;
  core.initSDL();
  core.initVulkan();
  core.runLoop();
  core.cleanup();
  return 0;
}
