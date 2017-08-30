/*
 * =====================================================================================
 *
 *       Filename:  TestInfra.C
 *
 *    Description:  Test cases for Infras
 *
 *        Created:  09/26/2016 10:48:10 PM
 *       Compiler:  g++
 *
 *         Author:  Tai Hu
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <TestInfra.H>
#include <quote/QuoteBook.H>
#include <config/Configuration.H>
#include <fix/FixOPHandler.H>
#include <storage/PersistOrder.H>
#include <storage/StrategyStore.H>
#include <fstream>
#include <regex>

void TestInfra::SetUp()
{
}

void TestInfra::TearDown()
{
}

TestInfra::TestInfra()
{
}

TEST_F(TestInfra, TExitIntervalisLoaded)
{
  std::ifstream src("./config/AlgoEngine.conf");
	//take snapshot of "./config/AlgoEngine.conf"
  std::string confContent((std::istreambuf_iterator<char>(src)),
                   std::istreambuf_iterator<char>());
  src.close();
  std::string confContentReplaced = confContent;
  std::regex TExitIntervalRegex("TExitInterval=[0-9.,]*");
  //set TExitInterval to 4123 ms in the tmp string
  confContentReplaced = std::regex_replace(confContent, TExitIntervalRegex, "TExitInterval=4123");
	//write the replaced version to "./config/AlgoEngine.conf", as it's hard coded to be read
	//TODO: make the config files to read configurable
  std::ofstream out("./config/AlgoEngine.conf", std::ofstream::trunc);
  out << confContentReplaced;
  out.close();

  Algo::Configuration::getInstance()->loadConfig();
  //Algo::Configuration::getInstance()->getTExitInterval() is used to calculat whether to modifySecondaryOrder(). 
  //As long as it's correct we should have the right behavior
  EXPECT_EQ(4123, Algo::Configuration::getInstance()->getTExitInterval());


	//revert back the content in "./config/AlgoEngine.conf"
  std::ofstream outRevert("./config/AlgoEngine.conf", std::ofstream::trunc);
  outRevert << confContent;
  outRevert.close();
}

