#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <jni.h>

class BaseConnection {
public:
    static BaseConnection* Create();
    static void Destroy(BaseConnection*&);

    bool isOpen{false};

    bool Open();
    bool Close();
    bool Write(const void* data, size_t length);
    bool Read(void* data, size_t length);

private:
    jobject mObject{nullptr};
    jmethodID mOpenMethod{nullptr};
    jmethodID mCloseMethod{nullptr};
    jmethodID mWriteMethod{nullptr};
    jmethodID mReadMethod{nullptr};
};

BaseConnection* BaseConnection::Create() {
    static BaseConnection connection;
    return &connection;
}

void BaseConnection::Destroy(BaseConnection*& c) {
    delete c;
    c = nullptr;
}

bool BaseConnection::Open() {
    if (mObject) {
        return false;
    }

    JNIEnv* env;
    if (JNI_OK != ::JNI_GetCreatedJavaVMs(&mJVM, 1, nullptr)) {
        return false;
    }
    if (JNI_OK != mJVM->AttachCurrentThread(&env, nullptr)) {
        return false;
    }

    jclass clazz = env->FindClass("com/example/Connection"); // Altere o nome da classe de acordo com a sua implementação
    if (!clazz) {
        return false;
    }

    mObject = env->AllocObject(clazz);
    if (!mObject) {
        return false;
    }

    mOpenMethod = env->GetMethodID(clazz, "open", "()Z");
    mCloseMethod = env->GetMethodID(clazz, "close", "()Z");
    mWriteMethod = env->GetMethodID(clazz, "write", "([B)Z");
    mReadMethod = env->GetMethodID(clazz, "read", "([B)Z");

    if (!mOpenMethod || !mCloseMethod || !mWriteMethod || !mReadMethod) {
        return false;
    }

    return env->CallBooleanMethod(mObject, mOpenMethod) == JNI_TRUE;
}

bool BaseConnection::Close() {
    if (!mObject) {
        return false;
    }

    JNIEnv* env;
    if (JNI_OK != mJVM->AttachCurrentThread(&env, nullptr)) {
        return false;
    }

    return env->CallBooleanMethod(mObject, mCloseMethod) == JNI_TRUE;
}

bool BaseConnection::Write(const void* data, size_t length) {
    if (!mObject || !data || length == 0) {
        return false;
    }

    JNIEnv* env;
    if (JNI_OK != mJVM->AttachCurrentThread(&env, nullptr)) {
        return false;
    }

    jbyteArray byteArray = env->NewByteArray((jsize)length);
    env->SetByteArrayRegion(byteArray, 0, (jsize)length, (const jbyte*)data);

    return env->CallBooleanMethod(mObject, mWriteMethod, byteArray) == JNI_TRUE;
}

bool BaseConnection::Read(void* data, size_t length) {
    if (!mObject || !data || length == 0) {
        return false;
    }

    JNIEnv* env;
    if (JNI_OK != mJVM->AttachCurrentThread(&env, nullptr)) {
        return false;
    }

    jbyteArray byteArray = env->NewByteArray((jsize)length);
    bool result = env->CallBooleanMethod(mObject, mReadMethod, byteArray) == JNI_TRUE;
    if (result) {
        env->GetByteArrayRegion
