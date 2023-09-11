//
//  minecraft.hpp
//  injtest
//
//  Created by Derek Li on 9/3/23.
//

#ifndef minecraft_hpp
#define minecraft_hpp

#include <stdio.h>
#include "../client.hpp"
#include "player.hpp"

class CMinecraft {
public:
    jclass GetClass();
    jobject GetInstance();
    
    CPlayer GetLocalPlayer();
};

#endif /* minecraft_hpp */
