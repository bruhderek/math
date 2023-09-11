//
//  player.hpp
//  injtest
//
//  Created by Derek Li on 9/3/23.
//

#ifndef player_hpp
#define player_hpp

#include <stdio.h>
#include "../client.hpp"

class CPlayer {
public:
    CPlayer(jobject instance);
    void Cleanup();
    jclass GetClass();
    bool exists();
    
    bool isSneaking();
private:
    jobject playerInstance;
};

#endif /* player_hpp */
