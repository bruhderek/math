//
// Created by Derek Li on 11/13/23.
//

#ifndef MATH_PATCHER_H
#define MATH_PATCHER_H

#include <jvmti.h>

namespace Patcher {
    void setup(jvmtiEnv *tiEnv);
    void transformClasses(jvmtiEnv *tiEnv);
}

#endif //MATH_PATCHER_H
