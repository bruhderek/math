//
// Created by Derek Li on 11/13/23.
//

#include "patcher.h"
#include "../client.hpp"

void JNICALL ClassFileLoadHook (
    jvmtiEnv* jvmti_env,
    JNIEnv* jni_env,
    jclass class_being_redefined,
    jobject loader,
    const char* name,
    jobject protection_domain,
    jint class_data_len,
    const unsigned char* class_data,
    jint* new_class_data_len,
    unsigned char** new_class_data) {

    std::function<void(const std::string&)> patchClass = [=](const std::string& patchMethod)
    {
        jclass ClassPatcherClass = app->getClass("me.zedik.HackClassPatcher");
        jbyteArray original_class_bytes = jni_env->NewByteArray(class_data_len);
        jni_env->SetByteArrayRegion(original_class_bytes, 0, class_data_len, (const jbyte*)class_data);

        jmethodID patchMethodID = jni_env->GetStaticMethodID(ClassPatcherClass, patchMethod.c_str(), "([BLjava/lang/String;)[B");
        if (patchMethodID == nullptr) {
            std::cout << "Could not find method id " << patchMethod << std::endl;
            return;
        }
        jstring ThreadContextClassName = jni_env->NewStringUTF("org/apache/logging/log4j/ThreadContext");

        auto new_class_bytes = (jbyteArray)jni_env->CallStaticObjectMethod(
                ClassPatcherClass,
                patchMethodID,
                original_class_bytes,
                ThreadContextClassName
        );

        jni_env->DeleteLocalRef(ThreadContextClassName);

        jni_env->DeleteLocalRef(original_class_bytes);
        *new_class_data_len = jni_env->GetArrayLength(new_class_bytes);
        jvmti_env->Allocate(*new_class_data_len, new_class_data);
        jni_env->GetByteArrayRegion(new_class_bytes, 0, *new_class_data_len, (jbyte*)*new_class_data);
        jni_env->DeleteLocalRef(new_class_bytes);
        std::cout << "Successfully patched " << patchMethod << std::endl;
    };

    if (jni_env->IsSameObject(class_being_redefined, app->getClass("net.minecraft.client.renderer.EntityRenderer"))) {
        patchClass("patchEntityRenderer");
    }
    if (jni_env->IsSameObject(class_being_redefined, app->getClass("net.minecraft.client.entity.EntityPlayerSP"))) {
        patchClass("patchOnTick");
    }

}

void Patcher::setup(jvmtiEnv *tiEnv) {
    jvmtiCapabilities capabilities{};
    capabilities.can_retransform_classes = JVMTI_ENABLE;
    tiEnv->AddCapabilities(&capabilities);
    jvmtiEventCallbacks callbacks{};
    callbacks.ClassFileLoadHook = &ClassFileLoadHook;
    tiEnv->SetEventCallbacks(&callbacks, sizeof(jvmtiEventCallbacks));
    tiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, nullptr);
}

void Patcher::transformClasses(jvmtiEnv *tiEnv) {
    jclass CEntityRenderer = app->getClass("net.minecraft.client.renderer.EntityRenderer");
    jclass CEntityPlayerSP = app->getClass("net.minecraft.client.entity.EntityPlayerSP");

    jclass classes[] = {
            CEntityRenderer, CEntityPlayerSP
    };
    tiEnv->RetransformClasses(2, classes);
    tiEnv->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, nullptr);
}
