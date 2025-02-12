/*
 * Copyright 2011, Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <allheaders.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <common.h>
#include <environ.h>
#include <jni.h>
#include <pix.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************
 * ReadFile *
 ************/

jlong Java_com_googlecode_leptonica_android_ReadFile_nativeReadMem(JNIEnv *env, jclass clazz,
                                                                   jbyteArray image, jint length) {
    jbyte *image_buffer = env->GetByteArrayElements(image, NULL);
    int buffer_length = env->GetArrayLength(image);

    PIX *pix = pixReadMem((const l_uint8 *) image_buffer, buffer_length);

    env->ReleaseByteArrayElements(image, image_buffer, JNI_ABORT);

    return (jlong) pix;
}

jlong Java_com_googlecode_leptonica_android_ReadFile_nativeReadBytes8(JNIEnv *env, jclass clazz,
                                                                      jbyteArray data, jint w,
                                                                      jint h) {
    PIX *pix = pixCreateNoInit((l_int32) w, (l_int32) h, 8);
    l_uint8 **lineptrs = pixSetupByteProcessing(pix, NULL, NULL);
    jbyte *data_buffer = env->GetByteArrayElements(data, NULL);
    l_uint8 *byte_buffer = (l_uint8 *) data_buffer;

    for (int i = 0; i < h; i++) {
        memcpy(lineptrs[i], (byte_buffer + (i * w)), w);
    }

    env->ReleaseByteArrayElements(data, data_buffer, JNI_ABORT);
    pixCleanupByteProcessing(pix, lineptrs);

    l_int32 d;

    pixGetDimensions(pix, &w, &h, &d);

    LOGE("Created image width w=%d, h=%d, d=%d", w, h, d);

    return (jlong) pix;
}

jboolean Java_com_googlecode_leptonica_android_ReadFile_nativeReplaceBytes8(JNIEnv *env,
                                                                            jclass clazz,
                                                                            jlong nativePix,
                                                                            jbyteArray data,
                                                                            jint srcw, jint srch) {
    PIX *pix = (PIX *) nativePix;
    l_int32 w, h, d;

    pixGetDimensions(pix, &w, &h, &d);

    if (d != 8 || (l_int32) srcw != w || (l_int32) srch != h) {
        LOGE("Failed to replace bytes at w=%d, h=%d, d=%d with w=%d, h=%d", w, h, d, srcw, srch);

        return JNI_FALSE;
    }

    l_uint8 **lineptrs = pixSetupByteProcessing(pix, NULL, NULL);
    jbyte *data_buffer = env->GetByteArrayElements(data, NULL);
    l_uint8 *byte_buffer = (l_uint8 *) data_buffer;

    for (int i = 0; i < h; i++) {
        memcpy(lineptrs[i], (byte_buffer + (i * w)), w);
    }

    env->ReleaseByteArrayElements(data, data_buffer, JNI_ABORT);
    pixCleanupByteProcessing(pix, lineptrs);

    return JNI_TRUE;
}

jlong Java_com_googlecode_leptonica_android_ReadFile_nativeReadFiles(JNIEnv *env, jclass clazz,
                                                                     jstring dirName, jstring prefix) {
    PIXA *pixad = NULL;

    const char *c_dirName = env->GetStringUTFChars(dirName, NULL);
    if (c_dirName == NULL) {
        LOGE("could not extract dirName string!");
        return JNI_FALSE;
    }

    const char *c_prefix = env->GetStringUTFChars(prefix, NULL);
    if (c_prefix == NULL) {
        LOGE("could not extract prefix string!");
        return JNI_FALSE;
    }

    pixad = pixaReadFiles(c_dirName, c_prefix);

    env->ReleaseStringUTFChars(dirName, c_dirName);
    env->ReleaseStringUTFChars(prefix, c_prefix);

    return (jlong) pixad;
}

jlong Java_com_googlecode_leptonica_android_ReadFile_nativeReadFile(JNIEnv *env, jclass clazz,
                                                                    jstring fileName) {
    PIX *pixd = NULL;

    const char *c_fileName = env->GetStringUTFChars(fileName, NULL);
    if (c_fileName == NULL) {
        LOGE("could not extract fileName string!");
        return JNI_FALSE;
    }

    pixd = pixRead(c_fileName);

    env->ReleaseStringUTFChars(fileName, c_fileName);

    return (jlong) pixd;
}

jlong Java_com_googlecode_leptonica_android_ReadFile_nativeReadBitmap(JNIEnv *env, jclass clazz,
                                                                      jobject bitmap) {
    LOGV(__FUNCTION__);

    l_int32 w, h, d;
    AndroidBitmapInfo info;
    void* pixels;
    int ret;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return JNI_FALSE;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888 !");
        return JNI_FALSE;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return JNI_FALSE;
    }

    PIX *pixd = pixCreate(info.width, info.height, 32);
    l_uint8 *src = (l_uint8 *) pixels;
    l_uint8 *dst = (l_uint8 *) pixGetData(pixd);
    l_int32 srcBpl = (info.stride);
    l_int32 dstBpl = pixGetWpl(pixd)*4;

    for (int dy = 0; dy < info.height; dy++) {
        memcpy(dst, src, 4 * info.width);
        dst += dstBpl;
        src += srcBpl;

    }
    pixEndianByteSwap(pixd);


    AndroidBitmap_unlockPixels(env, bitmap);

    return (jlong) pixd;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */