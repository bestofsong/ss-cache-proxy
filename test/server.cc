#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <cacheproxy/CacheProxy.h>
#include <iostream>

//------------------------------------------------------------------------------

using namespace smartstudy;

int main(int argc, char* argv[])
{

  CacheProxy cp;
  auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
  cp.start(argv[1], port, argv[3]);
  return 0;
}
