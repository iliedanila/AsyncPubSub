#ifndef nodeErrors_hpp
#define nodeErrors_hpp

namespace NetworkLayer
{
    enum SendError
    {
        eSuccess,
        eNoPath,
        eNodeNotAccepting
    };
}

#endif /* nodeErrors_hpp */
