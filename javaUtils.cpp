//
//  javaUtils.cpp
//  math
//
//  Created by Derek Li on 10/19/23.
//

#include "javaUtils.hpp"
#include "miniz.h"
#include "client.hpp"

#define checkNull(x) checkNull_(#x, x)

template <class T>
void checkNull_(const std::string &varName, T x) {
    if (!x) {
        std::cout << varName << " is null" << std::endl;
        throw std::runtime_error(varName + " is null");}
}

void JavaUtils::loadJar(jobject classLoader, const unsigned char *jarBytes, size_t size) {
    mz_zip_archive archive{};
    if (!mz_zip_reader_init_mem(&archive, jarBytes, size, 0)) return;
    mz_uint file_number = mz_zip_reader_get_num_files(&archive);
    for (mz_uint i = 0; i < file_number; i++) {
        if (!mz_zip_reader_is_file_supported(&archive, i)) continue;

        char str[256] = {0};
        mz_zip_reader_get_filename(&archive, i, str, 256);
        std::string fileName(str);

        if (fileName.size() <= 6) continue;
        if (fileName.substr(fileName.size() - 6) != ".class") continue;

        std::cout << "Loading " << fileName << std::endl;
        
        size_t classBytes_size = 0;
        auto *classBytes = (unsigned char*) mz_zip_reader_extract_to_heap(&archive, i, &classBytes_size, 0);
        if (!classBytes) {
            mz_zip_reader_end(&archive);
            return;
        }

        jclass jaclass = app->env->DefineClass(nullptr, classLoader, (const jbyte*) classBytes, (jsize) classBytes_size);
        if (jaclass) app->env->DeleteLocalRef(jaclass);
        else std::cout << "Failed to load class " << fileName << std::endl;
        mz_free(classBytes);
    }
    mz_zip_reader_end(&archive);
}


void JavaUtils::loadModJar(jobject classLoader, const unsigned char *jarBytes, size_t size) {
    mz_zip_archive archive{};
    if (!mz_zip_reader_init_mem(&archive, jarBytes, size, 0)) return;
    mz_uint file_number = mz_zip_reader_get_num_files(&archive);
    for (mz_uint i = 0; i < file_number; i++) {
        if (!mz_zip_reader_is_file_supported(&archive, i)) continue;

        char str[256] = {0};
        mz_zip_reader_get_filename(&archive, i, str, 256);
        std::string fileName(str);

//        if (fileName.size() <= 6) continue;
//        if (fileName.substr(fileName.size() - 6) != ".class") continue;

        std::cout << "Loading " << fileName << std::endl;

        size_t classBytes_size = 0;
        auto *classBytes = (unsigned char*) mz_zip_reader_extract_to_heap(&archive, i, &classBytes_size, 0);
        if (!classBytes) {
            mz_zip_reader_end(&archive);
            return;
        }

        JavaUtils::modClasses.push_back(fileName);

        std::filesystem::path p = "/Users/derek/classpath/";
        p += fileName;
        std::filesystem::create_directories(p.parent_path());
        std::ofstream of;
        of.open(p, std::ios::binary | std::ios::out);
        of.write((const char *) classBytes, classBytes_size);
        of.close();

//        jclass jaclass = app->env->DefineClass(nullptr, classLoader, (const jbyte*) classBytes, (jsize) classBytes_size);
//        if (jaclass) {
//            JavaUtils::modClasses.push_back(jaclass);
//            app->env->DeleteLocalRef(jaclass);
//        }
//        else {
//            std::cout << "Failed to load class " << fileName << std::endl;
//            if (app->env->ExceptionCheck() == JNI_TRUE) {
//                std::cout << "Exception loading class:" << std::endl;
//                jthrowable exception = app->env->ExceptionOccurred();
//                app->env->ExceptionClear();
//
//                JavaUtils::getInformation(exception);
//            }
//        }
        mz_free(classBytes);
    }
    mz_zip_reader_end(&archive);
}

jobject JavaUtils::newClassLoader() {
    jclass urlClass = app->env->FindClass("java/net/URL");
//    jmethodID urlContructor = app->env->GetMethodID(urlClass, "<init>", "(Ljava/lang/String;)V");
//    jstring str = app->env->NewStringUTF("file://ftp.yoyodyne.com/pub/files/foobar.txt");
//    jobject url = app->env->NewObject(urlClass, urlContructor, str);
    jobjectArray urls = app->env->NewObjectArray(0, urlClass, nullptr);
    jclass URLClassLoaderClass = app->env->FindClass("java/net/URLClassLoader");
    jmethodID URLClassLoaderContructor = app->env->GetMethodID(URLClassLoaderClass, "<init>", "([Ljava/net/URL;)V");
    jobject URLClassLoader = app->env->NewObject(URLClassLoaderClass, URLClassLoaderContructor, urls);

    app->env->DeleteLocalRef(urlClass);
//    app->env->DeleteLocalRef(url);
//    app->env->DeleteLocalRef(str);
    app->env->DeleteLocalRef(urls);
    app->env->DeleteLocalRef(URLClassLoaderClass);

    return URLClassLoader;
}

jobject JavaUtils::getClassLoader() {
    std::cout << "Getting class loader" << std::endl;
    jclass classClass = app->getClass("java.lang.Class");
    checkNull(classClass);
    jclass mcClass = app->getClass("net.minecraft.client.Minecraft");
    checkNull(mcClass);
    jmethodID getClassLoader = app->env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    checkNull(getClassLoader);
    jobject classLoader = app->env->CallObjectMethod(mcClass, getClassLoader);
    checkNull(classLoader);

    if (classLoader == nullptr) std::cout << "ClassLoader is null" << std::endl;

    return classLoader;
}

void append_exception_trace_messages(
        JNIEnv&      a_jni_env,
        std::string& a_error_msg,
        jthrowable   a_exception,
        jmethodID    a_mid_throwable_getCause,
        jmethodID    a_mid_throwable_getStackTrace,
        jmethodID    a_mid_throwable_toString,
        jmethodID    a_mid_frame_toString);

void JavaUtils::getInformation(jthrowable exception) {
    jclass throwable_class = app->env->FindClass("java/lang/Throwable");
    jmethodID mid_throwable_getCause =
            app->env->GetMethodID(throwable_class,
                                  "getCause",
                                  "()Ljava/lang/Throwable;");
    jmethodID mid_throwable_getStackTrace =
            app->env->GetMethodID(throwable_class,
                                  "getStackTrace",
                                  "()[Ljava/lang/StackTraceElement;");
    jmethodID mid_throwable_toString =
            app->env->GetMethodID(throwable_class,
                                  "toString",
                                  "()Ljava/lang/String;");

    jclass frame_class = app->env->FindClass("java/lang/StackTraceElement");
    jmethodID mid_frame_toString =
            app->env->GetMethodID(frame_class,
                                  "toString",
                                  "()Ljava/lang/String;");

    std::string error_msg;
    append_exception_trace_messages(*app->env, error_msg, exception, mid_throwable_getCause, mid_throwable_getStackTrace, mid_throwable_toString, mid_frame_toString);
    std::cout << error_msg << std::endl;
}

/**
 * Credits: hmjd from stackoverflow
 * @param a_jni_env
 * @param a_error_msg
 * @param a_exception
 * @param a_mid_throwable_getCause
 * @param a_mid_throwable_getStackTrace
 * @param a_mid_throwable_toString
 * @param a_mid_frame_toString
 */
void append_exception_trace_messages(
        JNIEnv&      a_jni_env,
        std::string& a_error_msg,
        jthrowable   a_exception,
        jmethodID    a_mid_throwable_getCause,
        jmethodID    a_mid_throwable_getStackTrace,
        jmethodID    a_mid_throwable_toString,
        jmethodID    a_mid_frame_toString)
{
    // Get the array of StackTraceElements.
    jobjectArray frames =
            (jobjectArray) a_jni_env.CallObjectMethod(
                    a_exception,
                    a_mid_throwable_getStackTrace);
    jsize frames_length = a_jni_env.GetArrayLength(frames);

    // Add Throwable.toString() before descending
    // stack trace messages.
    if (0 != frames)
    {
        jstring msg_obj =
                (jstring) a_jni_env.CallObjectMethod(a_exception,
                                                     a_mid_throwable_toString);
        const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

        // If this is not the top-of-the-trace then
        // this is a cause.
        if (!a_error_msg.empty())
        {
            a_error_msg += "\nCaused by: ";
            a_error_msg += msg_str;
        }
        else
        {
            a_error_msg = msg_str;
        }

        a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
        a_jni_env.DeleteLocalRef(msg_obj);
    }

    // Append stack trace messages if there are any.
    if (frames_length > 0)
    {
        jsize i = 0;
        for (i = 0; i < frames_length; i++)
        {
            // Get the string returned from the 'toString()'
            // method of the next frame and append it to
            // the error message.
            jobject frame = a_jni_env.GetObjectArrayElement(frames, i);
            jstring msg_obj =
                    (jstring) a_jni_env.CallObjectMethod(frame,
                                                         a_mid_frame_toString);

            const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

            a_error_msg += "\n    ";
            a_error_msg += msg_str;

            a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
            a_jni_env.DeleteLocalRef(msg_obj);
            a_jni_env.DeleteLocalRef(frame);
        }
    }

    // If 'a_exception' has a cause then append the
    // stack trace messages from the cause.
    if (0 != frames)
    {
        jthrowable cause =
                (jthrowable) a_jni_env.CallObjectMethod(
                        a_exception,
                        a_mid_throwable_getCause);
        if (0 != cause)
        {
            append_exception_trace_messages(a_jni_env,
                                             a_error_msg,
                                             cause,
                                             a_mid_throwable_getCause,
                                             a_mid_throwable_getStackTrace,
                                             a_mid_throwable_toString,
                                             a_mid_frame_toString);
        }
    }
}

void throwError(std::string s) {
    std::cout << "ERROR: " << s << std::endl;
    throw std::exception();
}

jobjectArray getModClassesArray() {
    jclass injector_class = app->getClass("me.zedik.ForgeInjector");
    checkNull(injector_class);
    jfieldID fstringArrayClass = app->env->GetStaticFieldID(injector_class, "stringArrayClass", "Ljava/lang/Class;");
    checkNull(fstringArrayClass);
    auto stringArrayClass = (jclass) (app->env->GetStaticObjectField(injector_class, fstringArrayClass));
    checkNull(stringArrayClass);
    jobjectArray modNames = app->env->NewObjectArray((jsize) JavaUtils::modClasses.size(), app->getClass("java.lang.String"), nullptr);
    checkNull(modNames);
    for (int i = 0; i < JavaUtils::modClasses.size(); i++) {
        jstring classNameString = app->env->NewStringUTF(JavaUtils::modClasses[i].c_str());
        if (classNameString == nullptr) std::cout << "classNameString is nullptr" << std::endl;
        app->env->SetObjectArrayElement(modNames, i, classNameString);
        app->env->DeleteLocalRef(classNameString);
    }
    return modNames;
}

void JavaUtils::injectMods() {
    std::cout << "Injecting mods" << std::endl;
    jclass injector_class = app->getClass("me.zedik.ForgeInjector");
    const auto inject_method_id = app->env->GetStaticMethodID(injector_class,
                                                              "inject", "([Ljava/lang/String;)V");
    if (!inject_method_id) {
        std::cout << ("Failed to find inject method ID") << std::endl;
        return;
    }

    std::cout << "Inject method ID found" << std::endl;

    app->env->CallStaticVoidMethod(injector_class, inject_method_id, getModClassesArray());
    std::cout << "Success" << std::endl;
}

void JavaUtils::addClassLoaderURL() {
    jobject classLoader = getClassLoader();
    checkNull(classLoader);
    jclass URLClassLoader = app->getClass("java.net.URLClassLoader");
    checkNull(URLClassLoader);
    jclass File = app->getClass("java.io.File");
    checkNull(File);
    jclass URI = app->getClass("java.net.URI");
    checkNull(URI);
    jmethodID fileConstructor = app->env->GetMethodID(File, "<init>", "(Ljava/lang/String;)V");
    checkNull(fileConstructor);
    jstring classpath = app->env->NewStringUTF("/Users/derek/classpath");
    checkNull(classpath);
    jobject fileObject = app->env->NewObject(File, fileConstructor, classpath);
    checkNull(fileObject);
    jmethodID toURI = app->env->GetMethodID(File, "toURI", "()Ljava/net/URI;");
    checkNull(toURI);
    jmethodID toURL = app->env->GetMethodID(URI, "toURL", "()Ljava/net/URL;");
    checkNull(toURL);
    jobject URIObject = app->env->CallObjectMethod(fileObject, toURI);
    checkNull(toURL);
    jobject URLObject = app->env->CallObjectMethod(URIObject, toURL);
    checkNull(URLObject);
    jmethodID addURL = app->env->GetMethodID(URLClassLoader, "addURL", "(Ljava/net/URL;)V");
    checkNull(addURL);
    app->env->CallVoidMethod(classLoader, addURL, URLObject);
    if (app->env->ExceptionCheck() == JNI_TRUE) {
        std::cout << ">-----------------------EXCEPTION---------------------------<" << std::endl;
        jthrowable exception = app->env->ExceptionOccurred();
        app->env->ExceptionClear();

        JavaUtils::getInformation(exception);
        return;
    }
    std::cout << "Successfully added path to classloader" << std::endl;
}