
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <stdio.h>
#include <CL/cl.h>
#include <memory>
#include "Simulation.h"


std::string kernelFilename = "Kernel.cl";


void checkErr(cl_int err, std::string name, int line) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")"
			<< " on line " << line << "\n";
		exit(EXIT_FAILURE);
	}
}

namespace cl {

	std::string deviceTypeName(cl_device_type type) {
		switch (type) {
			case CL_DEVICE_TYPE_DEFAULT:
				return "DEF";
			case CL_DEVICE_TYPE_CPU:
				return "CPU";
			case CL_DEVICE_TYPE_GPU:
				return "GPU";
			case CL_DEVICE_TYPE_ACCELERATOR:
				return "ACC";
		}
		return "ERROR";
	}

	std::string getPlatformInfo(cl_platform_id platform) {
		const int BUF_SIZE = 50;
		char platformVendor[BUF_SIZE];
		char platformName[BUF_SIZE];
		char platformVersion[BUF_SIZE];

		clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, BUF_SIZE, platformVendor, nullptr);
		clGetPlatformInfo(platform, CL_PLATFORM_NAME, BUF_SIZE, platformName, nullptr);
		clGetPlatformInfo(platform, CL_PLATFORM_VERSION, BUF_SIZE, platformVersion, nullptr);

		std::stringstream ss;

		ss << "Platform: " << platformVendor << ", "
			<< platformName << ", "
			<< platformVersion << "\n";

		return ss.str();
	}

	std::string getDeviceInfo(cl_device_id device) {
		char name[50];
		cl_bool avalible;
		cl_uint addressBits;
		cl_ulong globalMem;
		cl_ulong globalMemCache;
		cl_ulong localMem;
		cl_ulong maxAllocSize;
		cl_device_type type;
		cl_uint maxFreq;
		cl_uint maxComputeUnits;
		size_t maxWGSze;
		cl_uint maxWorkDims;
		size_t maxWorkItemSizes[3];

		clGetDeviceInfo(device, CL_DEVICE_NAME, 50, name, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_AVAILABLE, sizeof(avalible), &avalible, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(addressBits), &addressBits, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(globalMem), &globalMem, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(globalMemCache), &globalMemCache, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(localMem), &localMem, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxAllocSize), &maxAllocSize, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(maxFreq), &maxFreq, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWGSze), &maxWGSze, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(maxWorkDims), &maxWorkDims, nullptr);
		clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(maxWorkItemSizes), &maxWorkItemSizes, nullptr);


		std::stringstream ss;

		ss << "Device: " << name << "\n"
			<< "\tType:\t\t\t" << deviceTypeName(type) << "\n"
			<< "\tAvalible:\t\t" << avalible << "\n"
			<< "\tMax allocation size:\t" << maxAllocSize << "\n"
			<< "\tAddress bits:\t\t" << addressBits << "\n"
			<< "\tGlobal memory:\t\t" << globalMem << "\n"
			<< "\tGlobal mem cache:\t" << globalMemCache << "\n"
			<< "\tLocal memory:\t\t" << localMem << "\n"
			<< "\tMax clock frequency:\t" << maxFreq << "\n"
			<< "\tMax compute units:\t" << maxComputeUnits << "\n"
			<< "\tMax workgroup size:\t" << maxWGSze << "\n"
			<< "\tMax work dimensions:\t" << maxWorkDims << "\n"
			<< "\tMax work item sizes:\t{" << maxWorkItemSizes[0] << ","
			<< maxWorkItemSizes[1] << ","
			<< maxWorkItemSizes[2] << "}\n";

		return ss.str();
	}

	std::string getDeviceName(cl_device_id device) {
		char name[50];
		clGetDeviceInfo(device, CL_DEVICE_NAME, 50, name, nullptr);
		return std::string(name);
	}

	std::string getKernelInfo(cl_kernel kernel, cl_device_id device) {

		size_t wgSize;
		size_t actualWgSize[3];
		cl_ulong localMem;
		size_t preferredWgSize;
		cl_ulong privateMem;

		clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(wgSize), &wgSize, nullptr);
		clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, sizeof(actualWgSize), &actualWgSize, nullptr);
		clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_LOCAL_MEM_SIZE, sizeof(localMem), &localMem, nullptr);
		clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(preferredWgSize), &preferredWgSize, nullptr);
		clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(privateMem), &privateMem, nullptr);


		std::stringstream ss;

		ss << "Device: " << getDeviceName(device) << "\n"
			<< "WgSize: \t\t" << wgSize << "\n"
			<< "LocalMem: \t\t" << localMem << "\n"
			<< "PrivateMem: \t\t" << privateMem << "\n"
			<< "PreferredWGSize: \t" << preferredWgSize << "\n"
			;

		return ss.str();
	}

	std::vector<cl_platform_id> getPlatforms() {
		cl_int err;
		cl_uint num_platforms;
		std::vector<cl_platform_id> pids;

		err = clGetPlatformIDs(0, nullptr, &num_platforms);
		checkErr(err, "clGetPlatformIDs (1)", __LINE__);
		pids.resize(num_platforms);
		err = clGetPlatformIDs(num_platforms, pids.data(), nullptr);
		checkErr(err, "clGetPlatformIDs (2)", __LINE__);

		return pids;
	}

	std::vector<cl_device_id> getDevices(cl_platform_id platform) {
		cl_int err;
		cl_uint num_devices;
		std::vector<cl_device_id> dids;

		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices);
		checkErr(err, "clGetDeviceIDs (1)", __LINE__);
		dids.resize(num_devices);
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, dids.data(), nullptr);
		checkErr(err, "clGetDeviceIDs (1)", __LINE__);

		return dids;
	}

	void printAll() {
		cl_int err;
		auto platformList = getPlatforms();
		std::cout << "Number of platforms is: " << platformList.size() << "\n";
		checkErr(platformList.size() > 0 ? CL_SUCCESS : -1, "platformList.size() > 0", __LINE__);

		for (size_t i = 0; i < platformList.size(); i++) {
			auto& platform = platformList.at(i);

			std::cout << getPlatformInfo(platform);
			auto devices = getDevices(platform);
			checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0", __LINE__);

			for (size_t j = 0; j < devices.size(); j++) {
				std::cout << getDeviceInfo(devices[j]);
			}
		}
	}
}


std::string loadKernelFile(std::string filename) {
	std::ifstream file(filename);
	checkErr(file.is_open() ? CL_SUCCESS : -1, "opening file: " + filename + "\n", __LINE__);

	std::string content;
	file.seekg(0, std::ios::end);
	content.resize(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&content[0], content.size());
	file.close();

	return content;
}


const size_t pltfC = 0;
const size_t deviC = 0;

const size_t max_agents = 600;
const size_t max_walls = 50;



static struct {
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;

	cl_mem bufferA;
	cl_mem bufferW;

	cl_program program;
	cl_kernel kernel;

	cl_command_queue queue;
} clo;

void initGpuCode() {
	cl_int err;

	const auto platformList = cl::getPlatforms();
	checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get", __LINE__);
	clo.platform = platformList.at(pltfC);

	
	const auto devices = cl::getDevices(clo.platform);
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "No devices!", __LINE__);

	clo.device = devices.at(deviC);
	std::cout << cl::getDeviceName(clo.device) << "\n";

	clo.context = clCreateContext(nullptr, 1, &clo.device, nullptr, nullptr, &err);
	checkErr(err, "clCreateContext", __LINE__);

	// Setup the buffers
	clo.bufferA = clCreateBuffer(clo.context, CL_MEM_READ_WRITE, max_agents*sizeof(Agent), nullptr, &err);
	checkErr(err, "clCreateBuffer(Agent)", __LINE__);

	clo.bufferW = clCreateBuffer(clo.context, CL_MEM_WRITE_ONLY, max_walls * sizeof(Wall), nullptr, &err);
	checkErr(err, "clCreateBuffer(Wall)", __LINE__);

	// Compile kernel
	std::string kernelSource = loadKernelFile(kernelFilename);
	clo.program = clCreateProgramWithSource(clo.context, 1, (const char**) kernelSource.c_str(), nullptr, &err);
	checkErr(err, "clCreateProgramWithSource", __LINE__);

	err = clBuildProgram(clo.program, 1, &clo.device, "-cl-single-precision-constant", nullptr, nullptr);
	if (err) {
		std::cout << "ERROR: Program::build() " << err << " " << __LINE__ << "\n";

		size_t len = 0;
		clGetProgramBuildInfo(clo.program, clo.device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &len);
		auto log_buffer = std::make_unique<char[]>(len);
		err = clGetProgramBuildInfo(clo.program, clo.device, CL_PROGRAM_BUILD_LOG, len, log_buffer.get(), nullptr);
		std::cout << "Log: " << log_buffer.get() << "\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Kernel compiled!" << "\n";

	clo.kernel = clCreateKernel(clo.program, "interactions", &err);
	checkErr(err, "clCreateKernel", __LINE__);
	
	err = clSetKernelArg(clo.kernel, 0, sizeof(cl_mem), (void *) &clo.bufferA);
	checkErr(err, "clSetKernelArg(0)", __LINE__);
	err = clSetKernelArg(clo.kernel, 2, sizeof(cl_mem), (void *) &clo.bufferW);
	checkErr(err, "clSetKernelArg(2)", __LINE__);

	clo.queue = clCreateCommandQueue(clo.context, clo.device, CL_QUEUE_PROFILING_ENABLE, &err);
	checkErr(err, "clCreateCommandQueue", __LINE__);
}

void gpuIter(std::vector<Agent> agents, std::vector<Wall> walls){
	cl_int err;
	cl_event event;


	cl_int num_agents = agents.size();
	cl_int num_walls = walls.size();
	err = clSetKernelArg(clo.kernel, 1, sizeof(cl_int), (void *) &num_agents);
	checkErr(err, "clSetKernelArg(1)", __LINE__);
	err = clSetKernelArg(clo.kernel, 3, sizeof(cl_int), (void *) &num_walls);
	checkErr(err, "clSetKernelArg(3)", __LINE__);

	err = clEnqueueWriteBuffer(clo.queue, clo.bufferA, CL_TRUE, 0, agents.size()*sizeof(Agent), agents.data(), 0, nullptr, nullptr);
	checkErr(err, "clEnqueueWriteBuffer(Agent)", __LINE__);
	err = clEnqueueWriteBuffer(clo.queue, clo.bufferW, CL_TRUE, 0, walls.size()*sizeof(Wall), walls.data(), 0, nullptr, nullptr);
	checkErr(err, "clEnqueueWriteBuffer(Wall)", __LINE__);

	size_t global_work_size = agents.size();

	err = clEnqueueNDRangeKernel(clo.queue, clo.kernel, 1, nullptr, &global_work_size, nullptr, 0, nullptr, nullptr);
	checkErr(err, "clEnqueueNDRangeKernel", __LINE__);

	err = clEnqueueReadBuffer(clo.queue, clo.bufferA, CL_TRUE, 0, agents.size()*sizeof(Agent), agents.data(), 0, NULL, NULL);
	checkErr(err, "clEnqueueReadBuffer", __LINE__);
}


void gpuClose() {
	cl_int err;
	/* Finalization */
	err = clFlush(clo.queue);
	err = clFinish(clo.queue);
	err = clReleaseKernel(clo.kernel);
	err = clReleaseProgram(clo.program);
	err = clReleaseMemObject(clo.bufferA);
	err = clReleaseMemObject(clo.bufferW);
	err = clReleaseCommandQueue(clo.queue);
	err = clReleaseContext(clo.context);
}

