/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef ANDROID_LOG_H
#define ANDROID_LOG_H

#include <android/log.h>

#define DEBUG 1

#if DEBUG
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,\
	"LOG_TAG_V", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,\
	"LOG_TAG_D", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,\
	"LOG_TAG_I", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,\
	"LOG_TAG_W", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,\
	"LOG_TAG_E", __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,\
	"LOG_TAG_F", __VA_ARGS__)
#else
#define LOGV(...) do {} while(0)
#define LOGD(...) do {} while(0)
#define LOGI(...) do {} while(0)
#define LOGW(...) do {} while(0)
#define LOGE(...) do {} while(0)
#define LOGF(...) do {} while(0)
#endif
#endif
