//
//  player.cpp
//  injtest
//
//  Created by Derek Li on 9/3/23.
//

#include "player.hpp"

CPlayer::CPlayer(jobject instance) {
    this->playerInstance = instance;
}

void CPlayer::Cleanup() {
    client->env->DeleteLocalRef(this->playerInstance);
}

jclass CPlayer::GetClass() {
    return client->getClass("net.minecraft.entity.Entity");
}

bool CPlayer::isSneaking() {
    jclass entityClass = this->GetClass();
    jmethodID isSneaking = client->env->GetMethodID(entityClass, "isSneaking", "()Z");
    
    bool ret = (bool) client->env->CallBooleanMethod(this->playerInstance, isSneaking);
    return ret;
}

bool CPlayer::exists() {
    return !client->env->IsSameObject(this->playerInstance, NULL);
}
