/*
 * =====================================================================================
 *
 *       Filename:  TestInfra.H
 *
 *    Description:  
 *
 *        Created:  09/26/2016 10:42:36 PM
 *       Compiler:  g++
 *
 *         Author:  Tai Hu
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef TestInfra_H
#define TestInfra_H

#include <gtest/gtest.h>
#include <shared/commands.h>
#include <strategy/AlgoMode1Strategy.H>

class TestInfra : public ::testing::Test
{
  protected:
    void SetUp();
    void TearDown();
    TestInfra();


    Algo::AlgoModel1 am;
    int fd;
    std::shared_ptr<Algo::AlgoMode1Strategy> algoMode1Strategy;

};

#endif
