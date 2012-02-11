#ifdef TIXML_USE_STL
  #include <iostream>
  #include <sstream>
  using namespace std;
#else
  #include <stdio.h>
#endif

#if defined( WIN32 ) && defined( TUNE )
  #include <crtdbg.h>
  _CrtMemState startMemState;
  _CrtMemState endMemState;
#endif

#include "tinyxml.h"

int main()
{
  return 0;
}
