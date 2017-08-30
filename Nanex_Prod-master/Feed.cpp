#include <iostream>
#include <iomanip>
#include "NxCoreAPI_Wrapper_C++.h"
#include <NxCoreAPI_Wrapper_C.h>

int onNxCoreCallback(const NxCoreSystem* pNxCoreSys,const NxCoreMessage* pNxCoreMsg);
char* exchangeNameFromExg(char* buf, int exg);
void onGetExgQuoteStates(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg);
void GetDefinedString();

NxCoreClass NxCore;
int main(int argc, char* argv[])
{
  std::cout << "AlgoEngine main" << std::endl;

  if(NxCore.LoadNxCore("../../../NxCoreAPI/libnx.so"))
  {
    int returnValue = NxCore.ProcessTape("",NULL,(NxCF_EXCLUDE_QUOTES2 | NxCF_EXCLUDE_OPRA),0,onNxCoreCallback);
    NxCore.ProcessReturnValue(returnValue);
    NxCore.UnloadNxCore();
  }
  return 0;
}

int onNxCoreCallback(const NxCoreSystem* pNxCoreSys,const NxCoreMessage* pNxCoreMsg)
{
  switch (pNxCoreMsg->MessageType)
  {
     case NxMSG_STATUS:
     {
      std::cout << "Status " << pNxCoreSys->Status << std::endl;
		break;
      }
    case NxMSG_EXGQUOTE:
      onGetExgQuoteStates(pNxCoreSys, pNxCoreMsg);
      break;
    case NxMSG_SYMBOLSPIN:
      if (pNxCoreMsg->coreHeader.pnxStringSymbol->String[0]=='f')
      {
	std::cout << pNxCoreMsg->coreHeader.pnxStringSymbol->String;
      }
      break;
  }
  return NxCALLBACKRETURN_CONTINUE;
}

void onGetExgQuoteStates(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg)
{
  char buf[100] = "hello";
  if (pNxCoreMsg->coreHeader.pnxStringSymbol->String[0] == 'f')
  {
    NxCoreExgQuote* Quote = (NxCoreExgQuote*) &pNxCoreMsg->coreData.ExgQuote;
    int refresh = (int)Quote->coreQuote.Refresh;
    double Bid = NxCore.PriceToDouble(Quote->coreQuote.BidPrice, Quote->coreQuote.PriceType);
    double Ask = NxCore.PriceToDouble(Quote->coreQuote.AskPrice, Quote->coreQuote.PriceType);
    std::cout << "ExgQuote for Symbol: " << pNxCoreMsg->coreHeader.pnxStringSymbol->String + 1 << "," <<
                 " Time: " << (int) pNxCoreMsg->coreHeader.nxExgTimestamp.Hour << ":"
                           << (int) pNxCoreMsg->coreHeader.nxExgTimestamp.Minute << ":"
                           << (int) pNxCoreMsg->coreHeader.nxExgTimestamp.Second <<
                 " Bid: "  << Bid <<
                 " Ask: "  << Ask <<
                 " Exchg: "<< pNxCoreMsg->coreHeader.ReportingExg << 
                 " Refresh "<< refresh << std::endl;
  }
}

