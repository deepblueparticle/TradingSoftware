/*
 * =====================================================================================
 *
 *       Filename:  TestAlgomodelStrategy2.H
 *
 *    Description:  
 *
 *        Created:  10/01/2016 07:55:24 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef TestAlgomodelStrategy2_H
#define TestAlgomodelStrategy2_H

#include <gtest/gtest.h>
#include <shared/commands.h>
#include <strategy/AlgoMode1Strategy.H>

class TestAlgomodelStrategy2 : public ::testing::Test
{
  protected:
    void SetUp();
    void TearDown();
    TestAlgomodelStrategy2();
    void Initialize();


    Algo::AlgoModel1 am;
    int fd;
    Algo::AlgoMode1Strategy *algoMode1Strategy;
};

#endif
