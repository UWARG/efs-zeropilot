// main.cpp -- take 1
#include "UnitTest++/UnitTest++.h"

TEST(Sanity)
{
      CHECK_EQUAL(1, 2);
}


int main(int, const char *[])
{
  return UnitTest::RunAllTests();
}