/*
 * =============================================================================
 * The University of Illinois/NCSA
 * Open Source License (NCSA)
 *
 * Copyright (c) 2023, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Developed by:
 *
 *                 AMD Research and AMD ROC Software Development
 *
 *                 Advanced Micro Devices, Inc.
 *
 *                 www.amd.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimers.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimers in
 *    the documentation and/or other materials provided with the distribution.
 *  - Neither the names of <Name of Development Group, Name of Institution>,
 *    nor the names of its contributors may be used to endorse or promote
 *    products derived from this Software without specific prior written
 *    permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 *
 */

#ifndef AMD_SMI_INCLUDE_AMD_SMI_SYSTEM_H_
#define AMD_SMI_INCLUDE_AMD_SMI_SYSTEM_H_

#include <vector>
#include <set>
#include "amd_smi/amdsmi.h"
#include "amd_smi/impl/amd_smi_socket.h"
#include "amd_smi/impl/amd_smi_processor.h"
#include "amd_smi/impl/amd_smi_drm.h"
#ifdef ENABLE_ESMI_LIB
#include "amd_smi/impl/amd_smi_cpu_socket.h"
#endif

namespace amd {
namespace smi {

// Singleton: Only one system in an application
class AMDSmiSystem {
 public:
    static AMDSmiSystem& getInstance() {
        static AMDSmiSystem instance;
        return instance;
    }
    amdsmi_status_t init(uint64_t flags);
    amdsmi_status_t cleanup();

    std::vector<AMDSmiSocket*>& get_sockets() {return sockets_;}

    amdsmi_status_t handle_to_socket(amdsmi_socket_handle socket_handle,
            AMDSmiSocket** socket);

    amdsmi_status_t handle_to_processor(amdsmi_processor_handle processor_handle,
            AMDSmiProcessor** device);

    amdsmi_status_t gpu_index_to_handle(uint32_t gpu_index,
                    amdsmi_processor_handle* processor_handle);

#ifdef ENABLE_ESMI_LIB
    std::vector<AMDSmiCpuSocket*>& get_cpu_sockets() {return cpu_sockets_;}

    amdsmi_status_t handle_to_cpusocket(amdsmi_cpusocket_handle cpusock_handle,
            AMDSmiCpuSocket** cpu_socket);

    amdsmi_status_t cpu_index_to_handle(uint32_t cpu_index,
                    amdsmi_cpusocket_handle* cpusock_handle);

    amdsmi_status_t get_cpu_sockets(uint32_t *socks);

    amdsmi_status_t get_cpu_cores(uint32_t *cpus);

    amdsmi_status_t get_threads_per_core(uint32_t *threads);

    amdsmi_status_t get_cpu_family(uint32_t *family);

    amdsmi_status_t get_cpu_model(uint32_t *model);
#endif
 private:
    AMDSmiSystem() : init_flag_(AMDSMI_INIT_AMD_GPUS) {}

    /* The GPU socket id is used to identify the socket, so that the XCDs
    on the same physical device will be collected under the same socket.
    The BD part of the BDF is used as GPU socket to represent a phyiscal device.
    */
    amdsmi_status_t get_gpu_socket_id(uint32_t index, std::string& socketid);
    amdsmi_status_t populate_amd_gpu_devices();
    uint64_t init_flag_;
    AMDSmiDrm drm_;
    std::vector<AMDSmiSocket*> sockets_;
    std::set<AMDSmiProcessor*> processors_;     // Track valid processors
#ifdef ENABLE_ESMI_LIB
    amdsmi_status_t populate_amd_cpus();
    std::vector<AMDSmiCpuSocket*> cpu_sockets_;
    static uint32_t sockets;
    static uint32_t cpus;
    static uint32_t threads;
    static uint32_t family;
    static uint32_t model;
#endif
};



}  // namespace smi
}  // namespace amd

#endif  // AMD_SMI_INCLUDE_AMD_SMI_SYSTEM_H_
