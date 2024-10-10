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


#ifndef AMD_SMI_INCLUDE_IMPL_AMD_SMI_CPU_DEVICE_H_
#define AMD_SMI_INCLUDE_IMPL_AMD_SMI_CPU_DEVICE_H_

#include "amd_smi/amdsmi.h"
#include "amd_smi/impl/amd_smi_processor.h"
#include "shared_mutex.h"  // NOLINT
#include "rocm_smi/rocm_smi_logger.h"

namespace amd {
namespace smi {

// PID, amdsmi_proc_info_t
using CPUComputeProcessList_t = std::map<amdsmi_process_handle_t, amdsmi_proc_info_t>;
using ComputeProcessListClassType_t = uint16_t;

enum class ComputeProcessListType_t : ComputeProcessListClassType_t
{
    kAllProcesses,
    kAllProcessesOnDevice,
};

class AMDSmiCPUDevice : public AMDSmiProcessor {

 public:
    AMDSmiCPUDevice(uint32_t cpu_id, std::string model):
            AMDSmiProcessor(AMDSMI_PROCESSOR_TYPE_AMD_CPU), cpu_id_(cpu_id), model_(model) {}

    ~AMDSmiCPUDevice() {
    }

    uint32_t get_cpu_id() const;
    std::string& get_cpu_model();
    uint32_t get_vendor_id();
    const CPUComputeProcessList_t& amd_get_compute_process_list(ComputeProcessListType_t list_type = ComputeProcessListType_t::kAllProcessesOnDevice);
    const CPUComputeProcessList_t& amd_get_all_compute_process_list() {
        return amd_get_compute_process_list(ComputeProcessListType_t::kAllProcesses);
    }

    amdsmi_status_t amd_query_info(unsigned info_id, unsigned size, void *value) const;
    amdsmi_status_t amd_query_cpu_features(void *features) const;
    amdsmi_status_t amd_query_cpu_temperature(void *temperature) const;
    amdsmi_status_t amd_query_driver_name(std::string& name) const;
    amdsmi_status_t amd_query_driver_date(std::string& date) const;

 private:
    uint32_t cpu_id_;
    std::string model_;
    uint32_t vendor_id_;
    CPUComputeProcessList_t compute_process_list_;
    int32_t get_compute_process_list_impl(CPUComputeProcessList_t& compute_process_list,
                                          ComputeProcessListType_t list_type);
};

}  // namespace smi
}  // namespace amd

#endif  // AMD_SMI_INCLUDE_IMPL_AMD_SMI_CPU_DEVICE_H_


// #else 

// #ifndef AMD_SMI_INCLUDE_IMPL_AMD_SMI_CPU_DEVICE_H_
// #define AMD_SMI_INCLUDE_IMPL_AMD_SMI_CPU_DEVICE_H_

// #include "amd_smi/amdsmi.h"
// #include "amd_smi/impl/amd_smi_processor.h"
// #include "amd_smi/impl/amd_smi_drm.h"
// #include "shared_mutex.h"  // NOLINT
// #include "rocm_smi/rocm_smi_logger.h"

// namespace amd {
// namespace smi {

// // PID, amdsmi_proc_info_t
// using CPUComputeProcessList_t = std::map<amdsmi_process_handle_t, amdsmi_proc_info_t>;
// using ComputeProcessListClassType_t = uint16_t;

// // enum class ComputeProcessListType_t : ComputeProcessListClassType_t
// // {
// //     kAllProcesses,
// //     kAllProcessesOnDevice,
// // };


// class AMDSmiCPUDevice: public AMDSmiProcessor {

//  public:
//     AMDSmiCPUDevice(uint32_t CPU_id, uint32_t fd, std::string path, amdsmi_bdf_t bdf, AMDSmiDrm& drm):
//             AMDSmiProcessor(AMDSMI_PROCESSOR_TYPE_AMD_CPU), CPU_id_(CPU_id), fd_(fd), path_(path), bdf_(bdf), drm_(drm) {}

//     AMDSmiCPUDevice(uint32_t CPU_id, AMDSmiDrm& drm):
//             AMDSmiProcessor(AMDSMI_PROCESSOR_TYPE_AMD_CPU), CPU_id_(CPU_id), drm_(drm) {
//                 if (check_if_drm_is_supported()) this->get_drm_data();
//             }
//     ~AMDSmiCPUDevice() {
//     }

//     amdsmi_status_t init();
//     amdsmi_status_t cleanup();
//     uint32_t get_cpu_family(void *info) const;

    

// #if 1
//     amdsmi_status_t get_drm_data();
//     pthread_mutex_t* get_mutex();
//     uint32_t get_CPU_id() const;
//     uint32_t get_CPU_fd() const;
//     std::string& get_CPU_path();
//     amdsmi_bdf_t  get_bdf();
//     bool check_if_drm_is_supported() { return drm_.check_if_drm_is_supported(); }
//     uint32_t get_vendor_id();
//     // const CPUComputeProcessList_t& amdcpu_get_compute_process_list(ComputeProcessListType_t list_type = ComputeProcessListType_t::kAllProcessesOnDevice);
//     // const CPUComputeProcessList_t& amdcpu_get_all_compute_process_list() {
//     //     return amdcpu_get_compute_process_list(ComputeProcessListType_t::kAllProcesses);
//     // }

//     amdsmi_status_t amdcpu_query_info(unsigned info_id,
//                     unsigned size, void *value) const;
//     amdsmi_status_t amdcpu_query_hw_ip(unsigned info_id, unsigned hw_ip_type,
//             unsigned size, void *value) const;
//     amdsmi_status_t amdcpu_query_fw(unsigned info_id, unsigned fw_type,
//             unsigned size, void *value) const;
//     amdsmi_status_t amdcpu_query_vbios(void *info) const;
//     amdsmi_status_t amdcpu_query_driver_name(std::string& name) const;
//     amdsmi_status_t amdcpu_query_driver_date(std::string& date) const;
// #endif
//  private:
//     // uint32_t vendor_id;
//     uint32_t memory_size_in_GB;
//     uint32_t CPU_id_;
//     uint32_t fd_;
//     std::string path_;
//     amdsmi_bdf_t bdf_;
//     uint32_t vendor_id_;
//     AMDSmiDrm& drm_;

// /*
//  * total number of cores and sockets in the system
//  * This information is going to be fixed for a boot cycle.
//  */
// // struct system_metrics {
// // 	uint32_t total_cores;		// total cores in a system.
// // 	uint32_t total_sockets;		// total sockets in a system.
// // 	uint32_t threads_per_core;	// threads per core in each cpu.
// // 	uint32_t cpu_family;		// system cpu family.
// // 	uint32_t cpu_model;		// system cpu model.
// // 	int32_t  hsmp_proto_ver;	// hsmp protocol version.
// // 	esmi_status_t init_status;	// esmi init status
// // 	esmi_status_t energy_status;	// energy driver status
// // 	esmi_status_t msr_status;	// MSR driver status
// // 	esmi_status_t hsmp_status;	// hsmp driver status
// // 	struct cpu_mapping *map;
// // 	uint8_t df_pstate_max_limit;	// df pstate maximum limit
// // 	uint8_t gmi3_link_width_limit;	// gmi3 maximum link width
// // 	uint8_t pci_gen5_rate_ctl;
// // 	struct link_encoding *lencode;	// holds platform specifc link encodings
// // };

//     CPUComputeProcessList_t compute_process_list_;
//     int32_t get_compute_process_list_impl(CPUComputeProcessList_t& compute_process_list,
//                                           ComputeProcessListType_t list_type);

// };

// }  // namespace smi
// }  // namespace amd

// #endif  // AMD_SMI_INCLUDE_IMPL_AMD_SMI_CPU_DEVICE_H_

// #endif