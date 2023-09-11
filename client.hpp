//
//  client.hpp
//  injtest
//
//  Created by Derek Li on 9/2/23.
//

#ifndef client_hpp
#define client_hpp

#include <jvmti.h>
#include <stdio.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include "logging.hpp"
#include <vector>

class Client {
public:
    JNIEnv* env;
    JavaVM* vm;
    
    void getLoadedClasses() {
        jvmtiEnv* jvmti;
        if (vm->GetEnv((void**) &jvmti, JVMTI_VERSION_1_2) != JNI_OK) return;
        
        jclass lang = env->FindClass("java/lang/Class");
        jmethodID getName = env->GetMethodID(lang, "getName", "()Ljava/lang/String;");
        
        jclass* classesPtr;
        jint amount;
        
        jvmti->GetLoadedClasses(&amount, &classesPtr);
        std::cout << std::endl;
        std::cout << "getting " << amount << " classes" << std::endl;
        
        for (int i = 0; i < amount; i++) {
            jstring name = (jstring) env->CallObjectMethod(classesPtr[i], getName);
            const char* className = env->GetStringUTFChars(name, 0);
            
            // std::cout << className << std::endl;
            
            classes.emplace(std::make_pair((std::string) className, classesPtr[i]));
            env->ReleaseStringUTFChars(name, className);
        }
    }
    
    void getAllFields(std::string className) {
        jclass klass = getClass(className);
        if (klass == NULL) return;
        jmethodID getFields = env->GetMethodID(env->GetObjectClass(klass), "getFields", "()[Ljava/lang/reflect/Field;");
        jobjectArray fieldsList = (jobjectArray) env->CallObjectMethod(klass, getFields);
        jsize len = env->GetArrayLength(fieldsList);
        
        for (jsize i = 0; i < len; i++) {
            jobject field = env->GetObjectArrayElement(fieldsList, i);
            jclass fieldClass = env->GetObjectClass(field);
            
            jmethodID getName = env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
            jmethodID getType = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
            if (getType == NULL) {
                std::cout << "getType does not exist" << std::endl;
                return;
            }
            
            jstring fieldName = (jstring) env->CallObjectMethod(field, getName);
            
            jobject fieldType = env->CallObjectMethod(field, getType);
            jmethodID descriptorString = env->GetMethodID(env->GetObjectClass(fieldType), "descriptorString", "()Ljava/lang/String;");
            jstring fieldDescriptorString = (jstring) env->CallObjectMethod(fieldType, descriptorString);
            
            const char *cFieldName = env->GetStringUTFChars(fieldName, 0);
            const char *cDescriptorString = env->GetStringUTFChars(fieldDescriptorString, 0);
            
            std::cout << " -> " << cFieldName << std::endl;
            std::cout << "    " << cDescriptorString << std::endl;
            env->ReleaseStringUTFChars(fieldName, cFieldName);
            env->ReleaseStringUTFChars(fieldDescriptorString, cDescriptorString);
            
            env->DeleteLocalRef(field);
            env->DeleteLocalRef(fieldClass);
            env->DeleteLocalRef(fieldName);
            env->DeleteLocalRef(fieldType);
            env->DeleteLocalRef(fieldDescriptorString);
        }
        env->DeleteLocalRef(fieldsList);
    }
    
    jclass getClass(std::string className) {
        std::cout << "Getting class " << className << std::endl;
        if (classes.contains(className))
            return classes.at(className);
        std::cout << "Class " << className << " is NULL" << std::endl;
        return NULL;
    }
private:
    std::unordered_map<std::string, jclass> classes;
};

inline auto client = std::make_unique<Client>();

#endif /* client_hpp */
