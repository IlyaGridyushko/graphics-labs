#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
#include <vector>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")
#elif __linux__
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fstream>
#endif

namespace SystemInfo {

    // Utility function for drawing horizontal separators
    inline std::string getHorizontalLine(char c, int length = 80) {
        return std::string(length, c);
    }

    // Formats the time elapsed
    inline std::string formatTimeElapsed(double milliseconds) {
        std::stringstream ss;
        if (milliseconds < 1.0) {
            ss << std::fixed << std::setprecision(3) << (milliseconds * 1000.0) << " μs";
        } else if (milliseconds < 1000.0) {
            ss << std::fixed << std::setprecision(2) << milliseconds << " ms";
        } else {
            ss << std::fixed << std::setprecision(2) << (milliseconds / 1000.0) << " s";
        }
        return ss.str();
    }

    // Formats a memory size in bytes to a human-readable format
    inline std::string formatMemorySize(uint64_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024.0 && unitIndex < 4) {
            size /= 1024.0;
            unitIndex++;
        }
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
        return ss.str();
    }

    // Gets CPU information
    inline std::vector<std::string> getCPUInfo() {
        std::vector<std::string> info;
        
        #ifdef _WIN32
            // Windows CPU info
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            
            std::stringstream ss;
            ss << "Logical Processors: " << sysInfo.dwNumberOfProcessors;
            info.push_back(ss.str());
            
            // CPU architecture
            std::string architecture;
            switch(sysInfo.wProcessorArchitecture) {
                case PROCESSOR_ARCHITECTURE_AMD64: architecture = "x64 (AMD or Intel)"; break;
                case PROCESSOR_ARCHITECTURE_ARM: architecture = "ARM"; break;
                case PROCESSOR_ARCHITECTURE_ARM64: architecture = "ARM64"; break;
                case PROCESSOR_ARCHITECTURE_INTEL: architecture = "x86 (Intel)"; break;
                default: architecture = "Unknown"; break;
            }
            info.push_back("Architecture: " + architecture);
            
            // Get CPU brand string using cpuid
            char CPUBrandString[0x40] = {0};
            int CPUInfo[4] = {-1};
            
            __cpuid(CPUInfo, 0x80000000);
            unsigned int nExIds = CPUInfo[0];
            
            if (nExIds >= 0x80000004) {
                for (unsigned int i = 0x80000002; i <= 0x80000004; ++i) {
                    __cpuid(CPUInfo, i);
                    memcpy(CPUBrandString + (i - 0x80000002) * 16, CPUInfo, sizeof(CPUInfo));
                }
                info.push_back("Processor: " + std::string(CPUBrandString));
            }
        #elif __linux__
            // Linux CPU info
            std::ifstream cpuinfo("/proc/cpuinfo");
            std::string line;
            int processorCount = 0;
            std::string modelName;
            std::string cacheSize;
            
            if (cpuinfo.is_open()) {
                while (std::getline(cpuinfo, line)) {
                    if (line.find("processor") == 0) {
                        processorCount++;
                    }
                    else if (line.find("model name") == 0 && modelName.empty()) {
                        size_t pos = line.find(':');
                        if (pos != std::string::npos) {
                            modelName = line.substr(pos + 2);
                        }
                    }
                    else if (line.find("cache size") == 0 && cacheSize.empty()) {
                        size_t pos = line.find(':');
                        if (pos != std::string::npos) {
                            cacheSize = line.substr(pos + 2);
                        }
                    }
                }
                cpuinfo.close();
            }
            
            if (!modelName.empty()) {
                info.push_back("Processor: " + modelName);
            }
            
            std::stringstream ss;
            ss << "Logical Cores: " << processorCount;
            info.push_back(ss.str());
            
            if (!cacheSize.empty()) {
                info.push_back("Cache Size: " + cacheSize);
            }
            
            // Get CPU architecture
            struct utsname sysName;
            if (uname(&sysName) == 0) {
                info.push_back("Architecture: " + std::string(sysName.machine));
                info.push_back("OS Kernel: " + std::string(sysName.sysname) + " " + 
                              std::string(sysName.release) + " " + std::string(sysName.version));
            }
        #endif
        
        return info;
    }

    // Gets memory information
    inline std::vector<std::string> getMemoryInfo() {
        std::vector<std::string> info;
        
        #ifdef _WIN32
            // Windows memory info
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            
            uint64_t totalPhysMem = memInfo.ullTotalPhys;
            uint64_t availPhysMem = memInfo.ullAvailPhys;
            uint64_t totalVirtMem = memInfo.ullTotalVirtual;
            uint64_t availVirtMem = memInfo.ullAvailVirtual;
            
            info.push_back("Total Physical Memory: " + formatMemorySize(totalPhysMem));
            info.push_back("Available Physical Memory: " + formatMemorySize(availPhysMem));
            info.push_back("Total Virtual Memory: " + formatMemorySize(totalVirtMem));
            info.push_back("Available Virtual Memory: " + formatMemorySize(availVirtMem));
            
            std::stringstream ss;
            ss << "Memory Load: " << memInfo.dwMemoryLoad << "%";
            info.push_back(ss.str());
        #elif __linux__
            // Linux memory info
            struct sysinfo sysInfo;
            if (sysinfo(&sysInfo) == 0) {
                uint64_t totalPhysMem = sysInfo.totalram * sysInfo.mem_unit;
                uint64_t availPhysMem = sysInfo.freeram * sysInfo.mem_unit;
                uint64_t totalSwap = sysInfo.totalswap * sysInfo.mem_unit;
                uint64_t availSwap = sysInfo.freeswap * sysInfo.mem_unit;
                
                info.push_back("Total Physical Memory: " + formatMemorySize(totalPhysMem));
                info.push_back("Available Physical Memory: " + formatMemorySize(availPhysMem));
                info.push_back("Total Swap Space: " + formatMemorySize(totalSwap));
                info.push_back("Available Swap Space: " + formatMemorySize(availSwap));
                
                // Calculate memory load percentage
                double memoryLoad = 100.0 * (1.0 - (static_cast<double>(availPhysMem) / totalPhysMem));
                std::stringstream ss;
                ss << "Memory Load: " << std::fixed << std::setprecision(1) << memoryLoad << "%";
                info.push_back(ss.str());
            }
            
            // More detailed memory info from /proc/meminfo
            std::ifstream meminfo("/proc/meminfo");
            std::string line;
            std::string buffers, cached;
            
            if (meminfo.is_open()) {
                while (std::getline(meminfo, line)) {
                    if (line.find("Buffers:") == 0) {
                        buffers = line.substr(line.find(':') + 1);
                    }
                    else if (line.find("Cached:") == 0) {
                        cached = line.substr(line.find(':') + 1);
                        break; // We have what we need
                    }
                }
                meminfo.close();
                
                if (!buffers.empty()) info.push_back("Memory Buffers:" + buffers);
                if (!cached.empty()) info.push_back("Memory Cached:" + cached);
            }
        #endif
        
        return info;
    }

    // Gets GPU details beyond basic OpenGL info
    inline std::vector<std::string> getDetailedGPUInfo() {
        std::vector<std::string> info;
        
        // Get renderer and vendor
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* vendor = glGetString(GL_VENDOR);
        const GLubyte* version = glGetString(GL_VERSION);
        
        if (renderer) info.push_back("GPU: " + std::string(reinterpret_cast<const char*>(renderer)));
        if (vendor) info.push_back("GPU Vendor: " + std::string(reinterpret_cast<const char*>(vendor)));
        if (version) info.push_back("OpenGL Version: " + std::string(reinterpret_cast<const char*>(version)));
        
        // Get GLSL version
        const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
        if (glslVersion) {
            info.push_back("GLSL Version: " + std::string(reinterpret_cast<const char*>(glslVersion)));
        }
        
        // Get max texture size
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        std::stringstream ss1;
        ss1 << "Max Texture Size: " << maxTextureSize << " x " << maxTextureSize << " pixels";
        info.push_back(ss1.str());
        
        // Get max viewport dimensions
        GLint maxViewportDims[2];
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewportDims);
        std::stringstream ss2;
        ss2 << "Max Viewport Dimensions: " << maxViewportDims[0] << " x " << maxViewportDims[1];
        info.push_back(ss2.str());
        
        // Get max render buffer size
        GLint maxRenderBufferSize;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
        std::stringstream ss3;
        ss3 << "Max Render Buffer Size: " << maxRenderBufferSize;
        info.push_back(ss3.str());
        
        // Check for extensions
        if (glfwExtensionSupported("GL_ARB_compute_shader"))
            info.push_back("Compute Shaders: Supported");
        else
            info.push_back("Compute Shaders: Not supported");
            
        if (glfwExtensionSupported("GL_ARB_tessellation_shader"))
            info.push_back("Tessellation Shaders: Supported");
        else
            info.push_back("Tessellation Shaders: Not supported");
            
        if (glfwExtensionSupported("GL_ARB_geometry_shader4"))
            info.push_back("Geometry Shaders: Supported");
        else
            info.push_back("Geometry Shaders: Not supported");
            
        if (glfwExtensionSupported("GL_ARB_separate_shader_objects"))
            info.push_back("Separate Shader Objects: Supported");
        else
            info.push_back("Separate Shader Objects: Not supported");
        
        // NVIDIA-specific info
        if (vendor && strcmp(reinterpret_cast<const char*>(vendor), "NVIDIA Corporation") == 0) {
            if (glfwExtensionSupported("GL_NVX_gpu_memory_info")) {
                GLint totalMemoryKb = 0;
                glGetIntegerv(0x9048, &totalMemoryKb); // GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
                GLint currentMemoryKb = 0;
                glGetIntegerv(0x9049, &currentMemoryKb); // GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_MEMORY_NVX
                
                std::stringstream ss4;
                ss4 << "Total GPU Memory: " << formatMemorySize(static_cast<uint64_t>(totalMemoryKb) * 1024);
                info.push_back(ss4.str());
                
                std::stringstream ss5;
                ss5 << "Available GPU Memory: " << formatMemorySize(static_cast<uint64_t>(currentMemoryKb) * 1024);
                info.push_back(ss5.str());
            }
        }
        
        // AMD-specific info
        if (vendor && strcmp(reinterpret_cast<const char*>(vendor), "AMD") == 0) {
            // Detailed info not easily accessible through OpenGL directly
            // Could try to parse renderer string for more details
            std::string rendererStr = reinterpret_cast<const char*>(renderer);
            std::stringstream ss;
            
            // Try to extract driver version from the renderer string
            size_t pos = rendererStr.find("LLVM");
            if (pos != std::string::npos) {
                size_t end = rendererStr.find(",", pos);
                if (end != std::string::npos) {
                    ss << "Driver: " << rendererStr.substr(pos, end - pos);
                    info.push_back(ss.str());
                }
            }
            
            // Extract DRM version if available
            pos = rendererStr.find("DRM");
            if (pos != std::string::npos) {
                size_t end = rendererStr.find(",", pos);
                if (end != std::string::npos) {
                    ss.str("");
                    ss << "DRM Version: " << rendererStr.substr(pos + 4, end - pos - 4);
                    info.push_back(ss.str());
                }
            }
        }
        
        return info;
    }

    // Gets library versions and build information
    inline std::vector<std::string> getLibraryVersions() {
        std::vector<std::string> info;
        
        // GLFW version
        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);
        std::stringstream ss1;
        ss1 << "GLFW: v" << major << "." << minor << "." << revision;
        info.push_back(ss1.str());
        
        // GLM version (compile-time constants)
        #ifdef GLM_VERSION_MAJOR
        std::stringstream ss2;
        ss2 << "GLM: v" << GLM_VERSION_MAJOR << "." 
            << GLM_VERSION_MINOR << "." 
            << GLM_VERSION_PATCH;
        info.push_back(ss2.str());
        #endif
        
        // ImGui version
        #ifdef IMGUI_VERSION
        info.push_back("ImGui: v" + std::string(IMGUI_VERSION));
        #endif
        
        // Compiler information
        #if defined(_MSC_VER)
        std::stringstream ss3;
        ss3 << "Compiler: MSVC " << _MSC_VER;
        info.push_back(ss3.str());
        #elif defined(__GNUC__)
        std::stringstream ss3;
        ss3 << "Compiler: GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
        info.push_back(ss3.str());
        #elif defined(__clang__)
        std::stringstream ss3;
        ss3 << "Compiler: Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__;
        info.push_back(ss3.str());
        #else
        info.push_back("Compiler: Unknown");
        #endif
        
        // C++ standard
        #if defined(__cplusplus)
        std::stringstream ss4;
        ss4 << "C++ Standard: ";
        if (__cplusplus >= 202002L)
            ss4 << "C++20 or newer (" << __cplusplus << ")";
        else if (__cplusplus >= 201703L)
            ss4 << "C++17 (" << __cplusplus << ")";
        else if (__cplusplus >= 201402L)
            ss4 << "C++14 (" << __cplusplus << ")";
        else if (__cplusplus >= 201103L)
            ss4 << "C++11 (" << __cplusplus << ")";
        else
            ss4 << "Pre-C++11 (" << __cplusplus << ")";
        info.push_back(ss4.str());
        #endif
        
        return info;
    }

    // Print all system information with advanced formatting
    inline void printAll() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "\n\n";
        std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                     ADVANCED SYSTEM DIAGNOSTICS REPORT                       ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";
        
        // System identity
        std::cout << "┌─────────────────────────────[ SYSTEM IDENTITY ]─────────────────────────────┐\n";
        
        // Get hostname and username
        char hostname[256] = {0};
        char username[256] = {0};
        
        #ifdef _WIN32
        DWORD size = sizeof(hostname);
        GetComputerNameA(hostname, &size);
        size = sizeof(username);
        GetUserNameA(username, &size);
        #else
        gethostname(hostname, sizeof(hostname));
        getlogin_r(username, sizeof(username));
        #endif
        
        time_t now = time(0);
        char timeBuffer[30];
        struct tm *timeinfo = localtime(&now);
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        std::cout << " Host: " << std::left << std::setw(30) << hostname;
        std::cout << "User: " << std::left << std::setw(30) << username << "\n";
        std::cout << " Time: " << std::left << std::setw(72) << timeBuffer << "\n";
        
        #ifdef __linux__
        // Get distribution info on Linux
        std::ifstream osRelease("/etc/os-release");
        std::string distro = "Linux";
        std::string line;
        if (osRelease.is_open()) {
            while (std::getline(osRelease, line)) {
                if (line.find("PRETTY_NAME=") == 0) {
                    distro = line.substr(13, line.length() - 14); // Remove quotes
                    break;
                }
            }
            osRelease.close();
        }
        std::cout << " OS:   " << std::left << std::setw(72) << distro << "\n";
        #elif _WIN32
        OSVERSIONINFOEX osInfo;
        ZeroMemory(&osInfo, sizeof(OSVERSIONINFOEX));
        osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        GetVersionEx((OSVERSIONINFO*)&osInfo);
        std::stringstream ss;
        ss << "Windows " << osInfo.dwMajorVersion << "." << osInfo.dwMinorVersion << " (Build " << osInfo.dwBuildNumber << ")";
        std::cout << " OS:   " << std::left << std::setw(72) << ss.str() << "\n";
        #else
        std::cout << " OS:   " << std::left << std::setw(72) << "Unknown" << "\n";
        #endif
        
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
        
        // Hardware specifications
        std::cout << "┌───────────────────────[ HARDWARE SPECIFICATIONS ]───────────────────────────┐\n";
        
        // CPU information
        std::cout << " CPU Information:                                                            \n";
        std::vector<std::string> cpuInfo = getCPUInfo();
        for (const auto& info : cpuInfo) {
            std::cout << " ├─ " << std::left << std::setw(73) << info << "\n";
        }
        
        // Memory information
        std::cout << " Memory Information:                                                         \n";
        std::vector<std::string> memInfo = getMemoryInfo();
        for (const auto& info : memInfo) {
            std::cout << " ├─ " << std::left << std::setw(73) << info << "\n";
        }
        
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
        
        // OpenGL Information
        std::cout << "┌───────────────────────[ GRAPHICS SPECIFICATIONS ]───────────────────────────┐\n";
        
        std::vector<std::string> gpuInfo = getDetailedGPUInfo();
        for (const auto& info : gpuInfo) {
            std::cout << " " << std::left << std::setw(76) << info << "\n";
        }
        
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
        
        // Library Versions
        std::cout << "┌─────────────────────────────[ LIBRARY VERSIONS ]─────────────────────────────┐\n";
        
        std::vector<std::string> libInfo = getLibraryVersions();
        for (const auto& info : libInfo) {
            std::cout << " " << std::left << std::setw(76) << info << "\n";
        }
        
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
        
        // OpenGL Extensions (just a summary count)
        GLint numExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
        
        std::cout << "┌───────────────────────────[ OPENGL CAPABILITIES ]────────────────────────────┐\n";
        std::cout << " Number of OpenGL Extensions: " << std::left << std::setw(48) << numExtensions << "\n";
        
        // Check for key OpenGL features
        std::cout << " Graphics Pipeline Stages:                                                   \n";
        if (glfwExtensionSupported("GL_ARB_vertex_program"))
            std::cout << " ├─ " << std::left << std::setw(73) << "Vertex Shaders: ✓" << "\n";
        else
            std::cout << " ├─ " << std::left << std::setw(73) << "Vertex Shaders: ✗" << "\n";
            
        if (glfwExtensionSupported("GL_ARB_fragment_program"))
            std::cout << " ├─ " << std::left << std::setw(73) << "Fragment Shaders: ✓" << "\n";
        else
            std::cout << " ├─ " << std::left << std::setw(73) << "Fragment Shaders: ✗" << "\n";
            
        if (glfwExtensionSupported("GL_ARB_geometry_shader4"))
            std::cout << " ├─ " << std::left << std::setw(73) << "Geometry Shaders: ✓" << "\n";
        else
            std::cout << " ├─ " << std::left << std::setw(73) << "Geometry Shaders: ✗" << "\n";
            
        if (glfwExtensionSupported("GL_ARB_tessellation_shader"))
            std::cout << " ├─ " << std::left << std::setw(73) << "Tessellation Shaders: ✓" << "\n";
        else
            std::cout << " ├─ " << std::left << std::setw(73) << "Tessellation Shaders: ✗" << "\n";
            
        if (glfwExtensionSupported("GL_ARB_compute_shader"))
            std::cout << " └─ " << std::left << std::setw(73) << "Compute Shaders: ✓" << "\n";
        else
            std::cout << " └─ " << std::left << std::setw(73) << "Compute Shaders: ✗" << "\n";
            
        std::cout << "                                                                            \n";
        std::cout << " Advanced Features:                                                         \n";
        
        if (glfwExtensionSupported("GL_ARB_framebuffer_object"))
            std::cout << " ├─ " << std::left << std::setw(73) << "Framebuffer Objects: ✓" << "\n";
        else
            std::cout << " ├─ " << std::left << std::setw(73) << "Framebuffer Objects: ✗" << "\n";
            
        if (glfwExtensionSupported("GL_ARB_texture_compression"))
            std::cout << " ├─ " << std::left << std::setw(73) << "Texture Compression: ✓" << "\n";
        else
            std::cout << " ├─ " << std::left << std::setw(73) << "Texture Compression: ✗" << "\n";
            
        if (glfwExtensionSupported("GL_ARB_instanced_arrays"))
            std::cout << " ├─ " << std::left << std::setw(73) << "Instanced Rendering: ✓" << "\n";
        else
            std::cout << " ├─ " << std::left << std::setw(73) << "Instanced Rendering: ✗" << "\n";
            
        if (glfwExtensionSupported("GL_ARB_occlusion_query"))
            std::cout << " ├─ " << std::left << std::setw(73) << "Occlusion Queries: ✓" << "\n";
        else
            std::cout << " ├─ " << std::left << std::setw(73) << "Occlusion Queries: ✗" << "\n";
            
        if (glfwExtensionSupported("GL_ARB_uniform_buffer_object"))
            std::cout << " └─ " << std::left << std::setw(73) << "Uniform Buffer Objects: ✓" << "\n";
        else
            std::cout << " └─ " << std::left << std::setw(73) << "Uniform Buffer Objects: ✗" << "\n";
            
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
        
        // Performance metrics and initialization time
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        std::cout << "┌────────────────────────────[ DIAGNOSTIC INFO ]──────────────────────────────┐\n";
        std::cout << " Report generation time: " << std::left << std::setw(54) << formatTimeElapsed(elapsedTime) << "\n";
        
        // Additional platform-specific performance metrics could be added here
        
        std::cout << "└────────────────────────────────────────────────────────────────────────────┘\n\n";
        
        std::cout << "                       [END OF DIAGNOSTIC REPORT]                             \n\n";
    }
    
    // Print only OpenGL information
    inline void printOpenGLInfo() {
        std::cout << "\n";
        std::cout << "┌───────────────────────[ OPENGL INFORMATION REPORT ]───────────────────────┐\n";
        std::cout << " " << std::left << std::setw(74) << ("OpenGL Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)))) << "\n";
        std::cout << " " << std::left << std::setw(74) << ("OpenGL Renderer: " + std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)))) << "\n";
        std::cout << " " << std::left << std::setw(74) << ("OpenGL Vendor: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)))) << "\n";
        std::cout << " " << std::left << std::setw(74) << ("GLSL Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)))) << "\n";
        std::cout << "└──────────────────────────────────────────────────────────────────────────┘\n\n";
    }
}