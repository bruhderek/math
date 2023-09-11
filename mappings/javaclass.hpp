//
//  javaclass.hpp
//  math
//
//  Created by Derek Li on 9/24/23.
//

#ifndef javaclass_hpp
#define javaclass_hpp

#include <stdio.h>
#include <cstdarg>
#include <vector>
#include "../client.hpp"

class JavaClass {
private:
    const jclass klass;
    const std::string path;
public:
    JavaClass(jclass klass, std::string path) : klass(klass), path(path) {}
    
    jclass getClass() const {
        return this->klass;
    }
    
    const std::string *getPath() {
        return &this->path;
    }
    
    template <class T>
    T *invokeStaticMethod(const char *methodName, const char *methodSignature, ...) {
        va_list arg;
        va_start(arg, methodSignature);
        jmethodID id = client->env->GetStaticMethodID(this->klass, methodName, methodSignature);
        
        if constexpr (std::is_same_v<T, void>) {
            client->env->CallStaticVoidMethodV(this->klass, id, arg);
            va_end(arg);
            return nullptr;
        } else {
            T ret;
            if constexpr (std::is_same_v<T, int>) {
                ret = client->env->CallStaticIntMethodV(this->klass, id, arg);
            }
            if constexpr (std::is_same_v<T, short>) {
                ret = client->env->CallStaticShortMethodV(this->klass, id, arg);
            }
            if constexpr (std::is_same_v<T, long>) {
                ret = client->env->CallStaticLongMethodV(this->klass, id, arg);
            }
            if constexpr (std::is_same_v<T, float>) {
                ret = client->env->CallStaticFloatMethodV(this->klass, id, arg);
            }
            if constexpr (std::is_same_v<T, double>) {
                ret = client->env->CallStaticDoubleMethodV(this->klass, id, arg);
            }
            if constexpr (std::is_same_v<T, bool>) {
                ret = client->env->CallStaticBooleanMethodV(this->klass, id, arg);
            }
            if constexpr (std::is_same_v<T, char>) {
                ret = client->env->CallStaticCharMethodV(this->klass, id, arg);
            }
            if constexpr (std::is_same_v<T, jobject> ||
                          std::is_same_v<T, jobjectArray> ||
                          std::is_same_v<T, jintArray> ||
                          std::is_same_v<T, jshortArray> ||
                          std::is_same_v<T, jlongArray> ||
                          std::is_same_v<T, jfloatArray> ||
                          std::is_same_v<T, jdoubleArray> ||
                          std::is_same_v<T, jbooleanArray> ||
                          std::is_same_v<T, jcharArray>) {
                ret = client->env->CallStaticObjectMethodV(this->klass, id, arg);
            }
            va_end(arg);
            return &ret;
        }
    }
    
    template <class T>
    T *invokeMethod(jobject object, const char *methodName, const char *methodSignature, ...) {
        va_list arg;
        va_start(arg, methodSignature);
        jmethodID id = client->env->GetMethodID(this->klass, methodName, methodSignature);
        
        if constexpr (std::is_same_v<T, void>) {
            client->env->CallVoidMethodV(object, id, arg);
            va_end(arg);
            return nullptr;
        } else {
            T ret;
            if constexpr (std::is_same_v<T, int>) {
                ret = client->env->CallIntMethodV(object, id, arg);
            }
            if constexpr (std::is_same_v<T, short>) {
                ret = client->env->CallShortMethodV(object, id, arg);
            }
            if constexpr (std::is_same_v<T, long>) {
                ret = client->env->CallLongMethodV(object, id, arg);
            }
            if constexpr (std::is_same_v<T, float>) {
                ret = client->env->CallFloatMethodV(object, id, arg);
            }
            if constexpr (std::is_same_v<T, double>) {
                ret = client->env->CallDoubleMethodV(object, id, arg);
            }
            if constexpr (std::is_same_v<T, bool>) {
                ret = client->env->CallBooleanMethodV(object, id, arg);
            }
            if constexpr (std::is_same_v<T, char>) {
                ret = client->env->CallCharMethodV(object, id, arg);
            }
            if constexpr (std::is_same_v<T, jobject> ||
                          std::is_same_v<T, jobjectArray> ||
                          std::is_same_v<T, jintArray> ||
                          std::is_same_v<T, jshortArray> ||
                          std::is_same_v<T, jlongArray> ||
                          std::is_same_v<T, jfloatArray> ||
                          std::is_same_v<T, jdoubleArray> ||
                          std::is_same_v<T, jbooleanArray> ||
                          std::is_same_v<T, jcharArray>) {
                ret = client->env->CallObjectMethodV(object, id, arg);
            }
            va_end(arg);
            return &ret;
        }
    }
};

#endif /* javaclass_hpp */
