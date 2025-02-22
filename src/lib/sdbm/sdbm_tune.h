/* Copyright 2000-2005 The Apache Software Foundation or its licensors, as
 * applicable.
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

/*
 * sdbm - ndbm work-alike hashed database library
 * tuning and portability constructs [not nearly enough]
 * author: oz@nexus.yorku.ca
 */

#ifndef SDBM_TUNE_H
#define SDBM_TUNE_H

#include "pa_errno.h"

/* ### this might be better off as sizeof(char *) */
#define BYTESIZ		8

/*
 * misc
 */
#ifdef DEBUG
#define debug(x)	printf x
#else
#define debug(x)
#endif

#endif /* SDBM_TUNE_H */
