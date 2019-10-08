#include "network_loki_lokinet_LokinetDaemon.h"
#include "lokinet_jni_common.hpp"
#include "lokinet_jni_vpnio.hpp"
#include <llarp.h>

extern "C"
{
  JNIEXPORT jobject JNICALL
  Java_network_loki_lokinet_LokinetDaemon_Obtain(JNIEnv *env, jclass)
  {
    llarp_main *ptr = llarp_main_default_init();
    if(ptr == nullptr)
      return nullptr;
    return env->NewDirectByteBuffer(ptr, llarp_main_size());
  }

  JNIEXPORT void JNICALL
  Java_network_loki_lokinet_LokinetDaemon_Free(JNIEnv *env, jclass, jobject buf)
  {
    llarp_main *ptr = FromBuffer< llarp_main >(env, buf);
    llarp_main_free(ptr);
  }

  JNIEXPORT jboolean JNICALL
  Java_network_loki_lokinet_LokinetDaemon_Configure(JNIEnv *env, jobject self,
                                                    jobject conf)
  {
    llarp_main *ptr      = FromObjectMember< llarp_main >(env, self, "impl");
    llarp_config *config = FromObjectMember< llarp_config >(env, conf, "impl");
    if(ptr == nullptr || config == nullptr)
      return JNI_FALSE;
    if(llarp_main_configure(ptr, config))
      return JNI_TRUE;
    return JNI_FALSE;
  }

  JNIEXPORT jint JNICALL
  Java_network_loki_lokinet_LokinetDaemon_Mainloop(JNIEnv *env, jobject self)
  {
    static llarp_main_runtime_opts opts;
    llarp_main *ptr = FromObjectMember< llarp_main >(env, self, "impl");
    if(ptr == nullptr)
      return -1;
    return llarp_main_run(ptr, opts);
  }

  JNIEXPORT jboolean JNICALL
  Java_network_loki_lokinet_LokinetDaemon_IsRunning(JNIEnv *env, jobject self)
  {
    llarp_main *ptr = FromObjectMember< llarp_main >(env, self, "impl");
    return (ptr != nullptr && llarp_main_is_running(ptr)) ? JNI_TRUE
                                                          : JNI_FALSE;
  }

  JNIEXPORT jboolean JNICALL
  Java_network_loki_lokinet_LokinetDaemon_Stop(JNIEnv *env, jobject self)
  {
    llarp_main *ptr = FromObjectMember< llarp_main >(env, self, "impl");
    if(ptr == nullptr)
      return JNI_FALSE;
    if(not llarp_main_is_running(ptr))
      return JNI_FALSE;
    llarp_main_stop(ptr);
    return llarp_main_is_running(ptr) ? JNI_FALSE : JNI_TRUE;
  }

  JNIEXPORT jboolean JNICALL
  Java_network_loki_lokinet_LokinetDaemon_InjectVPN(JNIEnv *env, jobject self,
                                                    jobject vpn)
  {
    llarp_main *ptr = FromObjectMember< llarp_main >(env, self, "impl");
    lokinet_jni_vpnio *impl =
        FromObjectMember< lokinet_jni_vpnio >(env, vpn, "impl");
    if(ptr == nullptr || impl == nullptr)
      return JNI_FALSE;
    if(not impl->Init(ptr))
      return JNI_FASE;
    return llarp_main_inject_default_vpn(ptr, &impl->io, impl->info)
        ? JNI_TRUE
        : JNI_FALSE;
  }
}