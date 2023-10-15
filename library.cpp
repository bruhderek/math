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
#include "minecraft/minecraft.hpp"
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
    
    // setupConsole();
    
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
        PlayerC
    }
    
//    auto Minecraft = std::make_unique<CMinecraft>();
    
//    while (true) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(5));
//        CPlayer player = Minecraft->GetLocalPlayer();
        //            std::cout << player.isSneaking() << std::endl;
//    }
}

__attribute__((constructor)) static void install() {
    freopen("/Users/derekli/classes.txt", "w", stdout);
    freopen("/Users/derekli/classes.txt", "w", stderr);
    std::thread thread(init);
    thread.detach();
}
