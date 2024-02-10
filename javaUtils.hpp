//
//  javaUtils.hpp
//  math
//
//  Created by Derek Li on 10/19/23.
//

#ifndef javaUtils_hpp
#define javaUtils_hpp

#include <iostream>
#include <vector>
#include <jvmti.h>

namespace JavaUtils {
std::string getString(jobject s);
void loadJar(jobject classLoader, const unsigned char *jarBytes, size_t size);
jobject newClassLoader();
jobject getClassLoader();
void getInformation(jthrowable exception);
void loadModJar(jobject classLoader, const unsigned char *jarBytes, size_t size);
void addClassLoaderURL();

    void injectMods();

    inline std::vector<std::string> modClasses;

    void initialize();
}

#endif /* javaUtils_hpp */
