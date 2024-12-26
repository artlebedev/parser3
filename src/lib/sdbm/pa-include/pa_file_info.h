/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PA_FILE_INFO_H
#define PA_FILE_INFO_H

#include "pa_apr.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * @file pa_file_info.h
 * @brief APR File handling
 */

/**
 * Structure for determining file permissions.
 * @defvar pa_fileperms_t
 */
typedef pa_int32_t               pa_fileperms_t;

/**
 * @defgroup PA_File_Info Stat Functions
 * @{
 */
#define PA_FINFO_SIZE   0x00000100 /**< Size of the file */

/**
 * The file information structure.  This is analogous to the POSIX
 * stat structure.
 */
typedef struct pa_finfo_t        pa_finfo_t;

struct pa_finfo_t {
    /** The size of the file */
    pa_off_t size;
};

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* ! PA_FILE_INFO_H */
