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

#include "pa_file_info.h"
#include "pa_file_io.h"
#include "pa_sdbm.h"

#include "sdbm_private.h"
#include "sdbm_tune.h"

/* NOTE: this function blocks until it acquires the lock */
pa_status_t pa_sdbm_lock(pa_sdbm_t *db, int type)
{
    pa_status_t status;

    if (!(type == PA_FLOCK_SHARED || type == PA_FLOCK_EXCLUSIVE))
        return PA_EINVAL;

    if (db->flags & SDBM_EXCLUSIVE_LOCK) {
        ++db->lckcnt;
        return PA_SUCCESS;
    }
    else if (db->flags & SDBM_SHARED_LOCK) {
        /*
         * Cannot promote a shared lock to an exlusive lock
         * in a cross-platform compatibile manner.
         */
        if (type == PA_FLOCK_EXCLUSIVE)
            return PA_EINVAL;
        ++db->lckcnt;
        return PA_SUCCESS;
    }
    /*
     * zero size: either a fresh database, or one with a single,
     * unsplit data page: dirpage is all zeros.
     */
    if ((status = pa_file_lock(db->dirf, type)) == PA_SUCCESS) 
    {
        pa_finfo_t finfo;
        if ((status = pa_file_info_get(&finfo, PA_FINFO_SIZE, db->dirf))
                != PA_SUCCESS) {
            (void) pa_file_unlock(db->dirf);
            return status;
        }

        SDBM_INVALIDATE_CACHE(db, finfo);

        ++db->lckcnt;
        if (type == PA_FLOCK_SHARED)
            db->flags |= SDBM_SHARED_LOCK;
        else if (type == PA_FLOCK_EXCLUSIVE)
            db->flags |= SDBM_EXCLUSIVE_LOCK;
    }
    return status;
}

pa_status_t pa_sdbm_unlock(pa_sdbm_t *db)
{
    if (!(db->flags & (SDBM_SHARED_LOCK | SDBM_EXCLUSIVE_LOCK)))
        return PA_EINVAL;
    if (--db->lckcnt > 0)
        return PA_SUCCESS;
    db->flags &= ~(SDBM_SHARED_LOCK | SDBM_EXCLUSIVE_LOCK);
    return pa_file_unlock(db->dirf);
}
