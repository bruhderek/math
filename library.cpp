//
//  library.cpp
//  injtest
//
//  Created by Derek Li on 9/2/23.
//

#include <stdio.h>
#include <thread>
#include <jvmti.h>
#include <jni.h>
#include "client.hpp"
#include <chrono>
#include "logging.hpp"
#include "util/image.hpp"
#include "server.hpp"
#include "mappings/javaobject.hpp"
#include "mappings/initialize.hpp"
#include "mappings/javaobjectimpl.hpp"

void init() {
    jsize count;
    if (JNI_GetCreatedJavaVMs(&client->vm, 1, &count) != JNI_OK || count == 0) return;
    jint res = client->vm->GetEnv((void**) &client->env, JNI_VERSION_1_8);
    
    if (res == JNI_EDETACHED) res = client->vm->AttachCurrentThread((void**) &client->env, nullptr);
    
//    setupConsole();
    
    if (client->env == nullptr) return;
    
    client->getLoadedClasses();
    // lunar check
    if (client->getClass("com.lunarclient.bukkitapi.nethandler.shared.LCPacketEmoteBroadcast") == NULL) {
        std::cout << "lunar client not found" << std::endl;
        return;
    }
    
    for (int i = 0; i < 10; i++) {
        std::cout << std::endl;
    }
    std::cout << ">---------------DYLIB INJECTED----------------<" << std::endl;
    std::thread serverthread(startServer);
    serverthread.detach();
    
    initialize();
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        if (Minecraft::getMinecraft().f_theWorld() == NULL) continue;
        if (!Minecraft::getMinecraft().f_inGameHasFocus()) continue;
        
        EntityPlayerSP player = Minecraft::getMinecraft().f_thePlayer();
        PlayerControllerMP controller = Minecraft::getMinecraft().f_playerController();
        
        ArrayList entities = Minecraft::getMinecraft().f_theWorld().f_loadedEntityList().getObject();
        for (int i = 0; i < 50; i++) {
            std::cout << std::endl;
        }
        for (int i = 0; i < entities.size(); i++) {
            Entity e = entities.get(i).getObject();
            // killaura
            if (e.getDistance(player.f_posX(), player.f_posY(), player.f_posZ()) < 6.0) {
                if (e.equals(player)) continue;
                Class entityLiving(classes["net.minecraft.entity.EntityLiving"]->getClass());
                if (!entityLiving.isAssignableFrom(e.getClass())) continue;
                if (e.f_isDead()) continue;
                controller.attackEntity(player.getObject(), e.getObject());
                player.swingItem();
            }
        }
        
        
        if (client->env->ExceptionCheck() == JNI_TRUE) {
            client->env->ExceptionDescribe();
            client->env->ExceptionClear();
        }
    }
}

__attribute__((constructor)) static void install() {
    freopen("/Users/derekli/classes.txt", "w", stdout);
    freopen("/Users/derekli/classes.txt", "w", stderr);
    std::thread thread(init);
    thread.detach();
}
