/* 
 * File:   common.h
 * Author: divya
 *
 * Created on May 8, 2015, 9:45 PM
 */

#ifndef COMMON_H
#define	COMMON_H

#include <string>
#include "defines.h"

namespace Algo {
    struct Elements {
        Elements() : _size{0} {
            memset(_clientName, 0, CLIENT_NAME_SIZE);
            memset(_elements, 0, MAX_BUF);
        }

        int _size;
        char _clientName[CLIENT_NAME_SIZE];
        char _elements[MAX_BUF];
    };
}

#endif	/* COMMON_H */

