//
//  javaobject.hpp
//  math
//
//  Created by Derek Li on 10/7/23.
//

#ifndef javaobject_hpp
#define javaobject_hpp

#include <iostream>
#include <vector>
#include <cstdarg>
#include "javaclass.hpp"
#include "../client.hpp"
#define STATICINITIALIZE static void initialize() { javaClass = new JavaClass(client->getClass(className), std::string(className)); }

typedef unsigned char byte;

class JavaObject {
protected:
    const inline static char *className = "java.lang.Object";
    inline static JavaClass *javaClass = nullptr;
    jobject object;
public:
    STATICINITIALIZE;
    
    JavaObject(jobject object) {
        this->object = object;
    }
    
    template <typename... Ts>
    JavaObject(JavaClass *klass, const char *signature, const Ts&... args) {
        jmethodID constructor = client->env->GetMethodID(klass->getClass(), "<init>", signature);
        jobject newObject = client->env->NewObject(klass->getClass(), constructor, args...);
        this->object = newObject;
    }
    
    operator jobject() {
        return this->object;
    }
};

#endif
