//
// Created by F1 on 3/21/2016.
//

#ifndef ENGINE_COMMON_H
#define ENGINE_COMMON_H

#define DEBUG_MODE


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity",__VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity",__VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity",__VA_ARGS__))

extern "C" {
#include "native_app_glue/android_native_app_glue.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <jni.h>
#include <android/native_activity.h>
#include <android/configuration.h>
#include <pthread.h>
#include <android/sensor.h>
#include <android/log.h>
#include <string.h>
#include <android/asset_manager.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
//#include <SLES/OpenSLES_AndroidConfiguration.h>

//#include <errno.h>
//#include <unistd.h>
}

//This is the internal data value for opengl to recognize ETC1 compressed textures
#define ETC1_RGB8 0x8D64

#define SBC_AUDIO_OUT_CHANNELS 8
#define SBC_AUDIO_OUT_BUFFER_SIZE 256
#define SBC_AUDIO_OUT_SAMPLE_RATE 11025

#include "Engine.h"
#include "File_Utils.h"
#include "GL_Utils.h"
#include "SL_Utils.h"
#include "Misc.h"
#include "math/Math.h"

#endif //ENGINE_COMMON_H
