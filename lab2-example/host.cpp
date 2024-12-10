/**********
Copyright (c) 2019, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#include "xcl2.hpp"
#include "event_timer.hpp"
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cstdint>

//#include <time.h>

#define lm 4
#define ln 4
#define lp 4

#define m (1 << lm)
#define n (1 << ln)
#define p (1 << lp)

#define DATA_SIZE m

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
    return EXIT_FAILURE;
  }

  EventTimer et;

  std::string binaryFile = argv[1];
  size_t vector_size_bytes = sizeof(unsigned int) * DATA_SIZE * DATA_SIZE;
  cl_int err;
  cl::Context context;
  cl::Kernel krnl_vector_mul;
  cl::CommandQueue q;
  // Allocate Memory in Host Memory
  // When creating a buffer with user pointer (CL_MEM_USE_HOST_PTR), under the
  // hood user ptr
  // is used if it is properly aligned. when not aligned, runtime had no choice
  // but to create
  // its own host side buffer. So it is recommended to use this allocator if
  // user wish to
  // create buffer using CL_MEM_USE_HOST_PTR to align user buffer to page
  // boundary. It will
  // ensure that user buffer is used when user create Buffer/Mem object with
  // CL_MEM_USE_HOST_PTR
  et.add("Allocate Memory in Host Memory");

//Take 2
//  std::vector<std::vector<unsigned int>> source_in1(DATA_SIZE, std::vector<unsigned int>(DATA_SIZE));
//  std::vector<std::vector<unsigned int>> source_in2(DATA_SIZE, std::vector<unsigned int>(DATA_SIZE));
//  std::vector<std::vector<unsigned int>> source_hw_results(DATA_SIZE, std::vector<unsigned int>(DATA_SIZE, 0));
//  std::vector<std::vector<unsigned int>> source_sw_results(DATA_SIZE, std::vector<unsigned int>(DATA_SIZE, 0));

  //Take3
//  std::vector<std::vector<unsigned int, aligned_allocator<unsigned int>>> source_in1(DATA_SIZE, std::vector<unsigned int, aligned_allocator<unsigned int>>(DATA_SIZE));
//  std::vector<std::vector<unsigned int, aligned_allocator<unsigned int>>> source_in2(DATA_SIZE, std::vector<unsigned int, aligned_allocator<unsigned int>>(DATA_SIZE));
//  std::vector<std::vector<unsigned int, aligned_allocator<unsigned int>>> source_hw_results(DATA_SIZE, std::vector<unsigned int, aligned_allocator<unsigned int>>(DATA_SIZE));
//  std::vector<std::vector<unsigned int, aligned_allocator<unsigned int>>> source_sw_results(DATA_SIZE, std::vector<unsigned int, aligned_allocator<unsigned int>>(DATA_SIZE));

  std::vector<unsigned int, aligned_allocator<unsigned int>> source_in1(DATA_SIZE * DATA_SIZE);
  std::vector<unsigned int, aligned_allocator<unsigned int>> source_in2(DATA_SIZE * DATA_SIZE);
  std::vector<unsigned int, aligned_allocator<unsigned int>> source_hw_results(DATA_SIZE * DATA_SIZE);
  std::vector<unsigned int, aligned_allocator<unsigned int>> source_sw_results(DATA_SIZE * DATA_SIZE);


//original
  //std::vector<int, aligned_allocator<int>> source_in1(DATA_SIZE);
  //std::vector<int, aligned_allocator<int>> source_in2(DATA_SIZE);
  //std::vector<int, aligned_allocator<int>> source_hw_results(DATA_SIZE);
  //std::vector<int, aligned_allocator<int>> source_sw_results(DATA_SIZE);
  et.finish();

  // Create the test data
  et.add("Fill the buffers");
//  std::generate(source_in1.begin(), source_in1.end(), std::rand);
//  std::generate(source_in2.begin(), source_in2.end(), std::rand);

//  auto rand_gen = []() { return rand() % 256; };
//
//  for (auto& row : source_in1) {
//      std::generate(row.begin(), row.end(), rand_gen);
//  }
//  for (auto& row : source_in2) {
//      std::generate(row.begin(), row.end(), rand_gen);
//  }
//
//  for (int i = 0; i < m; i++) {
//      for (int j = 0; j < p; j++) {
//          unsigned int sum = 0;
//          for (int k = 0; k < n; k++) {
//              sum += source_in1[i][k] * source_in2[k][j];
//          }
//          source_sw_results[i][j] = sum;
//          source_hw_results[i][j] = 0;
//      }
//  }

  // Random number generator
     auto rand_gen = []() { return rand() % 256; };

  // Fill source_in1 and source_in2 with random values
      for (int i = 0; i < DATA_SIZE; i++) {
          for (int j = 0; j < DATA_SIZE; j++) {
              source_in1[i * DATA_SIZE + j] = rand_gen(); // Accessing as 1D array
              source_in2[i * DATA_SIZE + j] = rand_gen(); // Accessing as 1D array
          }
      }

      // Compute the software reference results
      for (int i = 0; i < DATA_SIZE; i++) {
          for (int j = 0; j < DATA_SIZE; j++) {
              unsigned int sum = 0;
              for (int k = 0; k < DATA_SIZE; k++) {
                  sum += source_in1[i * DATA_SIZE + k] * source_in2[k * DATA_SIZE + j]; // Accessing as 1D array
              }
              source_sw_results[i * DATA_SIZE + j] = sum; // Storing result in 1D array
              source_hw_results[i * DATA_SIZE + j] = 0; // Initialize hardware results to 0
          }
      }

//  for (int i = 0; i < DATA_SIZE; i++) {
//    source_sw_results[i] = source_in1[i] + source_in2[i];
//    source_hw_results[i] = 0;
//  }

//  for (int i = 0; i < m; i++) {
//          for (int j = 0; j < p; j++) {
//              unsigned int sum = 0;
//              for (int k = 0; k < n; k++) {
//                  sum += (unsigned int)source_in1[i][k] * (unsigned int)source_in2[k][j];
//              }
//              source_sw_results[i][j] = sum;
//          }
//      }

  et.finish();

  //DO NOT TOUCH THIS BLOCK
  // OPENCL HOST CODE AREA START
  // get_xil_devices() is a utility API which will find the xilinx
  // platforms and will return list of devices connected to Xilinx platform
  auto devices = xcl::get_xil_devices();
  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  et.add("Load Binary File to Alveo U200");
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  int valid_device = 0;
  for (unsigned int i = 0; i < devices.size(); i++) {
    auto device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                        CL_QUEUE_PROFILING_ENABLE, &err));
    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      OCL_CHECK(err, krnl_vector_mul = cl::Kernel(program, "vadd", &err));
      valid_device++;
      break; // we break because we found a valid device
    }
  }
  if (valid_device == 0) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }
  et.finish();

  //DO NOT TOUCH THIS BLOCK
  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  et.add("Allocate Buffer in Global Memory");
  OCL_CHECK(err, cl::Buffer buffer_in1(
                     context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                     vector_size_bytes, source_in1.data(), &err));
  OCL_CHECK(err, cl::Buffer buffer_in2(
                     context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                     vector_size_bytes, source_in2.data(), &err));
  OCL_CHECK(err, cl::Buffer buffer_output(
                     context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                     vector_size_bytes, source_hw_results.data(), &err));
  et.finish();

  //TOUCH THIS
  et.add("Set the Kernel Arguments");
  int size = DATA_SIZE * DATA_SIZE;
  OCL_CHECK(err, err = krnl_vector_mul.setArg(0, buffer_in1));
  OCL_CHECK(err, err = krnl_vector_mul.setArg(1, buffer_in2));
  OCL_CHECK(err, err = krnl_vector_mul.setArg(2, buffer_output));
  OCL_CHECK(err, err = krnl_vector_mul.setArg(3, size));
  et.finish();

  // Copy input data to device global memory
  et.add("Copy input data to device global memory");
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2}, 0 /* 0 means from host*/));
  et.finish();

  // Launch the Kernel
  // For HLS kernels global and local size is always (1,1,1). So, it is
  // recommended
  // to always use enqueueTask() for invoking HLS kernel
  et.add("Launch the Kernel");
  OCL_CHECK(err, err = q.enqueueTask(krnl_vector_mul));
  et.finish();

  // Copy Result from Device Global Memory to Host Local Memory
  et.add("Copy Result from Device Global Memory to Host Local Memory");
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
  OCL_CHECK(err, err = q.finish());
  et.finish();
  // OPENCL HOST CODE AREA END

  // Compare the results of the Device to the simulation
  //et.add("Compare the results of the Device to the simulation");
  //bool match = true;
  //for (int i = 0; i < DATA_SIZE; i++) {
  //  if (source_hw_results[i] != source_sw_results[i]) {
  //    std::cout << "Error: Result mismatch" << std::endl;
//      std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
//                << " Device result = " << source_hw_results[i] << std::endl;
//      match = false;
//      break;
//    }
//  }


//	  bool match = true;
//      for (int i = 0; i < DATA_SIZE; i++) {
//          for (int j = 0; j < DATA_SIZE; j++) {
//        	  if (source_hw_results[i][j] != source_sw_results[i][j]) {
//        	        std::cout << "Error: Result mismatch" << std::endl;
//        	        std::cout << "i = " << i << "j = " << j << " CPU result = " << source_sw_results[i][j]
//        	                  << " Device result = " << source_hw_results[i][j] << std::endl;
//        	        match = false;
//        	        break;
//        	      }
//          }
//      }

  bool match = true;
  for (int i = 0; i < DATA_SIZE; i++) { //The outer loop
      for (int j = 0; j < DATA_SIZE; j++) { //The inner loop
          //Accessing elements using 1D indexing
          if (source_hw_results[i * DATA_SIZE + j] != source_sw_results[i * DATA_SIZE + j]) {
              std::cout << "Error: Result mismatch" << std::endl;
              std::cout << "i = " << i << " j = " << j
                        << " CPU result = " << source_sw_results[i * DATA_SIZE + j]
                        << " Device result = " << source_hw_results[i * DATA_SIZE + j] << std::endl;
              match = false;
              break; // Break out of the inner loop on mismatch
          }
      }
  }



  et.finish();

  std::cout <<"----------------- Key execution times -----------------" << std::endl;
  et.print();

  std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
  return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
