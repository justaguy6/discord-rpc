#pragma once

#include <jni.h>
#include <android/log.h>
#include <cstdint>
#include <cstdlib>

class BaseConnection {
public:
    static BaseConnection* Create();
    static void Destroy(BaseConnection*& conn);
    bool isOpen{false};
    bool Open();
    bool Close();
    bool Write(const void* data, size_t length);
    bool Read(void* data, size_t length);

private:
    BaseConnection();
    ~BaseConnection();
    jclass jConnectionClass;
    jobject jConnectionObj;
    jmethodID jOpenMethod;
    jmethodID jCloseMethod;
    jmethodID jWriteMethod;
    jmethodID jReadMethod;
    JNIEnv* jniEnv;
};

BaseConnection::BaseConnection() {
    jniEnv = nullptr;
    jConnectionClass = nullptr;
    jConnectionObj = nullptr;
    jOpenMethod = nullptr;
    jCloseMethod = nullptr;
    jWriteMethod = nullptr;
    jReadMethod = nullptr;
}

BaseConnection::~BaseConnection() {
    if (jConnectionObj != nullptr) {
        jniEnv->DeleteGlobalRef(jConnectionObj);
    }
}

BaseConnection* BaseConnection::Create() {
    auto conn = new BaseConnection();
    JNIEnv* env = nullptr;
    if (JNI_OK != ::JNI_GetCreatedJavaVMs(&env, 1, nullptr)) {
        __android_log_print(ANDROID_LOG_ERROR, "BaseConnection", "Could not get JNI environment");
        return nullptr;
    }
    conn->jniEnv = env;
    jclass connectionClass = env->FindClass("com/example/Connection"); // Nome da classe de conexão no seu projeto Android
    if (connectionClass == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "BaseConnection", "Could not find class com.example.Connection");
        return nullptr;
    }
    conn->jConnectionClass = (jclass) env->NewGlobalRef(connectionClass);
    jmethodID constructor = env->GetMethodID(conn->jConnectionClass, "<init>", "()V");
    if (constructor == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "BaseConnection", "Could not find constructor for class com.example.Connection");
        return nullptr;
    }
    jobject connectionObj = env->NewObject(conn->jConnectionClass, constructor);
    if (connectionObj == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "BaseConnection", "Could not create instance of class com.example.Connection");
        return nullptr;
    }
    conn->jConnectionObj = env->NewGlobalRef(connectionObj);
    conn->jOpenMethod = env->GetMethodID(conn->jConnectionClass, "open", "()Z");
    conn->jCloseMethod = env->GetMethodID(conn->jConnectionClass, "close", "()Z");
    conn->jWriteMethod = env->GetMethodID(conn->jConnectionClass, "write", "([B)Z");
    conn->jReadMethod = env->GetMethodID(conn->jConnectionClass, "read", "([B)Z");
    return conn;
}

void BaseConnection::Destroy(BaseConnection*& conn) {
    if (conn != nullptr) {
        delete conn;
        conn = nullptr;
    }
}

bool BaseConnection::Open() {
    if (jOpenMethod == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "BaseConnection", "Connection method open not initialized");
        return false;
    }
    if (jniEnv->CallBooleanMethod(jConnectionObj, jOpenMethod)) {
        isOpen = true;
        return true;
    } else {
        return false;
    }
}

bool Base
