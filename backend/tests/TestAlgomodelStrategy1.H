/*
 * =====================================================================================
 *
 *       Filename:  TestAlgomodelStrategy1.H
 *
 *    Description:  
 *
 *        Created:  09/26/2016 10:42:36 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef TestAlgomodelStrategy1_H
#define TestAlgomodelStrategy1_H

#include <gtest/gtest.h>
#include <shared/commands.h>
#include <strategy/AlgoMode1Strategy.H>

class TestAlgomodelStrategy1 : public ::testing::Test
{
  protected:
    void SetUp();
    void TearDown();
    TestAlgomodelStrategy1();
    void Initialize();


    Algo::AlgoModel1 am;
    int fd;
    Algo::AlgoMode1Strategy *algoMode1Strategy;

};

#endif
