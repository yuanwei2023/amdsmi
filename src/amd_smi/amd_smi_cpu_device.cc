/*
 * =============================================================================
 * The University of Illinois/NCSA
 * Open Source License (NCSA)
 *
 * Copyright (c) 2024, Advanced Micro Devices, Inc.
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

#include "amd_smi/impl/amd_smi_gpu_device.h"
#include "amd_smi/impl/amd_smi_common.h"
#include "amd_smi/impl/fdinfo.h"
#include "rocm_smi/rocm_smi_kfd.h"
#include "rocm_smi/rocm_smi_utils.h"

#include <functional>
#include <map>
#include <memory>
#include <unordered_set>

#include <iostream>  
#include <fstream>  
#include <string>  

namespace amd {
namespace smi {

struct cpu_mapping {
	int proc_id;
	int apic_id;
	int sock_id;
};

#define CPU_INFO_LINE_SIZE	1024
#define CPU_INFO_PATH		"/proc/cpuinfo"

static char *delim1 = ":";
static char *delim2 = "\n";
static const char *proc_str = "processor";
static const char *apic_str = "apicid";
static const char *node_str = "physical id";

extern char energymon_path[DRVPATHSIZ];
extern bool *lut;
extern int lut_size;

amdsmi_status_t ErrnoTosmiStatus(int err) {
  switch (err) {
    case 0:      return AMDSMI_STATUS_SUCCESS;
    case ESRCH:  return AMDSMI_STATUS_NOT_FOUND;
    case EACCES: return AMDSMI_STATUS_PERMISSION;
    case EPERM:
    case ENOENT: return AMDSMI_STATUS_NOT_SUPPORTED;
    case EBADF:
    case EISDIR: return AMDSMI_STATUS_FILE_ERROR;
    case EINTR:  return AMDSMI_STATUS_INTERRUPT;
    case EIO:    return AMDSMI_STATUS_UNEXPECTED_SIZE;//AMDSMI_STATUS_IO
    case ENXIO:  return AMDSMI_STATUS_UNEXPECTED_DATA;
    case EBUSY:  return AMDSMI_STATUS_BUSY;
    default:     return AMDSMI_STATUS_UNKNOWN_ERROR;
  }
}

bool is_vm_guest() {
  // the cpuinfo will set hypervisor flag in VM guest
  const std::string hypervisor = "hypervisor";
  std::string line;

  // default to false if cannot find the file
  std::ifstream infile("/proc/cpuinfo");
  if (infile.fail()) {
    return false;
  }

  while (std::getline(infile, line)) {
    if (line.find(hypervisor) != std::string::npos) {
      return true;
    }
  }
  return false;
}

std::string get_model_name() {  
    std::ifstream infile("/proc/cpuinfo");  
    std::string line;  
    std::string model_name;  
  
    while (std::getline(infile, line)) {  
        if (line.find("model name") != std::string::npos) {  
            size_t colon_pos = line.find(":");  
            if (colon_pos != std::string::npos) {  
                model_name = line.substr(colon_pos + 2);  
                break;  
            }
        }
    }

    return model_name;  
}  
  
int main() {  
   
    std::cout << "Model Name: " << model_name << std::endl;  
    return 0;  
}  

//static esmi_status_t detect_packages(struct system_metrics *psysm)
static int32_t cpu_topology_parse() {

	uint32_t eax, ebx, ecx,edx;
	int max_cores_socket, ret;

	if (NULL == psysm) {
		return AMDSMI_STATUS_IO;
	}
	if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
		return AMDSMI_STATUS_IO;
	}
	psysm->cpu_family = ((eax >> 8) & 0xf) + ((eax >> 20) & 0xff);
	psysm->cpu_model = ((eax >> 16) & 0xf) * 0x10 + ((eax >> 4) & 0xf);

	if (!__get_cpuid(0x08000001e, &eax, &ebx, &ecx, &edx)) {
		return AMDSMI_STATUS_IO;
	}
	psysm->threads_per_core = ((ebx >> 8) & 0xff) + 1;

	ret = read_index(CPU_COUNT_PATH);
	if(ret < 0) {
		return AMDSMI_STATUS_IO;
	}
	psysm->total_cores = ret + 1;

	if (!__get_cpuid(0x1, &eax, &ebx, &ecx, &edx))
		return AMDSMI_STATUS_IO;

	max_cores_socket = ((ebx >> 16) & 0xff);

	/* Number of sockets in the system */
	psysm->total_sockets = psysm->total_cores / max_cores_socket;

	return ret;
}


uint32_t AMDSmiCPUDevice::get_cpu_family() const {
    return cpu_family_id;
}

amdsmi_status_t AMDSmiCPUDevice::read_cpuinfo() {
    int ret;;
    std::string falimy_name;

    ret = cpu_topology_parse()
    if (ret)
        return ret;

    return AMDSMI_STATUS_SUCCESS;
}

uint32_t AMDSmiCPUDevice::get_vendor_id() {
    return vendor_id_;
}

uint32_t AMDSmiCPUDevice::get_bios_version() {
    return bios_version;
}

uint32_t AMDSmiCPUDevice::get_memory_size() {
    return memory_size_in_GB;
}

std::string AMDSmiCPUDevice::get_scaling_driver() {
    return scaling_driver;
}

std::string AMDSmiCPUDevice::get_scaling_driver_mode() {
    return scaling_driver_mode;
}

amdsmi_status_t AMDSmiCPUDevice::check_if_amd_pstate_supported() {
    return (pstate_driver != NULL && pstate_driver.mode() >0) ? true : false;
}

AMDSmiCPUDevice& AMDSmiCPUDevice::getInstance(uint64_t flags) {
  // Assume c++11 or greater. static objects will be created by only 1 thread
  // and creation will be thread-safe.
  static AMDSmiCPUDevice singleton(flags);
  return singleton;
}

amdsmi_status_t AMDSmiDrm::init() {

amdsmi_status_t AMDSmiCPUDevice::cpu_topology_init() {

    amdsmi_status_t ret;

    ret = 

}

}  // namespace smi
}  // namespace amd