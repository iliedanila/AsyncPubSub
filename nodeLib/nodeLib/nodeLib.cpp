/*
 *  nodeLib.cpp
 *  nodeLib
 *
 *  Created by Ilie Danila on 20/11/2016.
 *  Copyright © 2016 Ilie Danila. All rights reserved.
 *
 */

#include <iostream>
#include "nodeLib.hpp"
#include "nodeLibPriv.hpp"

void nodeLib::HelloWorld(const char * s)
{
    nodeLibPriv *theObj = new nodeLibPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void nodeLibPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

