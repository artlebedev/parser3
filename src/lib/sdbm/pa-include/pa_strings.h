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

#ifndef PA_STRINGS_H
#define PA_STRINGS_H

#include "pa_apr.h"
#include "pa_errno.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Concatenate multiple strings, allocating memory out a pool
char *pa_pstrcat(pa_pool_t *p, ...);

// use libgc
void* pa_sdbm_malloc(unsigned int size);

#ifdef __cplusplus
}
#endif

#endif  /* !PA_STRINGS_H */
