//
// Created by F1 on 7/30/2016.
//

#ifndef ENGINE_JNI_INTERFACE_HPP
#define ENGINE_JNI_INTERFACE_HPP

#include "common.hpp"

//This class is a holder for all of the JNIEnv variables
class JNI_Interface
{
public:
	ANativeActivity* activity;
	JavaVM* java_vm;

	//The following variables are set as global references
	jclass activity_class;
	jobject activity_instance;

	//Method references (these do not need to be stored as global references)
	jmethodID test_method;

	jmethodID show_ad_method;
	jmethodID hide_ad_method;

	JNI_Interface(ANativeActivity* act)
	{
		activity = act;
		java_vm = activity->vm;

		JNIEnv* jni;

		bool attached_jni_thread = get_jni_env(&jni);

		jclass local_activity_class = jni->GetObjectClass(activity->clazz);
		activity_class = (jclass) jni->NewGlobalRef(local_activity_class);

		if(!activity_class)
		{
			LOGE("Error: Failed to get native activity class");
		}
		else
			LOGI("Found native activity class");


		test_method = jni->GetMethodID(activity_class,"test","(II)Z");

		show_ad_method = jni->GetMethodID(activity_class,"showAd","()V");
		hide_ad_method = jni->GetMethodID(activity_class,"hideAd","()V");

		jobject local_activity_instance = activity->clazz;
		activity_instance = jni->NewGlobalRef(local_activity_instance);

		if(attached_jni_thread)
		{
			detach_thread();
		}
	}

	~JNI_Interface()
	{
		JNIEnv* jni;

		bool attached_jni_thread = get_jni_env(&jni);

		jni->DeleteGlobalRef(activity_instance);
		jni->DeleteGlobalRef(activity_class);

		if(attached_jni_thread)
		{
			detach_thread();
		}
	}

	bool get_jni_env(JNIEnv** env)
	{
		*env = NULL;
		jint result = (*java_vm).GetEnv((void**) env,JNI_VERSION_1_6);
		if(result == JNI_EDETACHED)
		{
			return ((*java_vm).AttachCurrentThread(env,NULL) == JNI_OK);
		}
		else if(result == JNI_EVERSION)
		{
			LOGE("Error: Failed to invoke JNI instance. Unsupported JNI version.");
		}
		return false;
	}

	bool detach_thread()
	{
		java_vm->DetachCurrentThread();
	}

	//Shows ads
	void show_ad()
	{
		JNIEnv* jni;

		bool attached_jni_thread = get_jni_env(&jni);
		jni->CallVoidMethod(activity_instance,show_ad_method);
		if(attached_jni_thread)
		{
			detach_thread();
		}
	}
	//Hides ads
	void hide_ad()
	{
		JNIEnv* jni;

		bool attached_jni_thread = get_jni_env(&jni);
		jni->CallVoidMethod(activity_instance,hide_ad_method);
		if(attached_jni_thread)
		{
			detach_thread();
		}
	}

	//Method which calls the test java method
	void test_function()
	{
		JNIEnv* jni;

		bool attached_jni_thread = get_jni_env(&jni);

		jvalue java_args[2];
		java_args[0].i = 2;
		java_args[1].i = 3;

		jboolean result = jni->CallBooleanMethodA(activity_instance,test_method,java_args);
		LOGE("2 == 3? %d",result);

		java_args[0].i = 49;
		java_args[1].i = 49;

		result = jni->CallBooleanMethodA(activity_instance,test_method,java_args);
		LOGE("49 == 49? %d",result);

		if(attached_jni_thread)
		{
			detach_thread();
		}
	}




	//TODO: if we attached the thread, detach it



};

#endif //ENGINE_JNI_INTERFACE_HPP