#include "CTCB.hpp"

#include <jni.h>

extern "C"{
JNIEXPORT jintArray JNICALL Java_com_z_r_runCtc(JNIEnv *env, jclass, jintArray jniMergedCopus, jfloatArray jniMergedMat) {


    std::vector<uint16_t> mergedCopus;
    std::vector<float> mergedMat;
    {
        const jsize length = env->GetArrayLength(jniMergedCopus);
        jint *oarr = env->GetIntArrayElements(jniMergedCopus, nullptr);

        for (int i = 0; i < length; i++){
            mergedCopus.push_back(oarr[i]);
        }
    }
    {
        const jsize length = env->GetArrayLength(jniMergedMat);
        jfloat *oarr = env->GetFloatArrayElements(jniMergedMat, nullptr);

        for (int i = 0; i < length; i++){
            mergedMat.push_back(oarr[i]);
        }
    }


    auto ctcResult = CTCB::runCtc(mergedCopus, mergedMat);


    auto finalLength = ctcResult.charList_.size();

    jintArray result = env->NewIntArray(finalLength);

    jint *narr = env->GetIntArrayElements(result, nullptr);
    for (int i = 0; i < finalLength; i++){
        narr[i] = ctcResult.charList_[i];
    }

    env->ReleaseIntArrayElements(result, narr, 0);

    return result;
}

}

