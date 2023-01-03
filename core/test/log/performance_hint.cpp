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

#include <ginkgo/core/log/performance_hint.hpp>


#include <iomanip>
#include <sstream>


#include <gtest/gtest.h>


#include <ginkgo/core/base/executor.hpp>


#include "core/test/utils.hpp"


namespace {


TEST(PerformanceHint, CatchesRepeatedAllocationFree)
{
    std::ostringstream ss;
    auto exec = gko::ReferenceExecutor::create();
    exec->add_logger(gko::log::PerformanceHint::create(ss));

    for (int i = 0; i < 101; i++) {
        gko::array<char>{exec, 25};
    }

    ASSERT_EQ(ss.str(),
              "[PERFORMANCE] >>> Observed 10 allocate-free pairs of size 25 "
              "that may point to unnecessary allocations.\n[PERFORMANCE] >>> "
              "Observed 100 allocate-free pairs of size 25 that may point to "
              "unnecessary allocations.\n");
}


TEST(PerformanceHint, IgnoresSmallAllocationFreePairs)
{
    std::ostringstream ss;
    auto exec = gko::ReferenceExecutor::create();
    exec->add_logger(gko::log::PerformanceHint::create(ss, 32));

    for (int i = 0; i < 101; i++) {
        gko::array<char>{exec, 25};
    }

    ASSERT_EQ(ss.str(), "");
}


TEST(PerformanceHint, IgnoresSmallAllocations)
{
    std::ostringstream ss;
    auto exec = gko::ReferenceExecutor::create();
    exec->add_logger(gko::log::PerformanceHint::create(ss, 16, 16, 50));

    {
        std::vector<gko::array<char>> big(10, gko::array<char>{exec, 32});
        std::vector<gko::array<char>> small(100, gko::array<char>{exec, 1});
        std::vector<gko::array<char>> big2(10, gko::array<char>{exec, 32});
    }

    ASSERT_EQ(ss.str(),
              "[PERFORMANCE] >>> Observed 10 allocate-free pairs of size 32 "
              "that may point to unnecessary allocations.\n");
}


TEST(PerformanceHint, ForgetsOldAllocations)
{
    std::ostringstream ss;
    auto exec = gko::ReferenceExecutor::create();
    exec->add_logger(gko::log::PerformanceHint::create(ss, 16, 16, 9));

    std::vector<gko::array<char>> data;
    data.reserve(10);
    for (int i = 0; i < 10; i++) {
        data.emplace_back(exec, 32);
    }
    // by now we have forgotten the first allocation.
    while (!data.empty()) {
        data.pop_back();
    }

    ASSERT_EQ(ss.str(), "");

    for (int i = 0; i < 10; i++) {
        data.emplace_back(exec, 32);
    }
    data.clear();

    ASSERT_EQ(ss.str(),
              "[PERFORMANCE] >>> Observed 10 allocate-free pairs of size 32 "
              "that may point to unnecessary allocations.\n");
}


template <typename T>
std::string ptr_to_str(const T* location)
{
    std::ostringstream oss;
    oss << std::hex << "0x" << gko::uintptr(location);
    return oss.str();
}


TEST(PerformanceHint, CatchesRepeatedCrossExecutorCopy)
{
    std::ostringstream ss;
    std::ostringstream ss2;
    auto exec = gko::ReferenceExecutor::create();
    auto exec2 = gko::ReferenceExecutor::create();
    exec->add_logger(gko::log::PerformanceHint::create(ss));
    exec2->add_logger(gko::log::PerformanceHint::create(ss2));
    gko::array<int> a_small{exec, 1};
    gko::array<int> b_small{exec2, 1};
    gko::array<int> a{exec, 16};
    gko::array<int> b{exec2, 16};
    b.fill(0);
    const auto ptr1 = ptr_to_str(a.get_data());
    const auto ptr2 = ptr_to_str(b.get_data());

    for (int i = 0; i < 101; i++) {
        a_small = b_small;
        a = b;
    }

    ASSERT_EQ(
        ss.str(),
        "[PERFORMANCE] >>> Observed 10 cross-executor copies from " + ptr2 +
            " that may point to unnecessary data transfers.\n[PERFORMANCE] >>> "
            "Observed 10 cross-executor copies to " +
            ptr1 +
            " that may point to unnecessary data transfers.\n[PERFORMANCE] >>> "
            "Observed 100 cross-executor copies from " +
            ptr2 +
            " that may point to unnecessary data transfers.\n[PERFORMANCE] >>> "
            "Observed 100 cross-executor copies to " +
            ptr1 + " that may point to unnecessary data transfers.\n");
    ASSERT_EQ(
        ss2.str(),
        "[PERFORMANCE] >>> Observed 10 cross-executor copies from " + ptr2 +
            " that may point to unnecessary data transfers.\n[PERFORMANCE] >>> "
            "Observed 10 cross-executor copies to " +
            ptr1 +
            " that may point to unnecessary data transfers.\n[PERFORMANCE] >>> "
            "Observed 100 cross-executor copies from " +
            ptr2 +
            " that may point to unnecessary data transfers.\n[PERFORMANCE] >>> "
            "Observed 100 cross-executor copies to " +
            ptr1 + " that may point to unnecessary data transfers.\n");
}


TEST(PerformanceHint, IgnoresSmallCrossExecutorCopy)
{
    std::ostringstream ss;
    std::ostringstream ss2;
    auto exec = gko::ReferenceExecutor::create();
    auto exec2 = gko::ReferenceExecutor::create();
    exec->add_logger(gko::log::PerformanceHint::create(ss, 16, 1024));
    exec2->add_logger(gko::log::PerformanceHint::create(ss2, 16, 1024));
    gko::array<int> a{exec, 16};
    gko::array<int> b{exec2, 16};
    b.fill(0);

    for (int i = 0; i < 101; i++) {
        a = b;
    }

    ASSERT_EQ(ss.str(), "");
    ASSERT_EQ(ss2.str(), "");
}


TEST(PerformanceHint, PrintsStatus)
{
    std::ostringstream ss;
    auto exec = gko::ReferenceExecutor::create();
    auto exec2 = gko::ReferenceExecutor::create();
    auto logger = gko::share(gko::log::PerformanceHint::create(ss));
    exec->add_logger(logger);
    gko::array<int> a{exec, 16};
    gko::array<int> b{exec2, 16};
    b.fill(0);
    const auto ptr1 = ptr_to_str(a.get_data());
    const auto ptr2 = ptr_to_str(b.get_data());

    for (int i = 0; i < 101; i++) {
        gko::array<int>{exec, 16};
        a = b;
    }
    ss.str("");

    logger->print_status();
    ASSERT_EQ(
        ss.str(),
        "[PERFORMANCE] >>> Observed 101 allocate-free pairs of size 64 that "
        "may point to unnecessary allocations.\n[PERFORMANCE] >>> Observed 101 "
        "cross-executor copies from " +
            ptr2 +
            " that may point to unnecessary data transfers.\n[PERFORMANCE] >>> "
            "Observed 101 cross-executor copies to " +
            ptr1 + " that may point to unnecessary data transfers.\n");
}


}  // namespace
