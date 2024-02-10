//
//  client.hpp
//  injtest
//
//  Created by Derek Li on 9/2/23.
//

#ifndef client_hpp
#define client_hpp

#include <jvmti.h>
#include <cstdio>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "javaUtils.hpp"
#include "java/asmjar.h"
#include "java/libjar.h"
#include "patcher/patcher.h"
#include "java/jasperfinder.h"
#include "java/betterkeystrokes.h"
#include <filesystem>
#include <fstream>

class Client {
public:
    JNIEnv* env;
    JavaVM* vm;

    void addLoadedClasses(jvmtiEnv* jvmti) {
        jclass lang = env->FindClass("java/lang/Class");
        jmethodID getName = env->GetMethodID(lang, "getName", "()Ljava/lang/String;");

        jclass *classesPtr;
        jint amount;

        jvmti->GetLoadedClasses(&amount, &classesPtr);
        std::cout << std::endl;
        std::cout << "getting " << amount << " classes" << std::endl;

        for (int i = 0; i < amount; i++) {
            jstring name = (jstring) env->CallObjectMethod(classesPtr[i], getName);
            const char *className = env->GetStringUTFChars(name, 0);
            /*
            if (strcmp(className, "java.lang.String") == 0) {
                std::cout << className << ": " << std::endl;
                jmethodID* methodPtr;
                jint methodAmount;
                jvmti->GetClassMethods(classesPtr[i], &methodAmount, &methodPtr);
                for (int j = 0; j < methodAmount; j++) {
                    char *name;
                    char *signature;
                    char *generic;
                    jvmti->GetMethodName(methodPtr[j], &name, &signature, &generic);
                    std::cout << "  Name: " << name << std::endl;
                    std::cout << "  signature: " << signature << std::endl;
                }
            }
             */
            classes.emplace((std::string) className, classesPtr[i]);
            env->ReleaseStringUTFChars(name, className);
        }
    }

    static void deleteDirectoryContents(const std::filesystem::path& dir)
    {
        for (const auto& entry : std::filesystem::directory_iterator(dir))
            std::filesystem::remove_all(entry.path());
    }
    
    void getLoadedClasses() {
        jvmtiEnv* jvmti;
        if (vm->GetEnv((void**) &jvmti, JVMTI_VERSION_1_2) != JNI_OK) return;
        Patcher::setup(jvmti);

        deleteDirectoryContents("/Users/derek/classpath");

        addLoadedClasses(jvmti);

        jobject classLoader = JavaUtils::getClassLoader();
//        JavaUtils::loadJar(classLoader, asm_bytes, asm_size);
        JavaUtils::loadJar(classLoader, lib_bytes, lib_size);
//        JavaUtils::loadModJar(classLoader, jasperfinder_bytes, jasperfinder_size);
//        JavaUtils::loadModJar(classLoader, betterkeystrokes_bytes, betterkeystrokes_size);

        std::string path = "/Users/derek/mods";
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            std::cout << "LOADING MOD " << entry.path() << std::endl;
            std::ifstream input(entry.path(), std::ios::binary);
            auto *bytes = new std::vector<char>(
                    (std::istreambuf_iterator<char>(input)),
                    (std::istreambuf_iterator<char>()));
            input.close();
            std::cout << "size " << bytes->size() << std::endl;
            auto* u_bytes = new unsigned char[bytes->size()];
            std::cout << "size " << sizeof(u_bytes) << std::endl;
            for (int i = 0; i < bytes->size(); i++) {
                u_bytes[i] = (unsigned char) (*bytes)[i];
            }
            JavaUtils::loadModJar(classLoader, u_bytes, sizeof(unsigned char) * bytes->size());
            delete bytes;
            delete[] u_bytes;
        }

        // load the resources
        JavaUtils::addClassLoaderURL();

        std::cout << "Class bytes loaded" << std::endl;

        addLoadedClasses(jvmti);

        // add the patches
        Patcher::transformClasses(jvmti);
        std::cout << "Done transforming classes" << std::endl;

        JavaUtils::injectMods();
        std::cout << "Inject mods success" << std::endl;
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
        if (classes.contains(className))
            return classes.at(className);
        return NULL;
    }
private:
    std::unordered_map<std::string, jclass> classes;
};

inline auto app = std::make_unique<Client>();

#endif /* client_hpp */
