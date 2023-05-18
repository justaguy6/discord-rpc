#include <jni.h>
#include <algorithm>
#include <random>
#include <stdint.h>
#include <ctime>

extern "C" {

struct Backoff {
    int64_t minAmount;
    int64_t maxAmount;
    int64_t current;
    int fails;
    std::mt19937_64 randGenerator;
    std::uniform_real_distribution<> randDistribution;

    double rand01() { return randDistribution(randGenerator); }

    Backoff(int64_t min, int64_t max)
      : minAmount(min)
      , maxAmount(max)
      , current(min)
      , fails(0)
      , randGenerator((uint64_t)std::time(0))
    {
    }

    void reset()
    {
        fails = 0;
        current = minAmount;
    }

    int64_t nextDelay()
    {
        ++fails;
        int64_t delay = (int64_t)((double)current * 2.0 * rand01());
        current = std::min(current + delay, maxAmount);
        return current;
    }
};

JNIEXPORT jlong JNICALL Java_com_example_backoff_BackoffWrapper_create(JNIEnv* env, jobject, jlong minAmount, jlong maxAmount)
{
    return reinterpret_cast<jlong>(new Backoff(minAmount, maxAmount));
}

JNIEXPORT void JNICALL Java_com_example_backoff_BackoffWrapper_destroy(JNIEnv* env, jobject, jlong backoffPtr)
{
    delete reinterpret_cast<Backoff*>(backoffPtr);
}

JNIEXPORT void JNICALL Java_com_example_backoff_BackoffWrapper_reset(JNIEnv* env, jobject, jlong backoffPtr)
{
    reinterpret_cast<Backoff*>(backoffPtr)->reset();
}

JNIEXPORT jlong JNICALL Java_com_example_backoff_BackoffWrapper_nextDelay(JNIEnv* env, jobject, jlong backoffPtr)
{
    return reinterpret_cast<Backoff*>(backoffPtr)->nextDelay();
}

} // extern "C"
