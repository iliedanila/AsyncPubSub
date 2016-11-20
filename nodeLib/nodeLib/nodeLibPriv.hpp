/*
 *  nodeLibPriv.hpp
 *  nodeLib
 *
 *  Created by Ilie Danila on 20/11/2016.
 *  Copyright © 2016 Ilie Danila. All rights reserved.
 *
 */

/* The classes below are not exported */
#pragma GCC visibility push(hidden)

class nodeLibPriv
{
    public:
    void HelloWorldPriv(const char *);
};

#pragma GCC visibility pop
