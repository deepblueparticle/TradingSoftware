/*
 * =====================================================================================
 *
 *       Filename:  TestAlgomodelStrategy1.C
 *
 *    Description:  Test cases for Strategy Type 1 (Algo_1)
 *
 *        Created:  09/26/2016 10:48:10 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <TestAlgomodelStrategy1.H>
#include <quote/QuoteBook.H>
#include <config/Configuration.H>
#include <fix/FixOPHandler.H>
#include <storage/PersistOrder.H>

void TestAlgomodelStrategy1::SetUp()
{
  Algo::Configuration::getInstance()->loadConfig();
  algoMode1Strategy = nullptr;
}

void TestAlgomodelStrategy1::TearDown()
{
  if(algoMode1Strategy)
    delete algoMode1Strategy;
}

TestAlgomodelStrategy1::TestAlgomodelStrategy1()
{
}

void TestAlgomodelStrategy1::Initialize()
{
  am.SetInternalId(1);
  am.SetStrategyId(1);
  am.SetDataSymbol("EUR/USD");
  am.SetTickSize(0.00001);
  am.SetMode(Algo::OrderMode_BUY);
  am.SetPrevClose(1.8685);
  am.SetBaseValue(0.0254182);
  am.SetEntryPrice(7);
  am.SetEntryPriceCalc(1.09473);
  am.SetPointA(24);
  am.SetPointACalc(1.09295);
  am.SetPointB(5.5);
  am.SetPointBCalc(1.09155);
  am.SetCancelPoint1(40);
  am.SetCancelPoint1Calc(1.09702);
  am.SetCancelPoint2(16);
  am.SetCancelPoint2Calc(1.08278);
  am.SetProfit(84);
  am.SetProfitCalc(1.1082);
  am.SetLoss(16);
  am.SetLossCalc(1.08278);
  am.SetTolerance(100);
  am.SetQty(5000);
  am.SetTrailing(64);
  am.SetCancelTicks(100);
  am.SetAlgoType(Algo::Algo_1);
  am.SetTimeEnabled(0);
}

TEST_F(TestAlgomodelStrategy1, MethodIsChanged)
{
  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  double x = 4.0;
  double y = 5.0;
  EXPECT_EQ(true, algoMode1Strategy->isNotEqual(x, y));
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_With_StartTime_Disabled_Expact_Running)
{
  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_With_StartTime_Enabled_And_StartTime_InPast_Expact_Running)
{
  Initialize();
  am.SetTimeEnabled(1);
  am.SetStartTimeEpoch(std::time(0)-10000);
  am.SetStartDateEpoch(0);
  am.SetEndTimeEpoch(std::time(0)+10000);
  am.SetEndDateEpoch(0);

  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->processFeed();
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_With_StartTime_Enabled_AndStartTime_InFuture_Expact_Waiting)
{
  Initialize();
  am.SetTimeEnabled(1);
  time_t now;
  am.SetStartTimeEpoch(std::time(&now)+10000);
  am.SetStartDateEpoch(std::time(&now)+10000);
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  EXPECT_EQ(Algo::StrategyStatus_WAITING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_For_BUY_With_PointA_Reached_Before_Start_Expact_Stopped)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09298, 1.09296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();
  EXPECT_EQ(Algo::StrategyStatus_STOPPED, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_For_BUY_With_PointA_DidNotReache_After_Start_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.09294, 1.09293, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_BEGIN, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_For_BUY_With_PointA_Reached_After_Start_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.09297, 1.09296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_POINT_A, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_For_BUY_With_CP1_Reached_Before_Start_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.098, 1.098, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  am.SetPointACalc(2.0); // Setting unexpected value to point A
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_For_BUY_With_CP1_Reached_Afetr_PointA_Expact_Stopped)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.09297, 1.09296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.098, 1.096, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_STOPPED, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_STOPPED, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_For_BUY_With_PointB_Reached_Afetr_PointA_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.09297, 1.09296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.091, 1.09, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_POINT_B, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_For_BUY_With_CP2_Reached_Afetr_PointA_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.09297, 1.09296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  quote = Algo::Quotes("EUR/USD", 1.071, 1.07, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_POINT_B, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_RUNNING, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_For_BUY_With_CP2_Reached_Afetr_PointB_Expact_Running)
{
  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  //Point A reached
  quote = Algo::Quotes("EUR/USD", 1.09297, 1.09296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  //Point B reached
  quote = Algo::Quotes("EUR/USD", 1.091, 1.09, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  //Cancel Point B reached
  quote = Algo::Quotes("EUR/USD", 1.071, 1.07, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_STOPPED, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_STOPPED, algoMode1Strategy->getStatus());
}

TEST_F(TestAlgomodelStrategy1, Run_Strategy_For_BUY_With_Trigger_Reached_Afetr_PointB_Expact_Running)
{
  if(Algo::PersistOrder::getInstance().initDB(Algo::Configuration::getInstance()->getDBName()))
    Algo::PersistOrder::getInstance().loadOrders();

  //Add a dummy quote
  Algo::Quotes quote("EUR/USD", 1.09296, 1.09294, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);

  Initialize();
  algoMode1Strategy = new Algo::AlgoMode1Strategy(am);
  algoMode1Strategy->setQuotesProvider(Algo::QuoteBook::getInstance());
  algoMode1Strategy->processFeed();

  //Point A reached
  quote = Algo::Quotes("EUR/USD", 1.09297, 1.09296, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  //Point B reached
  quote = Algo::Quotes("EUR/USD", 1.091, 1.09, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  //Price Above Trigger Price
  quote = Algo::Quotes("EUR/USD", 1.095, 1.0949, 0, 0, "");
  Algo::QuoteBook::getInstance()->add(quote.getSecurityId(), quote);
  algoMode1Strategy->processFeed();

  EXPECT_EQ(Algo::AlgoMode1State_STOPPED, algoMode1Strategy->getLastState());
  EXPECT_EQ(Algo::StrategyStatus_STOPPED, algoMode1Strategy->getStatus());

  Algo::PersistOrder::getInstance().close();
}
