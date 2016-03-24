//
// Created by F1 on 3/21/2016.
//

#ifndef ENGINE_COMMON_H
#define ENGINE_COMMON_H


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity",__VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity",__VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity",__VA_ARGS__))

extern "C" {
#include "native_app_glue/android_native_app_glue.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <jni.h>
#include <android/native_activity.h>
#include <android/configuration.h>
#include <pthread.h>
#include <android/sensor.h>
#include <android/log.h>
#include <string.h>
#include <android/asset_manager.h>
//#include <errno.h>
//#include <unistd.h>
}

//#include "Engine.h"
//#include "File_Utils.h"
//#include "GL_Utils.h"

#endif //ENGINE_COMMON_H
