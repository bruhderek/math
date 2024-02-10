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
#include "server.hpp"
#include "javaUtils.hpp"

void init() {
    // trying to do some gui shit idk
//    embryo::module module("liblwjgl.dylib");
//    swapBuffets_ptr = reinterpret_cast<uintptr_t *>()
    // _Java_org_lwjgl_opengl_MacOSXContextImplementation_nSwapBuffers

    jsize count;
    if (JNI_GetCreatedJavaVMs(&app->vm, 1, &count) != JNI_OK || count == 0) return;
    jint res = app->vm->GetEnv((void**) &app->env, JNI_VERSION_1_8);
    
    if (res == JNI_EDETACHED) res = app->vm->AttachCurrentThread((void**) &app->env, nullptr);
    
//    setupConsole();
    
    if (app->env == nullptr) return;


    freopen("/Users/derek/classes.txt", "w", stdout);
    freopen("/Users/derek/classes.txt", "w", stderr);
    
    app->getLoadedClasses();
    // lunar check
    if (app->getClass("com.lunarclient.bukkitapi.nethandler.shared.LCPacketEmoteBroadcast") == NULL) {
        std::cout << "lunar client not found" << std::endl;
        return;
    }

    for (int i = 0; i < 10; i++) {
        std::cout << std::endl;
    }
    std::cout << ">---------------DYLIB INJECTED----------------<" << std::endl;
//    std::thread serverthread(startServer);
//    serverthread.detach();

    return;

    jclass libClass = app->getClass("me.zedik.HackClassPatcher");
    if (libClass == nullptr) {
        std::cout << "hack patcher class not found" << std::endl;
        return;
    }
    jmethodID libMethod = app->env->GetStaticMethodID(libClass, "doStuffIdk", "()V");
    if (libMethod == nullptr) {
        std::cout << "hack method not found" << std::endl;
        return;
    }
    app->env->CallStaticVoidMethod(libClass, libMethod);

    while (true) {
        if (app->env->ExceptionCheck() == JNI_TRUE) {
            std::cout << ">-----------------------EXCEPTION---------------------------<" << std::endl;
            jthrowable exception = app->env->ExceptionOccurred();
            app->env->ExceptionClear();

            JavaUtils::getInformation(exception);
            return;
        }
    }
}

__attribute__((constructor)) static void install() {
    std::thread thread(init);
    thread.detach();
}
