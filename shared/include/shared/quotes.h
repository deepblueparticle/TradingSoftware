/*
 * File:   quotes.h
 * Author: Abhishek Anand
 *
 * Created on May 8, 2015, 11:39 PM
 */

#ifndef QUOTES_H
#define	QUOTES_H

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <cmath>
#include <iomanip>
#include <shared/easylogging++.h>

namespace Algo {

    class Quotes
    {
    public:
        Quotes()
            :_securityId(""),
             _askPrice(0),
             _bidPrice(0),
             _askQty(0),
             _bidQty(0),
             _currency(""),
             _msTimeStamp(0)
        {
        }
        Quotes(std::string sid, double pAsk, double pBid, uint32_t qAsk, uint32_t qBid, std::string cur, time_t msTimeStamp = 0)
            :_securityId(sid),
             _askPrice(pAsk),
             _bidPrice(pBid),
             _askQty(qAsk),
             _bidQty(qBid),
             _currency(cur),
             _msTimeStamp(msTimeStamp)
        {
        }

        bool operator ==(const Quotes &rhs) const
        {
            return    getSecurityId() == rhs.getSecurityId()
                   && getCurrency() == rhs.getCurrency()
                   && getBidQty() == rhs.getBidQty()
                   && getAskQty() == rhs.getAskQty()
                   && std::fabs(getBidPrice() - rhs.getBidPrice()) < 0.00001
                   && std::fabs(getAskPrice() - rhs.getAskPrice()) < 0.00001
                   && getTimeStamp() == rhs.getTimeStamp();
        }


        const std::string& getSecurityId() const { return _securityId; }
        void setSecurityId(std::string str) { _securityId=str; }

        const std::string& getCurrency() const { return _currency; }
        void setCurrency(std::string str) { _currency=str; }

        void setBidPrice(double p)
        {
            _bidPrice = p;
        }
        void setAskPrice(double p)
        {
            _askPrice = p;
        }
        void setBidQty(uint32_t q)
        {
            _bidQty = q;
        }
        void setAskPrice(uint32_t q)
        {
            _askPrice = q;
        }
        void setAskQty(uint32_t q)
        {
            _askQty = q;
        }

        void setTimeStamp(time_t ts)
        {
            _msTimeStamp = ts;
        }

        double getBidPrice() const
        {
            return _bidPrice;
        }
        double getAskPrice() const
        {
            return _askPrice;
        }
        uint32_t getBidQty() const
        {
            return _bidQty;
        }
        uint32_t getAskQty() const
        {
            return _askQty;
        }

        time_t getTimeStamp() const
        {
            return _msTimeStamp;
        }

        void dump()
        {
          LOG(INFO) <<"==================================================="\
            "================";
          LOG(INFO)  << std::left << std::setw(23) << "Security ID: "
            << _securityId <<  std::left << "  | "<< std::setw(60)
            <<"Currency " <<_currency ;
          LOG(INFO)  << std::left  << std::setw(15)<< "Bid Qty: "  << "| "
            << std::left << std::setw(15)<< "Bid Price: " << "| "
            << std::left << std::setw(15)<< "Ask Price: "<<  "| "
            << std::left << std::setw(15)<< "Ask Qty: " << "| ";
          LOG(INFO)  << std::left << std::setw(15) <<_bidQty<< "| "
            << std::left << std::setw(15) <<_bidPrice<< "| "
            << std::left << std::setw(15)<<_askPrice<< "| "
            << std::left << std::setw(15)<<_askQty<< "| ";
          LOG(INFO) <<"******************************************"\
            "*************************";
        }

    private:
        std::string _securityId;
        double      _askPrice;
        double      _bidPrice;
        uint32_t   _askQty;
        uint32_t   _bidQty;
        std::string _currency;
        time_t _msTimeStamp;         // Unix Timestamp, but with milliseconds.
    };

}

#endif	/* QUOTES_H */

