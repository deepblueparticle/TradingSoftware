/*
 * =====================================================================================
 *
 *       Filename:  TestMain.C
 *
 *    Description:  Main function for unit test
 *
 *        Created:  10/19/2016 07:59:59 AM
 *       Compiler:  g++
 *
 *         Author:  Tai Hu
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
