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

#ifndef _MW_CL_PROGRAM_H_
#define _MW_CL_PROGRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "milkyway_cl.h"
#include "mw_cl_setup.h"

unsigned char* mwGetProgramBinary(CLInfo* ci, size_t* binSizeOut);

cl_int mwSetProgramFromBin(CLInfo* ci, const char* kernName, const unsigned char* bin, size_t binSize);
cl_int mwSetProgramFromSrc(CLInfo* ci,
                           const char* kernName,
                           const char** src,
                           const cl_uint srcCount,
                           const char* compileDefs);
cl_int mwBuildProgram(CLInfo* ci, const char* options, const char* kernName);

#ifdef __cplusplus
}
#endif

#endif /* _MW_CL_PROGRAM_H_ */

