//
//  minecraft.cpp
//  injtest
//
//  Created by Derek Li on 9/3/23.
//

#include "minecraft.hpp"
#include <thread>

jclass CMinecraft::GetClass() {
    return client->getClass("net.minecraft.client.Minecraft");
}

jobject CMinecraft::GetInstance() {
    jclass minecraftClass = this->GetClass();
    jfieldID getMinecraft = client->env->GetStaticFieldID(minecraftClass, "theMinecraft", "Lnet/minecraft/client/Minecraft;");
    jobject ret = client->env->GetStaticObjectField(minecraftClass, getMinecraft);
    
    return ret;
}

CPlayer CMinecraft::GetLocalPlayer() {
    jobject minecraftObject = this->GetInstance();
    jclass minecraftClass = this->GetClass();
    
    jfieldID getPlayer = client->env->GetFieldID(minecraftClass, "thePlayer", "Lnet/minecraft/client/entity/EntityPlayerSP;");
    
    jobject ret = client->env->GetObjectField(minecraftObject, getPlayer);
    
    client->env->DeleteLocalRef(minecraftObject);
    return CPlayer(ret);
}
