/* Copyright 2010 Matthew Arsenault, Travis Desell, Boleslaw
Szymanski, Heidi Newberg, Carlos Varela, Malik Magdon-Ismail and
Rensselaer Polytechnic Institute.

This file is part of Milkway@Home.

Milkyway@Home is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Milkyway@Home is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "milkyway_util.h"
#include "milkyway_cl.h"
#include "mw_cl.h"
#include "setup_cl.h"
#include "separation_cl_buffers.h"
#include "separation_cl_defs.h"
#include "separation_binaries.h"

#if SEPARATION_INLINE_KERNEL
  #include "integral_kernel.h"
#endif /* SEPARATION_INLINE_KERNEL */


/* Only sets the constant arguments, not the outputs which we double buffer */
static inline cl_int separationSetKernelArgs(CLInfo* ci, SeparationCLMem* cm)
{
    cl_int err = CL_SUCCESS;

    /* The constant arguments */
    err |= clSetKernelArg(ci->kern, 2, sizeof(cl_mem), &cm->ap);
    err |= clSetKernelArg(ci->kern, 3, sizeof(cl_mem), &cm->ia);
    err |= clSetKernelArg(ci->kern, 4, sizeof(cl_mem), &cm->sc);
    err |= clSetKernelArg(ci->kern, 5, sizeof(cl_mem), &cm->rc);
    err |= clSetKernelArg(ci->kern, 6, sizeof(cl_mem), &cm->sg_dx);
    err |= clSetKernelArg(ci->kern, 7, sizeof(cl_mem), &cm->rPts);
    err |= clSetKernelArg(ci->kern, 8, sizeof(cl_mem), &cm->lbts);

    if (err != CL_SUCCESS)
    {
        warn("Error setting kernel arguments: %s\n", showCLInt(err));
        return err;
    }

    return CL_SUCCESS;
}

#if DOUBLEPREC
  #define DOUBLEPREC_DEF_STRING "-DDOUBLEPREC=1 "
#else
  #define DOUBLEPREC_DEF_STRING "-DDOUBLEPREC=0 -cl-single-precision-constant "
#endif /* DOUBLEPREC */

#if SEPARATION_INLINE_KERNEL

char* findKernelSrc()
{
    return integral_kernel_src;
}

void freeKernelSrc(char* src)
{
  #pragma unused(src)
}

#else

/* Reading from the file is more convenient for actually working on
 * it. Inlining is more useful for releasing when we don't want to
 * deal with the hassle of distributing more files. */
char* findKernelSrc()
{
    char* kernelSrc = NULL;
    kernelSrc = mwReadFile("../kernels/integrals.cl");
    if (!kernelSrc)
        warn("Failed to read kernel file\n");

    return kernelSrc;
}

void freeKernelSrc(char* src)
{
    free(src);
}

#endif


#define NUM_CONST_BUF_ARGS 4

/* Check that the device has the necessary resources */
static cl_bool separationCheckDevCapabilities(const DevInfo* di, const SeparationSizes* sizes)
{
    size_t totalOut;
    size_t totalConstBuf;

    totalOut = 2 * sizes->outMu + 2 * sizes->outProbs; /* 2 buffers for double buffering */
    if (totalOut > di->memSize)
    {
        warn("Device has insufficient global memory for output buffers\n");
        return CL_FALSE;
    }

    if (sizes->outMu > di->maxMemAlloc || sizes->outProbs > di->maxMemAlloc)
    {
        warn("An output buffer would exceed CL_DEVICE_MAX_MEM_ALLOC_SIZE\n");
        return CL_FALSE;
    }

    if (NUM_CONST_BUF_ARGS > di->maxConstArgs)
    {
        warn("Need more constant arguments than available\n");
        return CL_FALSE;
    }

    totalConstBuf = sizes->ap + sizes->ia + sizes->sc + sizes-> rc;
    if (totalConstBuf > di-> maxConstBufSize)
    {
        warn("Device doesn't have enough constant buffer space\n");
        return CL_FALSE;
    }

    return CL_TRUE;
}

cl_int setupSeparationCL(CLInfo* ci,
                         SeparationCLMem* cm,
                         const ASTRONOMY_PARAMETERS* ap,
                         const INTEGRAL_AREA* ia,
                         const STREAM_CONSTANTS* sc,
                         const STREAM_GAUSS sg,
                         const CLRequest* clr)
{
    cl_int err;
    char* compileDefs;
    char* kernelSrc;
    char* cwd;
    DevInfo di;
    SeparationSizes sizes;

    err = mwSetupCL(ci, &di, clr);
    if (err != CL_SUCCESS)
    {
        warn("Error getting device and context: %s\n", showCLInt(err));
        return err;
    }

    /* ATI CLC: --single_precision_constant */

    cwd = getcwd(NULL, 0);
    asprintf(&compileDefs, DOUBLEPREC_DEF_STRING
                           "-DUSE_CL_MATH_TYPES=0 "
                           "-DUSE_MAD=1 "
                           "-DUSE_FMA=1 "
                           "-cl-mad-enable "
                           "-cl-no-signed-zeros "
                           "-cl-strict-aliasing "
                           "-cl-finite-math-only "
                           "-I%s/../include "
                           "-I%s/../../include "
                           "-I%s/../../milkyway/include "
                           "-DNSTREAM=%u "
                           "-DFAST_H_PROB=%d "
                           "-DAUX_BG_PROFILE=%d "
                           "-DZERO_Q=%d ",
                           cwd, cwd, cwd,
                           ap->number_streams,
                           ap->fast_h_prob,
                           ap->aux_bg_profile,
                           ap->zero_q);

    kernelSrc = findKernelSrc();
    if (!kernelSrc)
    {
        warn("Failed to read CL kernel source\n");
        return -1;
    }

    err = mwSetProgramFromSrc(ci, "mu_sum_kernel", &kernelSrc, 1, compileDefs);

    freeKernelSrc(kernelSrc);
    free(cwd);
    free(compileDefs);

    if (err != CL_SUCCESS)
    {
        warn("Error creating program from source: %s\n", showCLInt(err));
        return err;
    }

    calculateSizes(&sizes, ap, ia);

    if (!separationCheckDevCapabilities(&di, &sizes))
    {
        warn("Device failed capability check\n");
        return -1;
    }

    err = createSeparationBuffers(ci, cm, ap, ia, sc, sg, &sizes);
    if (err != CL_SUCCESS)
    {
        warn("Failed to create CL buffers: %s\n", showCLInt(err));
        return err;
    }

    err = separationSetKernelArgs(ci, cm);
    if (err != CL_SUCCESS)
    {
        warn("Failed to set integral kernel arguments: %s\n", showCLInt(err));
        return err;
    }

    return CL_SUCCESS;
}

