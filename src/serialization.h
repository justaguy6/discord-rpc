#include <jni.h>
#include <cstdint>
#include <ctime>
#include <cstring>
#include <cassert>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

class Backoff {
private:
    int64_t minAmount;
    int64_t maxAmount;
    int64_t current;
    int fails;
    std::mt19937_64 randGenerator;
    std::uniform_real_distribution<> randDistribution;

    double rand01() { return randDistribution(randGenerator); }

public:
    Backoff(int64_t min, int64_t max)
        : minAmount(min),
        maxAmount(max),
        current(min),
        fails(0),
        randGenerator(static_cast<uint64_t>(std::time(0))) {}

    void reset()
    {
        fails = 0;
        current = minAmount;
    }

    int64_t nextDelay()
    {
        ++fails;
        int64_t delay = static_cast<int64_t>(static_cast<double>(current) * 2.0 * rand01());
        current = std::min(current + delay, maxAmount);
        return current;
    }
};

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_backoff_BackoffWrapper_create(JNIEnv* env, jobject /* this */, jlong minAmount, jlong maxAmount)
{
    return reinterpret_cast<jlong>(new Backoff(static_cast<int64_t>(minAmount), static_cast<int64_t>(maxAmount)));
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_backoff_BackoffWrapper_destroy(JNIEnv* env, jobject /* this */, jlong backoffPtr)
{
    delete reinterpret_cast<Backoff*>(backoffPtr);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_backoff_BackoffWrapper_reset(JNIEnv* env, jobject /* this */, jlong backoffPtr)
{
    reinterpret_cast<Backoff*>(backoffPtr)->reset();
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_backoff_BackoffWrapper_nextDelay(JNIEnv* env, jobject /* this */, jlong backoffPtr)
{
    return reinterpret_cast<Backoff*>(backoffPtr)->nextDelay();
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_backoff_MainActivity_JsonWriteHandshakeObj(JNIEnv* env, jclass /* this */, jstring jDest, jint maxLen, jint version, jstring jApplicationId)
{
    const char* cDest = env->GetStringUTFChars(jDest, nullptr);
    const char* cApplicationId = env->GetStringUTFChars(jApplicationId, nullptr);

    char dest[maxLen];
    size_t written = JsonWriteHandshakeObj(dest, static_cast<size_t>(maxLen), static_cast<int>(version), cApplicationId);

    env->ReleaseStringUTFChars(jDest, cDest);
    env->ReleaseStringUTFChars(jApplicationId, cApplicationId);

    return env->NewStringUTF(dest);
}

// Rest of the functions follow the same pattern of receiving and returning Java types, with appropriate type conversions and error handling.

// Rest of the code remains the same.
