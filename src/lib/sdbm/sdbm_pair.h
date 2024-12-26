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

#ifndef SDBM_PAIR_H
#define SDBM_PAIR_H

/* Mini EMBED (pair.c) */
#define chkpage pa_sdbm_chkpage
#define delpair pa_sdbm_delpair
#define duppair pa_sdbm_duppair
#define fitpair pa_sdbm_fitpair
#define getnkey pa_sdbm_getnkey
#define getpair pa_sdbm_getpair
#define putpair pa_sdbm_putpair
#define splpage pa_sdbm_splpage

int fitpair(char *, int);
void  putpair(char *, pa_sdbm_datum_t, pa_sdbm_datum_t);
pa_sdbm_datum_t getpair(char *, pa_sdbm_datum_t);
int  delpair(char *, pa_sdbm_datum_t);
int  chkpage (char *);
pa_sdbm_datum_t getnkey(char *, int);
void splpage(char *, char *, long);
int duppair(char *, pa_sdbm_datum_t);

#endif /* SDBM_PAIR_H */

