//
// Created by F1 on 4/15/2016.
//

#include "SL_Utils.hpp"

//Code not utilized for getting native device sample rate, buffer size, and frame count
// (not used because latency is not a critical issue to this project, we can deal with having sample rate conversion latency)
//	app->activity->instance->getSystemService(Context.AUDIO_SERVICE);
//	jclass audioSystem = env->FindClass("android/media/AudioSystem");
//	jmethodID method = env->GetStaticMethodID(audioSystem, "getPrimaryOutputSamplingRate","()I");
//	jint sample_rate = env->CallStaticIntMethod(audioSystem, method);
//	method = env->GetStaticMethodID(audioSystem,"getPrimaryOutputFrameCount","()I");
//	jint buffer_length = env->CallStaticIntMethod(audioSystem, method);
