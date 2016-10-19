/*
 ============================================================================
 Name        : supla_client-jni.c
 Author      : Przemyslaw Zygmunt przemek@supla.org
 Version     : 1.0.7
 Copyright   : GPLv2
 ============================================================================
*/

#include <jni.h>
#include <android/log.h>
#include <string.h>

#include "proto.h"
#include "supla-client.h"

static char log_tag[] = "LibSuplaClient";
//  __android_log_write(ANDROID_LOG_DEBUG, log_tag, "XXXX");

typedef struct {

    void *_supla_client;
    
    jobject j_obj;
    
    jmethodID j_mid_on_versionerror;
    jmethodID j_mid_on_connected;
    jmethodID j_mid_on_connerror;
    jmethodID j_mid_on_disconnected;
    jmethodID j_mid_on_registering;
    jmethodID j_mid_on_registered;
    jmethodID j_mid_on_registererror;
    jmethodID j_mid_location_update;
    jmethodID j_mid_channel_update;
    jmethodID j_mid_channel_value_update;
    jmethodID j_mid_on_event;
    
}TAndroidSuplaClient;

static JavaVM *java_vm;

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    java_vm = vm;
    
    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        __android_log_write(ANDROID_LOG_DEBUG, log_tag, "GetEnv failed.");
        return -1;
    }
    
    return JNI_VERSION_1_6;
};

JNIEnv *supla_client_get_env(TAndroidSuplaClient *asc) {
    
    JNIEnv *env = NULL;
    int getEnvStat;
    
    if ( asc && asc->j_obj ) {
        
        getEnvStat = (*java_vm)->GetEnv(java_vm, (void**) &env, JNI_VERSION_1_6);
        
        if (getEnvStat == JNI_OK) {
            return env;
        }
    }

    
    return NULL;
}

jfieldID supla_client_GetFieldID(JNIEnv *env, jclass c, const char *name, const char *type) {
    jfieldID fid;
    
    
    fid = (*env)->GetFieldID(env, c, name, type);
    
    if ( fid == NULL ) {
        __android_log_print(ANDROID_LOG_ERROR, log_tag, "Unknown field name: %s type: %s", name, type);
    }
    
    return fid;
}

jmethodID supla_client_GetMethodID(JNIEnv *env, jclass c, const char *name, const char *type) {
    jmethodID methodID;
    
    methodID = (*env)->GetMethodID(env, c, name, type);
    
    if ( methodID == NULL ) {
        __android_log_print(ANDROID_LOG_ERROR, log_tag, "Unknown method name: %s type: %s", name, type);
    }
    
    return methodID;
}

void supla_android_client(TAndroidSuplaClient *asc, jmethodID mid, jobject obj) {
    
    JNIEnv *env = supla_client_get_env(asc);
    if ( env ) {
        
        if ( obj )
            (*env)->CallVoidMethod(env, asc->j_obj, mid, obj);
        else
            (*env)->CallVoidMethod(env, asc->j_obj, mid);
    }
    
}

void supla_android_client_cb_on_versionerror(void *_suplaclient, void *user_data, int version, int remote_version_min, int remote_version) {
   
    jfieldID fid;
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    JNIEnv* env = supla_client_get_env(asc);
    
    
    if ( env && asc && asc->j_mid_on_versionerror ) {
        
        jclass cls = (*env)->FindClass(env, "org/supla/android/lib/SuplaVersionError");
        
        jmethodID methodID = supla_client_GetMethodID(env, cls, "<init>", "()V");
        jobject rerr = (*env)->NewObject(env,cls, methodID);
        jclass crerr = (*env)->GetObjectClass(env, rerr);
        
        fid = supla_client_GetFieldID(env, crerr, "Version", "I");
        (*env)->SetIntField(env, rerr, fid, version);
        
        fid = supla_client_GetFieldID(env, crerr, "RemoteVersionMin", "I");
        (*env)->SetIntField(env, rerr, fid, remote_version_min);
        
        fid = supla_client_GetFieldID(env, crerr, "RemoteVersion", "I");
        (*env)->SetIntField(env, rerr, fid, remote_version);
        
        supla_android_client(asc, asc->j_mid_on_versionerror, rerr);
    }
    
}

void supla_android_client_cb_on_connected(void *_suplaclient, void *user_data) {

    
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    
    if ( asc && asc->j_mid_on_connected )
        supla_android_client(asc, asc->j_mid_on_connected, NULL);
    
}

void supla_android_client_cb_on_connerror(void *_suplaclient, void *user_data, int code) {

    jfieldID fid;
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    JNIEnv* env = supla_client_get_env(asc);
   
   
    if ( env && asc && asc->j_mid_on_connerror ) {

         jclass cls = (*env)->FindClass(env, "org/supla/android/lib/SuplaConnError");

         jmethodID methodID = supla_client_GetMethodID(env, cls, "<init>", "()V");
         jobject cerr = (*env)->NewObject(env,cls, methodID);
         jclass ccerr = (*env)->GetObjectClass(env, cerr);

         fid = supla_client_GetFieldID(env, ccerr, "Code", "I");
         (*env)->SetIntField(env, cerr, fid, code);

         supla_android_client(asc, asc->j_mid_on_connerror, cerr);
    }


}


void supla_android_client_cb_on_disconnected(void *_suplaclient, void *user_data) {
    
    
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    
    if ( asc && asc->j_mid_on_disconnected )
        supla_android_client(asc, asc->j_mid_on_disconnected, NULL);
    
}

void supla_android_client_cb_on_registering(void *_suplaclient, void *user_data) {
    
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    
    if ( asc && asc->j_mid_on_registering )
        supla_android_client(asc, asc->j_mid_on_registering, NULL);
    
}

void supla_android_client_cb_on_registered(void *_suplaclient, void *user_data, TSC_SuplaRegisterClientResult *result) {
    
    jfieldID fid;
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    JNIEnv* env = supla_client_get_env(asc);
    
    if ( env && asc && asc->j_mid_on_registered ) {
        
        jclass cls = (*env)->FindClass(env, "org/supla/android/lib/SuplaRegisterResult");
        
        jmethodID methodID = supla_client_GetMethodID(env, cls, "<init>", "()V");
        jobject reg = (*env)->NewObject(env,cls, methodID);
        jclass creg = (*env)->GetObjectClass(env, reg);
        
        fid = supla_client_GetFieldID(env, creg, "ResultCode", "I");
        (*env)->SetIntField(env, reg, fid, result->result_code);
        
        fid = supla_client_GetFieldID(env, creg, "ClientID", "I");
        (*env)->SetIntField(env, reg, fid, result->ClientID);
        
        fid = supla_client_GetFieldID(env, creg, "LocationCount", "I");
        (*env)->SetIntField(env, reg, fid, result->LocationCount);
    
        
        fid = supla_client_GetFieldID(env, creg, "ChannelCount", "I");
        (*env)->SetIntField(env, reg, fid, result->ChannelCount);
        
        fid = supla_client_GetFieldID(env, creg, "ActivityTimeout", "I");
        (*env)->SetIntField(env, reg, fid, result->activity_timeout);
        
        fid = supla_client_GetFieldID(env, creg, "Version", "I");
        (*env)->SetIntField(env, reg, fid, result->version);
        
        fid = supla_client_GetFieldID(env, creg, "VersionMin", "I");
        (*env)->SetIntField(env, reg, fid, result->version_min);
        
        supla_android_client(asc, asc->j_mid_on_registered, reg);
    }
    
}

void supla_android_client_cb_on_registererror(void *_suplaclient, void *user_data, int code) {
    
    jfieldID fid;
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    JNIEnv* env = supla_client_get_env(asc);
    
    
    if ( env && asc && asc->j_mid_on_registererror ) {

         jclass cls = (*env)->FindClass(env, "org/supla/android/lib/SuplaRegisterError");

         jmethodID methodID = supla_client_GetMethodID(env, cls, "<init>", "()V");
         jobject rerr = (*env)->NewObject(env,cls, methodID);
         jclass crerr = (*env)->GetObjectClass(env, rerr);
        
         fid = supla_client_GetFieldID(env, crerr, "ResultCode", "I");
         (*env)->SetIntField(env, rerr, fid, code);
        
         supla_android_client(asc, asc->j_mid_on_registererror, rerr);
    }
    
}

void supla_android_client_cb_location_update(void *_suplaclient, void *user_data, TSC_SuplaLocation *location) {
    
    jfieldID fid;
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    JNIEnv* env = supla_client_get_env(asc);
    
    if ( env && asc && asc->j_mid_location_update ) {
        
        jclass cls = (*env)->FindClass(env, "org/supla/android/lib/SuplaLocation");
        
        jmethodID methodID = supla_client_GetMethodID(env, cls, "<init>", "()V");
        jobject loc = (*env)->NewObject(env,cls, methodID);
        jclass cloc = (*env)->GetObjectClass(env, loc);
        
        fid = supla_client_GetFieldID(env, cloc, "EOL", "Z");
        (*env)->SetBooleanField(env, loc, fid, location->EOL == 1 ? JNI_TRUE : JNI_FALSE);
        
        fid = supla_client_GetFieldID(env, cloc, "Id", "I");
        (*env)->SetIntField(env, loc, fid, location->Id);
        
        fid = supla_client_GetFieldID(env, cloc, "Caption", "Ljava/lang/String;");
        (*env)->SetObjectField(env, loc, fid, (*env)->NewStringUTF(env, location->Caption));
        
        supla_android_client(asc, asc->j_mid_location_update, loc);
    }
    
}

jobject supla_android_client_channelvalue_to_jobject(void *_suplaclient, void *user_data, TSuplaChannelValue *channel_value) {
  
    jfieldID fid;
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    JNIEnv* env = supla_client_get_env(asc);
  
    if ( env && asc ) {
        
        jclass cls = (*env)->FindClass(env, "org/supla/android/lib/SuplaChannelValue");
        
        jmethodID methodID = supla_client_GetMethodID(env, cls, "<init>", "()V");
        jobject val = (*env)->NewObject(env,cls, methodID);
        jclass cval = (*env)->GetObjectClass(env, val);
        
        jbyteArray arr = (*env)->NewByteArray(env, SUPLA_CHANNELVALUE_SIZE);
        (*env)->SetByteArrayRegion (env, arr, 0, SUPLA_CHANNELVALUE_SIZE, channel_value->value);
        
        fid = supla_client_GetFieldID(env, cval, "Value", "[B");
        (*env)->SetObjectField(env, val, fid, arr);
        
        
        arr = (*env)->NewByteArray(env, SUPLA_CHANNELVALUE_SIZE);
        (*env)->SetByteArrayRegion (env, arr, 0, SUPLA_CHANNELVALUE_SIZE, channel_value->sub_value);
        
        fid = supla_client_GetFieldID(env, cval, "SubValue", "[B");
        (*env)->SetObjectField(env, val, fid, arr);
        

        return val;
    }
    
    return NULL;
}

void supla_android_client_cb_channel_update(void *_suplaclient, void *user_data, TSC_SuplaChannel *channel) {
   
    //int a; 
    jfieldID fid;
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    JNIEnv* env = supla_client_get_env(asc);
   
    //for(a=0;a<SUPLA_CHANNELVALUE_SIZE;a++)
    // __android_log_print(ANDROID_LOG_ERROR, log_tag, "Channel %i, Value[%i]=%i", channel->Id, a, channel->value.value[a]);    
 
    if ( env && asc && asc->j_mid_channel_update ) {
        
        jclass cls = (*env)->FindClass(env, "org/supla/android/lib/SuplaChannel");
        
        jmethodID methodID = supla_client_GetMethodID(env, cls, "<init>", "()V");
        jobject ch = (*env)->NewObject(env,cls, methodID);
        jclass cch = (*env)->GetObjectClass(env, ch);
        
        fid = supla_client_GetFieldID(env, cch, "EOL", "Z");
        (*env)->SetBooleanField(env, ch, fid, channel->EOL == 1 ? JNI_TRUE : JNI_FALSE);
        
        fid = supla_client_GetFieldID(env, cch, "Id", "I");
        (*env)->SetIntField(env, ch, fid, channel->Id);
        
        fid = supla_client_GetFieldID(env, cch, "LocationID", "I");
        (*env)->SetIntField(env, ch, fid, channel->LocationID);
        
        fid = supla_client_GetFieldID(env, cch, "Func", "I");
        (*env)->SetIntField(env, ch, fid, channel->Func);
        
        fid = supla_client_GetFieldID(env, cch, "OnLine", "Z");
        (*env)->SetBooleanField(env, ch, fid, channel->online == 1 ? JNI_TRUE : JNI_FALSE);
        
        fid = supla_client_GetFieldID(env, cch, "Value", "Lorg/supla/android/lib/SuplaChannelValue;");
        jobject chv = supla_android_client_channelvalue_to_jobject(_suplaclient, user_data, &channel->value);
        (*env)->SetObjectField(env, ch, fid, chv);
        
        fid = supla_client_GetFieldID(env, cch, "Caption", "Ljava/lang/String;");
        (*env)->SetObjectField(env, ch, fid, (*env)->NewStringUTF(env, channel->Caption));
        
        supla_android_client(asc, asc->j_mid_channel_update, ch);
    }
    
}

void supla_android_client_cb_channel_value_update(void *_suplaclient, void *user_data, TSC_SuplaChannelValue *channel_value) {
    
    jfieldID fid;
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    JNIEnv* env = supla_client_get_env(asc);
    
    if ( env && asc && asc->j_mid_channel_value_update ) {
        
        jclass cls = (*env)->FindClass(env, "org/supla/android/lib/SuplaChannelValueUpdate");
        
        jmethodID methodID = supla_client_GetMethodID(env, cls, "<init>", "()V");
        jobject val = (*env)->NewObject(env,cls, methodID);
        jclass cval = (*env)->GetObjectClass(env, val);
        
        fid = supla_client_GetFieldID(env, cval, "EOL", "Z");
        (*env)->SetBooleanField(env, val, fid, channel_value->EOL == 1 ? JNI_TRUE : JNI_FALSE);
        
        fid = supla_client_GetFieldID(env, cval, "Id", "I");
        (*env)->SetIntField(env, val, fid, channel_value->Id);
        
        fid = supla_client_GetFieldID(env, cval, "OnLine", "Z");
        (*env)->SetBooleanField(env, val, fid, channel_value->online == 1 ? JNI_TRUE : JNI_FALSE);
        
        fid = supla_client_GetFieldID(env, cval, "Value", "Lorg/supla/android/lib/SuplaChannelValue;");
        jobject chv = supla_android_client_channelvalue_to_jobject(_suplaclient, user_data, &channel_value->value);
        (*env)->SetObjectField(env, val, fid, chv);
        
        
        supla_android_client(asc, asc->j_mid_channel_value_update, val);

    }
    
}

void supla_android_client_cb_on_event(void *_suplaclient, void *user_data, TSC_SuplaEvent *event) {
    
    jfieldID fid;
    TAndroidSuplaClient *asc = (TAndroidSuplaClient*)user_data;
    JNIEnv* env = supla_client_get_env(asc);
    
    if ( env && asc && asc->j_mid_on_event ) {
        
        jclass cls = (*env)->FindClass(env, "org/supla/android/lib/SuplaEvent");
        
        jmethodID methodID = supla_client_GetMethodID(env, cls, "<init>", "()V");
        jobject ev = (*env)->NewObject(env, cls, methodID);
        jclass cev = (*env)->GetObjectClass(env, ev);
        
        fid = supla_client_GetFieldID(env, cev, "Event", "I");
        (*env)->SetIntField(env, ev, fid, event->Event);
        
        fid = supla_client_GetFieldID(env, cev, "ChannelID", "I");
        (*env)->SetIntField(env, ev, fid, event->ChannelID);
    
        fid = supla_client_GetFieldID(env, cev, "DurationMS", "J");
        (*env)->SetLongField(env, ev, fid, event->DurationMS);

        fid = supla_client_GetFieldID(env, cev, "SenderID", "I");
        (*env)->SetIntField(env, ev, fid, event->SenderID);
        
        fid = supla_client_GetFieldID(env, cev, "SenderName", "Ljava/lang/String;");
        (*env)->SetObjectField(env, ev, fid, (*env)->NewStringUTF(env, event->SenderName));
        
        supla_android_client(asc, asc->j_mid_on_event, ev);
        
    }
    
}



void supla_android_client_jstring_to_buffer(JNIEnv* env, jobject cfg, jclass jcs, const char *name, char *buff, int size) {
    
    jfieldID fid = supla_client_GetFieldID(env, jcs, name, "Ljava/lang/String;");
    jstring js_str = (*env)->GetObjectField(env, cfg, fid);
    const char *str = (*env)->GetStringUTFChars(env, js_str, 0);
    
    if ( str ) {
        memcpy(buff, str, strlen(str) >= size ? size-1 : strlen(str));
        (*env)->ReleaseStringUTFChars(env, js_str, str);
    }
}

void* supla_client_ptr(jlong _asc) {
    
#ifdef _LP64
    TAndroidSuplaClient *asc = (void*)_asc;
#else
    TAndroidSuplaClient *asc = (void*)(int)_asc;
#endif
    
    if ( asc )
        return asc->_supla_client;
    
    return NULL;
}

JNIEXPORT void JNICALL
Java_org_supla_android_lib_SuplaClient_CfgInit(JNIEnv* env, jobject thiz, jobject cfg) {

    TSuplaClientCfg sclient_cfg;
    jfieldID fid;
    
    jclass jcs = (*env)->GetObjectClass(env, cfg);
    
    if ( jcs ) {
        
        supla_client_cfginit(&sclient_cfg);
        
        fid = supla_client_GetFieldID(env, jcs, "ssl_enabled", "Z");
        (*env)->SetBooleanField(env, cfg, fid, sclient_cfg.ssl_enabled == 1 ? JNI_TRUE : JNI_FALSE);
        
        fid = supla_client_GetFieldID(env, jcs, "ssl_port", "I");
        (*env)->SetIntField(env, cfg, fid, sclient_cfg.ssl_port);
        
        fid = supla_client_GetFieldID(env, jcs, "tcp_port", "I");
        (*env)->SetIntField(env, cfg, fid, sclient_cfg.tcp_port);
        
        if ( sclient_cfg.host == NULL )
            sclient_cfg.host = "";
        
        fid = supla_client_GetFieldID(env, jcs, "Host", "Ljava/lang/String;");
        (*env)->SetObjectField(env, cfg, fid, (*env)->NewStringUTF(env, sclient_cfg.host));
        
        fid = supla_client_GetFieldID(env, jcs, "SoftVer", "Ljava/lang/String;");
        (*env)->SetObjectField(env, cfg, fid, (*env)->NewStringUTF(env, sclient_cfg.SoftVer));
        
        fid = supla_client_GetFieldID(env, jcs, "AccessIDpwd", "Ljava/lang/String;");
        (*env)->SetObjectField(env, cfg, fid, (*env)->NewStringUTF(env, sclient_cfg.AccessIDpwd));
        
        fid = supla_client_GetFieldID(env, jcs, "AccessID", "I");
        (*env)->SetIntField(env, cfg, fid, sclient_cfg.AccessID);
        
        fid = supla_client_GetFieldID(env, jcs, "Name", "Ljava/lang/String;");
        (*env)->SetObjectField(env, cfg, fid, (*env)->NewStringUTF(env, sclient_cfg.Name));
        
        jbyteArray arr = (*env)->NewByteArray(env, SUPLA_GUID_SIZE);
        (*env)->SetByteArrayRegion (env, arr, 0, SUPLA_GUID_SIZE, sclient_cfg.clientGUID);
        
        fid = supla_client_GetFieldID(env, jcs, "clientGUID", "[B");
        (*env)->SetObjectField(env, cfg, fid, arr);
        
    };
    
    
}

JNIEXPORT jlong JNICALL
Java_org_supla_android_lib_SuplaClient_scInit(JNIEnv* env, jobject thiz, jobject cfg) {


    TSuplaClientCfg sclient_cfg;
    jfieldID fid;
    
    jclass jcs = (*env)->GetObjectClass(env, cfg);
    
    if ( jcs ) {
        supla_client_cfginit(&sclient_cfg);
        
        fid = supla_client_GetFieldID(env, jcs, "ssl_enabled", "Z");
        sclient_cfg.ssl_enabled = (*env)->GetBooleanField(env, cfg, fid) == JNI_TRUE ? 1 : 0;
        
        fid = supla_client_GetFieldID(env, jcs, "ssl_port", "I");
        sclient_cfg.ssl_port = (*env)->GetIntField(env, cfg, fid);
        
        fid = supla_client_GetFieldID(env, jcs, "tcp_port", "I");
        sclient_cfg.tcp_port = (*env)->GetIntField(env, cfg, fid);
        
        fid = supla_client_GetFieldID(env, jcs, "Host", "Ljava/lang/String;");
        jstring js_host = (*env)->GetObjectField(env, cfg, fid);
        sclient_cfg.host = (char *)(*env)->GetStringUTFChars(env, js_host, 0);
        
        supla_android_client_jstring_to_buffer(env, cfg, jcs, "SoftVer", sclient_cfg.SoftVer, SUPLA_SOFTVER_MAXSIZE);
        
        supla_android_client_jstring_to_buffer(env, cfg, jcs, "AccessIDpwd", sclient_cfg.AccessIDpwd, SUPLA_ACCESSID_PWD_MAXSIZE);
        
        fid = supla_client_GetFieldID(env, jcs, "AccessID", "I");
        sclient_cfg.AccessID = (*env)->GetIntField(env, cfg, fid);
        
        supla_android_client_jstring_to_buffer(env, cfg, jcs, "Name", sclient_cfg.Name, SUPLA_CLIENT_NAME_MAXSIZE);
        
        fid = supla_client_GetFieldID(env, jcs, "clientGUID", "[B");
        jbyteArray barr = (*env)->GetObjectField(env, cfg, fid);
        
        if ( SUPLA_GUID_SIZE == (*env)->GetArrayLength(env, barr) ) {
            
            jbyte *data = (jbyte *)(*env)->GetByteArrayElements(env, barr, NULL);
            if ( data ) {
                memcpy(sclient_cfg.clientGUID, data, SUPLA_GUID_SIZE);
                (*env)->ReleaseByteArrayElements(env, barr, data, 0 );
            }
        }
        
        TAndroidSuplaClient *_asc = malloc(sizeof(TAndroidSuplaClient));
        memset(_asc, 0, sizeof(TAndroidSuplaClient));
        
        _asc->j_obj = (*env)->NewGlobalRef(env, thiz);
        
        jclass oclass = (*env)->GetObjectClass(env, thiz);
        
        _asc->j_mid_on_versionerror = supla_client_GetMethodID(env, oclass, "onVersionError", "(Lorg/supla/android/lib/SuplaVersionError;)V");
        _asc->j_mid_on_connected = supla_client_GetMethodID(env, oclass, "onConnected", "()V");
        _asc->j_mid_on_connerror = supla_client_GetMethodID(env, oclass, "onConnError", "(Lorg/supla/android/lib/SuplaConnError;)V");
        _asc->j_mid_on_disconnected = supla_client_GetMethodID(env, oclass, "onDisconnected", "()V");
        _asc->j_mid_on_registering = supla_client_GetMethodID(env, oclass, "onRegistering", "()V");
        _asc->j_mid_on_registered = supla_client_GetMethodID(env, oclass, "onRegistered", "(Lorg/supla/android/lib/SuplaRegisterResult;)V");
        _asc->j_mid_on_registererror = supla_client_GetMethodID(env, oclass, "onRegisterError", "(Lorg/supla/android/lib/SuplaRegisterError;)V");
        _asc->j_mid_location_update = supla_client_GetMethodID(env, oclass, "LocationUpdate", "(Lorg/supla/android/lib/SuplaLocation;)V");
        _asc->j_mid_channel_update = supla_client_GetMethodID(env, oclass, "ChannelUpdate", "(Lorg/supla/android/lib/SuplaChannel;)V");
        _asc->j_mid_channel_value_update = supla_client_GetMethodID(env, oclass, "ChannelValueUpdate", "(Lorg/supla/android/lib/SuplaChannelValueUpdate;)V");
        _asc->j_mid_on_event = supla_client_GetMethodID(env, oclass, "onEvent", "(Lorg/supla/android/lib/SuplaEvent;)V");
        
    
        sclient_cfg.user_data = _asc;
        sclient_cfg.cb_on_versionerror = supla_android_client_cb_on_versionerror;
        sclient_cfg.cb_on_connected = supla_android_client_cb_on_connected;
        sclient_cfg.cb_on_connerror = supla_android_client_cb_on_connerror;
        sclient_cfg.cb_on_disconnected = supla_android_client_cb_on_disconnected;
        sclient_cfg.cb_on_registering = supla_android_client_cb_on_registering;
        sclient_cfg.cb_on_registered = supla_android_client_cb_on_registered;
        sclient_cfg.cb_on_registererror = supla_android_client_cb_on_registererror;
        sclient_cfg.cb_location_update = supla_android_client_cb_location_update;
        sclient_cfg.cb_channel_update = supla_android_client_cb_channel_update;
        sclient_cfg.cb_channel_value_update = supla_android_client_cb_channel_value_update;
        sclient_cfg.cb_on_event = supla_android_client_cb_on_event;
        
        _asc->_supla_client = supla_client_init(&sclient_cfg);
        
        if ( sclient_cfg.host ) {
            (*env)->ReleaseStringUTFChars(env, js_host, sclient_cfg.host);
        }
        
        if ( _asc->_supla_client ) {

            #ifdef _LP64
            return (jlong)_asc;
            #else
            return (int)_asc;
            #endif
            
        } else {
            free(_asc);
        }
    }

    return 0;
}

JNIEXPORT void JNICALL
Java_org_supla_android_lib_SuplaClient_scFree(JNIEnv* env, jobject thiz, jlong _asc) {
    
#ifdef _LP64
    TAndroidSuplaClient *asc = (void*)_asc;
#else
    TAndroidSuplaClient *asc = (void*)(int)_asc;
#endif
    
    if ( asc ) {
        
        if ( asc->_supla_client ) {
            supla_client_free(asc->_supla_client);
            asc->_supla_client = NULL;
        }
        
        if ( asc->j_obj ) {
            (*env)->DeleteGlobalRef(env, asc->j_obj);
            asc->j_obj = NULL;
        }
    }
    
};

JNIEXPORT jint JNICALL
Java_org_supla_android_lib_SuplaClient_scGetId(JNIEnv* env, jobject thiz, jlong _asc) {
  
    void *supla_client = supla_client_ptr(_asc);
    
    if ( supla_client )
        return supla_client_get_id(supla_client);
    
    return 0;
};


JNIEXPORT jboolean JNICALL
Java_org_supla_android_lib_SuplaClient_scConnect(JNIEnv* env, jobject thiz, jlong _asc) {
    
    void *supla_client = supla_client_ptr(_asc);
    
    if ( supla_client )
        return supla_client_connect(supla_client) == 1 ? JNI_TRUE : JNI_FALSE;
    
    return JNI_FALSE;
};

JNIEXPORT jboolean JNICALL
Java_org_supla_android_lib_SuplaClient_scConnected(JNIEnv* env, jobject thiz, jlong _asc) {
    
    void *supla_client = supla_client_ptr(_asc);
    
    if ( supla_client )
        return supla_client_connected(supla_client) == 1 ? JNI_TRUE : JNI_FALSE;
    
    return JNI_FALSE;
};

JNIEXPORT jboolean JNICALL
Java_org_supla_android_lib_SuplaClient_scRegistered(JNIEnv* env, jobject thiz, jlong _asc) {
    
    void *supla_client = supla_client_ptr(_asc);
    
    if ( supla_client )
        return supla_client_registered(supla_client) == 1 ? JNI_TRUE : JNI_FALSE;
    
    return JNI_FALSE;
};

JNIEXPORT void JNICALL
Java_org_supla_android_lib_SuplaClient_scDisconnect(JNIEnv* env, jobject thiz, jlong _asc) {
    
    void *supla_client = supla_client_ptr(_asc);
    
    if ( supla_client )
        supla_client_disconnect(supla_client);
    
};

JNIEXPORT jboolean JNICALL
Java_org_supla_android_lib_SuplaClient_scIterate(JNIEnv* env, jobject thiz, jlong _asc, jint wait_usec) {
    
    void *supla_client = supla_client_ptr(_asc);

    return supla_client && supla_client_iterate(supla_client, wait_usec) == 1 ? JNI_TRUE : JNI_FALSE;
};

JNIEXPORT jboolean JNICALL
Java_org_supla_android_lib_SuplaClient_scOpen(JNIEnv* env, jobject thiz, jlong _asc, jint channelid, jint open) {
    
    void *supla_client = supla_client_ptr(_asc);
    
    if ( supla_client )
        return supla_client_open(supla_client, channelid, open) == 1 ? JNI_TRUE : JNI_FALSE;
    
    return JNI_FALSE;
};

JNIEXPORT jboolean JNICALL
Java_org_supla_android_lib_SuplaClient_scSetRGBW(JNIEnv* env, jobject thiz, jlong _asc, jint channelid, jint color, int color_brightness, int brightness) {

    void *supla_client = supla_client_ptr(_asc);

    if ( supla_client ) {

        if ( color_brightness > 255 
             || color_brightness < 0 )
          color_brightness = 0;

        if ( brightness > 255
             || brightness < 0 )
          brightness = 0; 
  
        return supla_client_set_rgbw(supla_client, channelid, color, color_brightness, brightness) == 1 ? JNI_TRUE : JNI_FALSE;
    }

    return JNI_FALSE;
};
