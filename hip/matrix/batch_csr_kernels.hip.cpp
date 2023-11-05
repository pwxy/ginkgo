/*******************************<GINKGO LICENSE>******************************
Copyright (c) 2017-2023, the Ginkgo authors
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************<GINKGO LICENSE>*******************************/

#include "core/matrix/batch_csr_kernels.hpp"


#include <hip/hip_runtime.h>
#include <thrust/functional.h>


#include <ginkgo/core/base/batch_multi_vector.hpp>
#include <ginkgo/core/base/types.hpp>
#include <ginkgo/core/matrix/batch_csr.hpp>


#include "core/base/batch_struct.hpp"
#include "core/matrix/batch_struct.hpp"
#include "hip/base/batch_struct.hip.hpp"
#include "hip/base/config.hip.hpp"
#include "hip/base/thrust.hip.hpp"
#include "hip/components/cooperative_groups.hip.hpp"
#include "hip/components/reduction.hip.hpp"
#include "hip/components/thread_ids.hip.hpp"
#include "hip/components/uninitialized_array.hip.hpp"
#include "hip/matrix/batch_struct.hip.hpp"


namespace gko {
namespace kernels {
namespace hip {
/**
 * @brief The Csr matrix format namespace.
 * @ref Csr
 * @ingroup batch_csr
 */
namespace batch_csr {


constexpr auto default_block_size = 256;
constexpr int sm_oversubscription = 4;

// clang-format off

// NOTE: DO NOT CHANGE THE ORDERING OF THE INCLUDES

// #include "common/cuda_hip/matrix/batch_csr_kernels.hpp.inc"


#include "common/cuda_hip/matrix/batch_csr_kernel_launcher.hpp.inc"

// clang-format on


}  // namespace batch_csr
}  // namespace hip
}  // namespace kernels
}  // namespace gko
