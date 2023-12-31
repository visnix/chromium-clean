// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "android_webview/lib/webview_jni_onload.h"
#include "base/android/base_jni_onload.h"
#include "base/android/jni_android.h"
#include "base/android/library_loader/library_loader_hooks.h"

#if defined(WEBVIEW_INCLUDES_WEBLAYER)
#include "weblayer/app/jni_onload.h"
#endif

namespace {

bool NativeInit(base::android::LibraryProcessType library_process_type) {
  switch (library_process_type) {
    case base::android::PROCESS_WEBVIEW:
    case base::android::PROCESS_WEBVIEW_CHILD:

    // TODO(crbug.com/1230005): Remove these once we stop setting these two
    // process types from tests.
    case base::android::PROCESS_CHILD:
    case base::android::PROCESS_BROWSER:
      return android_webview::OnJNIOnLoadInit();

    case base::android::PROCESS_WEBVIEW_NONEMBEDDED:
      return base::android::OnJNIOnLoadInit();

#if defined(WEBVIEW_INCLUDES_WEBLAYER)
    case base::android::PROCESS_WEBLAYER:
    case base::android::PROCESS_WEBLAYER_CHILD:
      return weblayer::OnJNIOnLoadInit();
#endif

    default:
      NOTREACHED();
      return false;
  }
}

}  // namespace

// This is called by the VM when the shared library is first loaded.
// Most of the initialization is done in LibraryLoadedOnMainThread(), not here.
JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  base::android::InitVM(vm);
  base::android::SetNativeInitializationHook(&NativeInit);
  return JNI_VERSION_1_4;
}
