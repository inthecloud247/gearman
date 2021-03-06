/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Gearmand client and server library.
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

/**
  Private structure.
*/
class FunctionV1: public _worker_function_st
{
  gearman_worker_fn *_worker_fn;

public:
  FunctionV1(gearman_worker_fn *worker_fn_arg, void *context_arg) :
    _worker_function_st(context_arg),
    _worker_fn(worker_fn_arg)
  { }

  bool has_callback() const
  {
    return bool(_worker_fn);
  }

  gearman_function_error_t callback(gearman_job_st* job, void *context_arg)
  {
    if (gearman_job_is_map(job))
    {
      gearman_job_build_reducer(job, NULL);
    }

    job->error_code= GEARMAN_SUCCESS;
    job->worker->impl()->work_result= _worker_fn(job, context_arg, &(job->worker->impl()->work_result_size), &job->error_code);

    if (job->error_code == GEARMAN_LOST_CONNECTION)
    {
      return GEARMAN_FUNCTION_ERROR;
    }

    if (job->error_code == GEARMAN_SHUTDOWN)
    {
      return GEARMAN_FUNCTION_SHUTDOWN;
    }

    if (gearman_failed(job->error_code))
    {
      return GEARMAN_FUNCTION_FATAL;
    }

    return GEARMAN_FUNCTION_SUCCESS;
  }
};

