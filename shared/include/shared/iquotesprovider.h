#ifndef IQUOTESPROVIDER_H
#define IQUOTESPROVIDER_H

#include "quotes.h"

namespace Algo {

/*! \class    IQuotesProvider
 *  \brief    Interface class for getting quotes
 *
 *  \see Algo::Quotes
 */
class IQuotesProvider
{
public:
    virtual ~IQuotesProvider() {}

    /*!
     * \brief Get quote by instrument name
     *
     * \param instrument a string name of the instrument
     * \return quotes for the instrument
     */
    virtual Quotes get(const std::string& instrument) = 0;
};

}

#endif // IQUOTESPROVIDER_H
