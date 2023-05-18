#include <jni.h>
#include <string>
#include <android/log.h>

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapp_MainActivity_registerProtocol(JNIEnv* env, jobject /* this */)
{
    jclass activityClass = env->FindClass("com/example/myapp/MainActivity");
    if (activityClass == nullptr) {
        __android_log_write(ANDROID_LOG_ERROR, "RegisterProtocol", "Failed to find MainActivity class");
        return;
    }

    // Get the application context
    jclass contextClass = env->FindClass("android/content/Context");
    jmethodID getApplicationContextMethod = env->GetMethodID(contextClass, "getApplicationContext", "()Landroid/content/Context;");
    jobject applicationContext = env->CallObjectMethod(activity, getApplicationContextMethod);

    // Create an intent filter
    jclass intentFilterClass = env->FindClass("android/content/IntentFilter");
    jmethodID intentFilterConstructor = env->GetMethodID(intentFilterClass, "<init>", "()V");
    jobject intentFilter = env->NewObject(intentFilterClass, intentFilterConstructor);

    // Add an action to the intent filter
    jstring actionString = env->NewStringUTF("android.intent.action.VIEW");
    jmethodID addActionMethod = env->GetMethodID(intentFilterClass, "addAction", "(Ljava/lang/String;)V");
    env->CallVoidMethod(intentFilter, addActionMethod, actionString);

    // Set the data scheme for the intent filter
    jstring dataSchemeString = env->NewStringUTF("myapp");
    jmethodID addDataSchemeMethod = env->GetMethodID(intentFilterClass, "addDataScheme", "(Ljava/lang/String;)V");
    env->CallVoidMethod(intentFilter, addDataSchemeMethod, dataSchemeString);

    // Register the intent filter
    jclass contextWrapperClass = env->FindClass("android/content/ContextWrapper");
    jmethodID registerReceiverMethod = env->GetMethodID(contextWrapperClass, "registerReceiver", "(Landroid/content/BroadcastReceiver;Landroid/content/IntentFilter;)Landroid/content/Intent;");
    jobject broadcastReceiver = nullptr; // You need to implement a BroadcastReceiver to handle the intent
    env->CallObjectMethod(applicationContext, registerReceiverMethod, broadcastReceiver, intentFilter);

    // Clean up local references
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(intentFilterClass);
    env->DeleteLocalRef(actionString);
    env->DeleteLocalRef(dataSchemeString);
}
