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

	//FIXME: not sure if we can safely store global references to all of this java data
	jclass activity_class;
	jobject activity_instance;

	//Method references
	jmethodID test_method;

	JNI_Interface(ANativeActivity* act)
	{
		activity = act;
		java_vm = activity->vm;

		JNIEnv* jni;

		bool attached_jni_thread = get_jni_env(&jni);

		activity_class = jni->GetObjectClass(activity->clazz);

		if(!activity_class)
		{
			LOGE("Failed to get native activity class");
		}
		else
			LOGE("Found native activity class");


		test_method = jni->GetMethodID(activity_class,"test","(II)Z");

		activity_instance = activity->clazz;
		//FIXME: should we use the following?
		//	if so, uncomment the code to delet ethe global ref in the destructor
		//activity_instance = jni->NewGlobalRef(activity->clazz);

		if(attached_jni_thread)
		{
			detach_thread();
		}
	}

	~JNI_Interface()
	{
		//Uncomment if we decide to use global refs
		//jni->DeleteGlobalRef(activity_instance);
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

//============================================================================
//=========================== Sample JNI init code ===========================
/*
	LOGE("Starting Android Java Code");

	ANativeActivity* activity = app->activity;

	JavaVM* vm = activity->vm;
	JNIEnv* jni;

	(*vm).AttachCurrentThread(&jni,NULL);
	JNIEnv* env = jni;

	jclass activityClass = jni->GetObjectClass(activity->clazz);

	if(!activityClass)
	{
		LOGE("Failed to get native activity class");
	}
	else
		LOGE("Found native activity class");

	jmethodID method = jni->GetMethodID(activityClass,"test","(II)Z");

	jobject instance = activity->clazz;


	jvalue java_args[2];
	java_args[0].i = 2;
	java_args[1].i = 3;

	jboolean result = env->CallBooleanMethodA(instance,method,java_args);
	LOGE("2 == 3? %d",result);

	java_args[0].i = 49;
	java_args[1].i = 49;

	result = env->CallBooleanMethodA(instance,method,java_args);
	LOGE("49 == 49? %d",result);
 */
//============================================================================
//============================================================================
