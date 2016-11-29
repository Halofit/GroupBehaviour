
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <stdio.h>
#include <CL/cl.h>


std::string kernelFilename = "Kernel.cl";


void checkErr(cl_int err, std::string name, int line) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")"
			<< " on line " << line << "\n";
		exit(EXIT_FAILURE);
	}
}

namespace info {

	std::string mapDeviceType(cl_device_type type) {
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
			<< "\tType:\t\t\t" << mapDeviceType(type) << "\n"
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

/*
const size_t pltfC = 0;
const size_t deviC = 0;

int* mandelbrot(int sizeX, int sizeY) {
	cl_int err;
	// Get the platform
	std::vector<cl::Platform> platformList;
	cl::Platform::get(&platformList);
	checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get", __LINE__);
	// Select the first platform
	cl::Platform& platform = platformList.at(pltfC);

	// Get the devices from platform
	std::vector<cl::Device> devices;
	err = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "No devices!", __LINE__);

	cl::Device& device = devices.at(deviC);
	std::cout << info::getDeviceName(device) << "\n";

	cl::Context context(devices);

	// Setup the output buffer
	size_t canvasSize = (size_t) sizeX*(size_t) sizeY;
	int* canvas = new int[canvasSize];
	cl::Buffer outputBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, canvasSize * sizeof(int), canvas, &err);

	// Compile kernel
	std::string kernelSource = loadKernelFile(kernelFilename);
	cl::Program::Sources source(1, std::make_pair(kernelSource.c_str(), kernelSource.length() + 1));

	cl::Program program(context, source);
	err = program.build(devices, "-cl-single-precision-constant");
	if (err) {
		std::cout << "ERROR: Program::build() " << err << " " << __LINE__ << "\n";
		std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device, &err);
		checkErr(err, "Program::getBuildInfo()", __LINE__);
		std::cout << "Log: " << log << "\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Kernel compiled!" << "\n";

	cl::Kernel kernel(program, "mandelbrotRedundant", &err);
	checkErr(err, "Kernel::Kernel()", __LINE__);

	err = kernel.setArg(0, outputBuffer);
	checkErr(err, "Kernel::setArg(0)", __LINE__);
	err = kernel.setArg(1, 0);
	checkErr(err, "Kernel::setArg(1)", __LINE__);
	err = kernel.setArg(2, sizeX);
	checkErr(err, "Kernel::setArg(2)", __LINE__);
	err = kernel.setArg(3, sizeY);
	checkErr(err, "Kernel::setArg(3)", __LINE__);

	cl::CommandQueue queue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
	checkErr(err, "CommandQueue::CommandQueue()", __LINE__);


	cl::Event event;
	//(kernel, offsets, global dimensions, local dimensions, condition events, retrun event)
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(sizeX, sizeY),
									 cl::NullRange, nullptr, &event);

	checkErr(err, "ComamndQueue::enqueueNDRangeKernel()", __LINE__);

	event.wait();
	err = queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, canvasSize * sizeof(int), canvas);
	checkErr(err, "ComamndQueue::enqueueReadBuffer()", __LINE__);

	return canvas;
}
*/


#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)


#define _CRT_SECURE_NO_WARNINGS

int launchCl(void) {
	info::printAll();

	cl_device_id device_id = nullptr;
	cl_context context = nullptr;
	cl_command_queue command_queue = nullptr;
	cl_mem memobj = nullptr;
	cl_program program = nullptr;
	cl_kernel kernel = nullptr;
	cl_platform_id platform_id = nullptr;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;

	char string[MEM_SIZE];

	FILE *fp;
	char fileName[] = "./hello.cl";
	char *source_str;
	size_t source_size;

	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*) malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/* Get Platform and Device Info */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* Create OpenCL context */
	context = clCreateContext(nullptr, 1, &device_id, nullptr, nullptr, &ret);

	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Create Memory Buffer */
	memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(char), nullptr, &ret);

	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(context, 1, (const char **) &source_str,
		(const size_t *) &source_size, &ret);

	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);

	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "hello", &ret);

	/* Set OpenCL Kernel Parameters */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &memobj);

	/* Execute OpenCL Kernel */
	ret = clEnqueueTask(command_queue, kernel, 0, nullptr, nullptr);

	/* Copy results from the memory buffer */
	ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0,
							  MEM_SIZE * sizeof(char), string, 0, NULL, NULL);

	/* Display Result */
	puts(string);

	/* Finalization */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	free(source_str);

	return 0;
}