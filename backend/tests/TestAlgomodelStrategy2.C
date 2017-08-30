/*
 * =====================================================================================
 *
 *       Filename:  TestAlgomodelStrategy2.C
 *
 *    Description:  Test cases for Strategy Type 2 (Algo_2)
 *
 *        Created:  10/01/2016 07:56:42 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <TestAlgomodelStrategy2.H>
#include <quote/QuoteBook.H>
#include <config/Configuration.H>
#include <fix/FixOPHandler.H>
#include <storage/PersistOrder.H>

void TestAlgomodelStrategy2::SetUp()
{
  Algo::Configuration::getInstance()->loadConfig();
  algoMode1Strategy = nullptr;

  Algo::FixOPHandler& opHandler = Algo::FixOPHandler::getInstance();
  opHandler.setBuyAccount(Algo::Configuration::getInstance()->getAccountNumberBuy().c_str());
  opHandler.setSellAccount(Algo::Configuration::getInstance()->getAccountNumberSell().c_str());
  opHandler.start(Algo::Configuration::getInstance()->getConfigFileOP(),
      Algo::Configuration::getInstance()->getFXCMOPLogLevel());
}

void TestAlgomodelStrategy2::TearDown()
{
  if(algoMode1Strategy)
    delete algoMode1Strategy;
}

TestAlgomodelStrategy2::TestAlgomodelStrategy2()
{
}

void TestAlgomodelStrategy2::Initialize()
{
  am.SetInternalId(1);
  am.SetStrategyId(1);
  am.SetDataSymbol("EUR/USD");
  am.SetTickSize(0.00001);
  am.SetMode(Algo::OrderMode_SELL);
  am.SetPrevClose(1.1339);
  am.SetBaseValue(0.0084919);
  am.SetEntryPrice(7);
  am.SetEntryPriceCalc(1.13535);
  am.SetPointA(24);
  am.SetPointACalc(1.13594);
  am.SetPointB(5.5);
  am.SetPointBCalc(1.13547);
  am.SetCancelPoint1(40);
  am.SetCancelPoint1Calc(1.1373);
  am.SetCancelPoint2(16);
  am.SetCancelPoint2Calc(1.13526);
  am.SetProfit(84);
  am.SetProfitCalc(1.112677);
  am.SetLoss(16);
  am.SetLossCalc(1.13526);
  am.SetTolerance(100);
  am.SetQty(5000);
  am.SetTrailing(64);
  am.SetCancelTicks(100);
  am.SetAlgoType(Algo::Algo_2);
  am.SetTimeEnabled(0);
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_PointA_Reached_Before_Start_Expact_Stopped)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.15, 1.14, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();
  EXPECT_EQ(Algo::StrategyStatus_STOPPED, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_PointA_DidNotReache_After_Start_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.09294, 1.09293, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_BEGIN, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_PointA_Reached_After_Start_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes ("EUR/USD", 1.15, 1.14, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_POINT_A, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_CP1_Reached_Before_Start_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.98, 1.97, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  am.SetPointACalc(2.0); // Setting unexpected value to point A
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_CP1_Reached_Afetr_PointA_Expact_Stopped)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.14, 1.139296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.14, 1.38, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_STOPPED, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_STOPPED, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_PointB_Reached_Afetr_PointA_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.14, 1.139296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.091, 1.09, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_POINT_B, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_CP2_Reached_Afetr_PointA_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  //am.SetCancelPoint1Calc(1.1373);
  //am.SetCancelPoint2Calc(1.13526);
  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  // Making Point A to true
  quote = Algo::Quotes("EUR/USD", 1.14, 1.139296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.1361, 1.136, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_POINT_A, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_CP2_Reached_Afetr_PointA_And_PointB_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.13556, 1.13554, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  //am.SetCancelPoint1Calc(1.1373);
  //am.SetCancelPoint2Calc(1.13526);
  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  // Making Point A to true
  quote = Algo::Quotes("EUR/USD", 1.14, 1.139296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  // Making Point B to true
  quote = Algo::Quotes("EUR/USD", 1.11, 1.10, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  // CP2 true
  quote = Algo::Quotes("EUR/USD", 1.1361, 1.136, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_POINT_B, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_CP2_Reached_Afetr_PointB_Then_PointA_Expact_Stopped)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  //am.SetCancelPoint1Calc(1.1373);
  //am.SetCancelPoint2Calc(1.13526);
  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  // Making Point A to true
  quote = Algo::Quotes("EUR/USD", 1.14, 1.139296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  // Making Point B to true
  quote = Algo::Quotes("EUR/USD", 1.11, 1.10, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  // CP2 true
  quote = Algo::Quotes("EUR/USD", 1.1361, 1.136, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_STOPPED, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_STOPPED, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy2, Run_Strategy_For_SELL_With_Trigger_Reached_Afetr_PointB_Expact_Running)
{
  if(Algo::PersistOrder::getInstance().initDB(Algo::Configuration::getInstance()->getDBName()))
    Algo::PersistOrder::getInstance().loadOrders();

  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.13556, 1.13554, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  //Point A reached
  quote = Algo::Quotes("EUR/USD", 1.14, 1.139296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  //Point B reached
  quote = Algo::Quotes("EUR/USD", 1.11, 1.10, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  //Price below Trigger Price
  quote = Algo::Quotes("EUR/USD", 1.13530, 1.13525, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_STOPPED, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_STOPPED, algoMode1Strategy->getStatus());
  Algo::PersistOrder::getInstance().close();
}

