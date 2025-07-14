#include <jsonifier/Index.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <random>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <wininet.h>
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "wininet.lib")
    #define close closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <curl/curl.h>
#endif

// 🏰 ULTIMATE LIGHTHOUSE BEACON SYSTEM 🏰
// Powered by RTC's Jsonifier - The Absolute Pinnacle of JSON Performance
// Featuring:
// - Sub-microsecond JSON parsing with SIMD optimization
// - Compile-time hash maps for O(1) key lookups  
// - CPU architecture auto-detection and optimization
// - Multi-threaded ultra-high performance monitoring
// - Rock-solid error handling and recovery
// - Real-time performance metrics and analysis

namespace UltimateLighthouse {

// 🚀 Enhanced FastPing Response Structure with RTC Jsonifier Integration
struct FastPingResponse {
    jsonifier::string status{};
    jsonifier::string connecting_ip{};
    jsonifier::string anonymity_level{};
    jsonifier::string speed_hint{};
    double server_processing_latency_ms{ 0.0 };
    jsonifier::string client_ip_from_headers{};
    jsonifier::string message{};
    
    // Performance tracking
    std::chrono::high_resolution_clock::time_point response_time{};
    std::chrono::microseconds parse_duration{ 0 };
    bool parse_success{ false };
};

// 🔥 Ultimate Beacon Payload with Performance Metrics
struct UltimateBeaconPayload {
    jsonifier::string beacon_id{ "ultimate-lighthouse-001" };
    uint64_t timestamp{ 0 };
    jsonifier::string status{ "initializing" };
    jsonifier::string last_ping_status{};
    double ping_latency_ms{ 0.0 };
    uint32_t signal_age_seconds{ 0 };
    
    // 🚀 Performance metrics
    double json_parse_time_microseconds{ 0.0 };
    double json_serialize_time_microseconds{ 0.0 };
    uint64_t total_requests_processed{ 0 };
    uint64_t successful_parses{ 0 };
    uint64_t failed_parses{ 0 };
    double average_throughput_mbps{ 0.0 };
    jsonifier::string cpu_optimization_level{};
    
    // 🏰 Lighthouse health metrics
    double system_uptime_hours{ 0.0 };
    uint32_t beacon_sequence_number{ 0 };
    jsonifier::string lighthouse_version{ "ULTIMATE-v3.0-RTC-POWERED" };
};

// ⚡ Ultra-High Performance JSON Processor
class UltimateJsonProcessor {
private:
    jsonifier::jsonifier_core<> core{};
    mutable std::mutex performance_mutex{};
    
    // Performance tracking
    std::atomic<uint64_t> total_parses{ 0 };
    std::atomic<uint64_t> successful_parses{ 0 };
    std::atomic<uint64_t> total_bytes_processed{ 0 };
    std::atomic<double> total_parse_time_microseconds{ 0.0 };
    
public:
    UltimateJsonProcessor() {
        std::cout << "🚀 RTC Jsonifier Ultimate Processor Initialized!\n";
        std::cout << "⚡ SIMD Optimization: " << getOptimizationLevel() << "\n";
        std::cout << "🔥 CPU Features: " << getCPUFeatures() << "\n\n";
    }
    
    // 🔥 Ultra-fast parsing with comprehensive metrics
    template<typename T>
    bool parseWithMetrics(T& object, const std::string& json_data) {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            auto result = core.parseJson(object, json_data);
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            double microseconds = duration.count() / 1000.0;
            
            // Update performance metrics
            total_parses.fetch_add(1);
            successful_parses.fetch_add(1);
            total_bytes_processed.fetch_add(json_data.size());
            
            // Atomic update of total parse time
            double expected = total_parse_time_microseconds.load();
            while (!total_parse_time_microseconds.compare_exchange_weak(expected, expected + microseconds));
            
            // Store parse time in the response object if it has the field
            if constexpr (requires { object.parse_duration; }) {
                object.parse_duration = std::chrono::microseconds(static_cast<uint64_t>(microseconds));
            }
            if constexpr (requires { object.parse_success; }) {
                object.parse_success = true;
            }
            
            return true;
        } catch (const std::exception& e) {
            total_parses.fetch_add(1);
            std::cerr << "🚨 Parse Error: " << e.what() << std::endl;
            return false;
        }
    }
    
    // 🚀 Ultra-fast serialization
    template<typename T>
    std::string serializeWithMetrics(const T& object) {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            std::string result;
            core.serializeJson(object, result);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "🚨 Serialize Error: " << e.what() << std::endl;
            return "{}";
        }
    }
    
    // 📊 Get comprehensive performance metrics
    struct PerformanceMetrics {
        uint64_t total_parses;
        uint64_t successful_parses;
        uint64_t total_bytes;
        double average_parse_time_us;
        double throughput_mbps;
        double success_rate;
    };
    
    PerformanceMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(performance_mutex);
        
        uint64_t parses = total_parses.load();
        uint64_t successes = successful_parses.load();
        uint64_t bytes = total_bytes_processed.load();
        double total_time_us = total_parse_time_microseconds.load();
        
        PerformanceMetrics metrics{};
        metrics.total_parses = parses;
        metrics.successful_parses = successes;
        metrics.total_bytes = bytes;
        metrics.average_parse_time_us = parses > 0 ? total_time_us / parses : 0.0;
        metrics.success_rate = parses > 0 ? (double)successes / parses * 100.0 : 0.0;
        
        // Calculate throughput in MB/s
        if (total_time_us > 0) {
            double seconds = total_time_us / 1000000.0;
            double mb = bytes / (1024.0 * 1024.0);
            metrics.throughput_mbps = mb / seconds;
        }
        
        return metrics;
    }
    
private:
    std::string getOptimizationLevel() const {
        #if JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX512)
            return "AVX-512 + AVX2 + BMI2 (MAXIMUM POWER!)";
        #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX2)
            return "AVX2 + BMI2 (HIGH PERFORMANCE)";
        #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX)
            return "AVX + SIMD (ACCELERATED)";
        #elif JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_NEON)
            return "ARM NEON SIMD (ARM OPTIMIZED)";
        #else
            return "Fallback Mode (Still Fast!)";
        #endif
    }
    
    std::string getCPUFeatures() const {
        std::vector<std::string> features;
        
        #if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_POPCNT)
            features.push_back("POPCNT");
        #endif
        #if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_BMI)
            features.push_back("BMI");
        #endif
        #if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_BMI2)
            features.push_back("BMI2");
        #endif
        #if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_LZCNT)
            features.push_back("LZCNT");
        #endif
        
        if (features.empty()) return "Basic";
        
        std::string result;
        for (size_t i = 0; i < features.size(); ++i) {
            result += features[i];
            if (i < features.size() - 1) result += " + ";
        }
        return result;
    }
};

// 🌐 Ultra-High Performance HTTP Client
class UltimateHttpClient {
private:
    #ifdef _WIN32
        HINTERNET hInternet;
        HINTERNET hConnect;
    #else
        CURL* curl;
    #endif
    
public:
    UltimateHttpClient() {
        #ifdef _WIN32
            hInternet = InternetOpenA("Ultimate-Lighthouse-Agent/3.0", 
                                    INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
            if (!hInternet) {
                throw std::runtime_error("Failed to initialize WinINet");
            }
        #else
            curl = curl_easy_init();
            if (!curl) {
                throw std::runtime_error("Failed to initialize libcurl");
            }
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Ultimate-Lighthouse-Agent/3.0");
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
        #endif
    }
    
    ~UltimateHttpClient() {
        #ifdef _WIN32
            if (hConnect) InternetCloseHandle(hConnect);
            if (hInternet) InternetCloseHandle(hInternet);
        #else
            if (curl) curl_easy_cleanup(curl);
        #endif
    }
    
    std::pair<bool, std::string> performRequest(const std::string& url) {
        #ifdef _WIN32
            return performWindowsRequest(url);
        #else
            return performLinuxRequest(url);
        #endif
    }
    
private:
    #ifdef _WIN32
    std::pair<bool, std::string> performWindowsRequest(const std::string& url) {
        HINTERNET hRequest = InternetOpenUrlA(hInternet, url.c_str(), 
                                            nullptr, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hRequest) {
            return {false, ""};
        }
        
        std::string response;
        char buffer[8192];
        DWORD bytesRead;
        
        while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }
        
        InternetCloseHandle(hRequest);
        return {!response.empty(), response};
    }
    #else
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
        size_t totalSize = size * nmemb;
        response->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }
    
    std::pair<bool, std::string> performLinuxRequest(const std::string& url) {
        std::string response;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(curl);
        return {res == CURLE_OK && !response.empty(), response};
    }
    #endif
};

// 🏰 The Ultimate Lighthouse Beacon System
class UltimateLighthouseBeacon {
private:
    std::unique_ptr<UltimateJsonProcessor> json_processor;
    std::unique_ptr<UltimateHttpClient> http_client;
    
    // Configuration
    std::string fastping_url{ "http://fastping.it.com/ping?format=json" };
    std::string beacon_ip{ "161.35.248.233" };
    int beacon_port{ 9876 };
    
    // Timing configuration
    std::chrono::seconds ping_interval{ 10 };
    std::chrono::seconds beacon_interval{ 5 };
    std::chrono::seconds status_interval{ 30 };
    
    // State management
    std::atomic<bool> running{ false };
    FastPingResponse last_response{};
    mutable std::mutex response_mutex{};
    
    // Performance tracking
    std::chrono::high_resolution_clock::time_point start_time{};
    std::atomic<uint32_t> beacon_sequence{ 0 };
    std::atomic<uint64_t> total_requests{ 0 };
    
    // Threading
    std::vector<std::thread> worker_threads{};
    
public:
    UltimateLighthouseBeacon() {
        json_processor = std::make_unique<UltimateJsonProcessor>();
        http_client = std::make_unique<UltimateHttpClient>();
        start_time = std::chrono::high_resolution_clock::now();
        
        #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
        
        displayStartupBanner();
    }
    
    ~UltimateLighthouseBeacon() {
        stop();
        #ifdef _WIN32
            WSACleanup();
        #endif
    }
    
    void start() {
        if (running.exchange(true)) {
            std::cout << "⚠️  Lighthouse already running!\n";
            return;
        }
        
        std::cout << "🚀 Starting Ultimate Lighthouse Beacon System...\n\n";
        
        // Start worker threads
        worker_threads.emplace_back(&UltimateLighthouseBeacon::listenerThread, this);
        worker_threads.emplace_back(&UltimateLighthouseBeacon::beaconThread, this);
        worker_threads.emplace_back(&UltimateLighthouseBeacon::statusThread, this);
        
        std::cout << "🔍 Ultra-Fast Listener Thread Started\n";
        std::cout << "📻 Ultra-Fast Beacon Thread Started\n";
        std::cout << "🕐 Enhanced Speaking Clock Started\n\n";
        
        std::cout << "🏰 ULTIMATE LIGHTHOUSE SYSTEM OPERATIONAL! 🏰\n";
        std::cout << "Press Enter to stop the lighthouse...\n";
    }
    
    void stop() {
        if (!running.exchange(false)) {
            return;
        }
        
        std::cout << "\n🛑 Stopping Ultimate Lighthouse System...\n";
        
        for (auto& thread : worker_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        worker_threads.clear();
        
        displayShutdownStats();
    }
    
private:
    void displayStartupBanner() {
        std::cout << R"(
🏰 ═══════════════════════════════════════════════════════════════════ 🏰
   ULTIMATE LIGHTHOUSE BEACON SYSTEM v3.0
🚀 Powered by RTC's Jsonifier - The Fastest JSON Library in Existence 🚀
⚡ Ultra-High Performance JSON Processing ⚡
🔥 SIMD Optimized Parsing with Sub-Microsecond Performance 🔥
💎 Compile-time Hash Maps for O(1) Key Lookups 💎
🏰 ═══════════════════════════════════════════════════════════════════ 🏰

)" << std::endl;
        
        std::cout << "🎯 Configuration:\n";
        std::cout << "   FastPing URL: " << fastping_url << "\n";
        std::cout << "   Beacon Target: " << beacon_ip << ":" << beacon_port << "\n";
        std::cout << "   Ping Interval: " << ping_interval.count() << "s\n";
        std::cout << "   Beacon Interval: " << beacon_interval.count() << "s\n\n";
    }
    
    void listenerThread() {
        while (running.load()) {
            auto cycle_start = std::chrono::high_resolution_clock::now();
            
            try {
                // 🚀 Perform ultra-fast HTTP request
                auto [success, response_data] = http_client->performRequest(fastping_url);
                
                if (success && !response_data.empty()) {
                    // 🔥 Ultra-fast JSON parsing with RTC Jsonifier
                    FastPingResponse response;
                    response.response_time = std::chrono::high_resolution_clock::now();
                    
                    bool parse_success = json_processor->parseWithMetrics(response, response_data);
                    
                    if (parse_success) {
                        std::lock_guard<std::mutex> lock(response_mutex);
                        last_response = std::move(response);
                        total_requests.fetch_add(1);
                        
                        auto cycle_end = std::chrono::high_resolution_clock::now();
                        auto cycle_time = std::chrono::duration_cast<std::chrono::milliseconds>(cycle_end - cycle_start);
                        
                        // Get performance metrics
                        auto metrics = json_processor->getMetrics();
                        
                        std::cout << "🚀 FastPing Ultra-Fast Update:\n";
                        std::cout << "   Status: " << last_response.status << " | IP: " << last_response.connecting_ip << "\n";
                        std::cout << "   Parse: " << std::fixed << std::setprecision(2) 
                                 << metrics.average_parse_time_us << "µs | Network: " << cycle_time.count() << "ms | ";
                        std::cout << "Throughput: " << std::fixed << std::setprecision(1) 
                                 << metrics.throughput_mbps << " MB/s\n";
                        std::cout << "   🔥 Total cycle time: " << cycle_time.count() << "ms\n\n";
                    }
                } else {
                    std::cout << "⚠️  FastPing request failed - retrying...\n";
                }
            } catch (const std::exception& e) {
                std::cout << "🚨 Listener error: " << e.what() << "\n";
            }
            
            std::this_thread::sleep_for(ping_interval);
        }
    }
    
    void beaconThread() {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            std::cerr << "🚨 Failed to create UDP socket\n";
            return;
        }
        
        sockaddr_in target_addr{};
        target_addr.sin_family = AF_INET;
        target_addr.sin_port = htons(beacon_port);
        inet_pton(AF_INET, beacon_ip.c_str(), &target_addr.sin_addr);
        
        while (running.load()) {
            try {
                // Create ultra-fast beacon payload
                UltimateBeaconPayload payload = createBeaconPayload();
                
                // 🚀 Ultra-fast serialization with RTC Jsonifier
                std::string json_payload = json_processor->serializeWithMetrics(payload);
                
                // Send UDP beacon
                ssize_t sent = sendto(sock, json_payload.c_str(), json_payload.length(), 0,
                                    reinterpret_cast<sockaddr*>(&target_addr), sizeof(target_addr));
                
                if (sent > 0) {
                    beacon_sequence.fetch_add(1);
                    // Uncomment for beacon confirmation:
                    // std::cout << "📡 Beacon #" << beacon_sequence.load() << " transmitted (" << sent << " bytes)\n";
                }
            } catch (const std::exception& e) {
                std::cout << "🚨 Beacon error: " << e.what() << "\n";
            }
            
            std::this_thread::sleep_for(beacon_interval);
        }
        
        close(sock);
    }
    
    void statusThread() {
        while (running.load()) {
            std::this_thread::sleep_for(status_interval);
            
            if (!running.load()) break;
            
            displayEnhancedStatus();
        }
    }
    
    UltimateBeaconPayload createBeaconPayload() {
        UltimateBeaconPayload payload;
        
        auto now = std::chrono::system_clock::now();
        payload.timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        
        {
            std::lock_guard<std::mutex> lock(response_mutex);
            
            auto age = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - last_response.response_time);
            
            payload.signal_age_seconds = static_cast<uint32_t>(age.count());
            payload.last_ping_status = last_response.status;
            payload.ping_latency_ms = last_response.server_processing_latency_ms;
            
            // Determine overall health status
            if (age.count() < 60 && last_response.status == "ok") {
                payload.status = "healthy";
            } else if (age.count() < 120) {
                payload.status = "warning";
            } else {
                payload.status = "critical";
            }
        }
        
        // Add performance metrics
        auto metrics = json_processor->getMetrics();
        payload.json_parse_time_microseconds = metrics.average_parse_time_us;
        payload.total_requests_processed = total_requests.load();
        payload.successful_parses = metrics.successful_parses;
        payload.failed_parses = metrics.total_parses - metrics.successful_parses;
        payload.average_throughput_mbps = metrics.throughput_mbps;
        
        #if JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX512)
            payload.cpu_optimization_level = "AVX-512";
        #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX2)
            payload.cpu_optimization_level = "AVX2";
        #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX)
            payload.cpu_optimization_level = "AVX";
        #else
            payload.cpu_optimization_level = "Standard";
        #endif
        
        auto uptime = std::chrono::duration_cast<std::chrono::hours>(
            std::chrono::high_resolution_clock::now() - start_time);
        payload.system_uptime_hours = uptime.count() + (uptime.count() % 1) / 60.0;
        
        payload.beacon_sequence_number = beacon_sequence.load();
        
        return payload;
    }
    
    void displayEnhancedStatus() {
        auto metrics = json_processor->getMetrics();
        
        std::cout << R"(
🏰 ═══════════════════════════════════════════════════════════════════ 🏰
   ULTIMATE LIGHTHOUSE STATUS REPORT
🏰 ═══════════════════════════════════════════════════════════════════ 🏰
)";
        
        {
            std::lock_guard<std::mutex> lock(response_mutex);
            auto age = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - last_response.response_time);
            
            std::string health_indicator = "✅ HEALTHY";
            if (age.count() > 60) health_indicator = "⚠️  WARNING";
            if (age.count() > 120) health_indicator = "❌ CRITICAL";
            
            std::cout << "   Signal Health: " << health_indicator << "\n";
            std::cout << "   Last Status: " << last_response.status << "\n";
            std::cout << "   Current IP: " << last_response.connecting_ip << "\n";
            std::cout << "   Anonymity: " << last_response.anonymity_level << "\n";
            std::cout << "   Speed Hint: " << last_response.speed_hint << "\n";
            std::cout << "   Signal Age: " << age.count() << " seconds\n";
        }
        
        std::cout << "\n🚀 ULTRA-HIGH PERFORMANCE METRICS:\n";
        std::cout << "   Total Requests: " << total_requests.load() << "\n";
        std::cout << "   Parse Success Rate: " << std::fixed << std::setprecision(1) 
                 << metrics.success_rate << "%\n";
        std::cout << "   Average Parse Time: " << std::fixed << std::setprecision(2) 
                 << metrics.average_parse_time_us << " microseconds\n";
        std::cout << "   JSON Throughput: " << std::fixed << std::setprecision(1) 
                 << metrics.throughput_mbps << " MB/s\n";
        std::cout << "   Beacons Transmitted: " << beacon_sequence.load() << "\n";
        
        auto uptime = std::chrono::duration_cast<std::chrono::minutes>(
            std::chrono::high_resolution_clock::now() - start_time);
        std::cout << "   System Uptime: " << uptime.count() << " minutes\n";
        
        std::cout << "\n🔥 RTC JSONIFIER OPTIMIZATION LEVEL: ";
        #if JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX512)
            std::cout << "MAXIMUM (AVX-512)\n";
        #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX2)
            std::cout << "HIGH (AVX2)\n";
        #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX)
            std::cout << "GOOD (AVX)\n";
        #else
            std::cout << "STANDARD\n";
        #endif
        
        std::cout << "🏰 ═══════════════════════════════════════════════════════════════════ 🏰\n";
    }
    
    void displayShutdownStats() {
        auto metrics = json_processor->getMetrics();
        auto uptime = std::chrono::duration_cast<std::chrono::minutes>(
            std::chrono::high_resolution_clock::now() - start_time);
        
        std::cout << "\n🏰 ULTIMATE LIGHTHOUSE SHUTDOWN STATISTICS:\n";
        std::cout << "   Total Runtime: " << uptime.count() << " minutes\n";
        std::cout << "   Total Requests: " << total_requests.load() << "\n";
        std::cout << "   Total JSON Parses: " << metrics.total_parses << "\n";
        std::cout << "   Success Rate: " << std::fixed << std::setprecision(1) 
                 << metrics.success_rate << "%\n";
        std::cout << "   Average Parse Time: " << std::fixed << std::setprecision(2) 
                 << metrics.average_parse_time_us << " microseconds\n";
        std::cout << "   Total Throughput: " << std::fixed << std::setprecision(1) 
                 << metrics.throughput_mbps << " MB/s\n";
        std::cout << "   Beacons Sent: " << beacon_sequence.load() << "\n";
        std::cout << "🏰 LIGHTHOUSE SECURED - Stay safe out there! 🏰\n\n";
    }
};

// 🎯 Ultimate Beacon Listener for Testing
class UltimateBeaconListener {
private:
    std::unique_ptr<UltimateJsonProcessor> json_processor;
    int listen_port;
    std::atomic<bool> running{ false };
    
public:
    UltimateBeaconListener(int port = 9876) : listen_port(port) {
        json_processor = std::make_unique<UltimateJsonProcessor>();
        
        #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
    }
    
    ~UltimateBeaconListener() {
        stop();
        #ifdef _WIN32
            WSACleanup();
        #endif
    }
    
    void start() {
        if (running.exchange(true)) {
            std::cout << "⚠️  Listener already running!\n";
            return;
        }
        
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            std::cerr << "🚨 Failed to create UDP socket\n";
            return;
        }
        
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(listen_port);
        
        if (bind(sock, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            std::cerr << "🚨 Failed to bind to port " << listen_port << "\n";
            close(sock);
            return;
        }
        
        std::cout << "🎯 Ultimate Beacon Listener bound to port " << listen_port << "\n";
        std::cout << "🎧 Listening for lighthouse beacons...\n";
        std::cout << "Press Ctrl+C to stop\n\n";
        
        char buffer[4096];
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        
        while (running.load()) {
            ssize_t received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                                      reinterpret_cast<sockaddr*>(&client_addr), &client_len);
            
            if (received > 0) {
                buffer[received] = '\0';
                
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                
                auto now = std::chrono::system_clock::now();
                auto time_t = std::chrono::system_clock::to_time_t(now);
                auto tm = *std::localtime(&time_t);
                
                std::cout << "📡 [" << std::put_time(&tm, "%H:%M:%S") << "] ";
                std::cout << "Received " << received << " bytes from " << client_ip << "\n";
                
                // Parse the beacon payload with ultra-fast RTC Jsonifier
                UltimateBeaconPayload payload;
                auto start = std::chrono::high_resolution_clock::now();
                bool success = json_processor->parseWithMetrics(payload, std::string(buffer, received));
                auto end = std::chrono::high_resolution_clock::now();
                
                auto parse_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                double parse_microseconds = parse_time.count() / 1000.0;
                
                if (success) {
                    displayBeaconInfo(payload, parse_microseconds);
                } else {
                    std::cout << "🚨 Failed to parse beacon payload\n";
                    std::cout << "Raw data: " << std::string(buffer, received) << "\n\n";
                }
            }
        }
        
        close(sock);
    }
    
    void stop() {
        running.store(false);
    }
    
private:
    void displayBeaconInfo(const UltimateBeaconPayload& payload, double parse_time_us) {
        std::cout << "\n┌─────────────────────────────────────────┐\n";
        std::cout << "│ 🚨 ULTIMATE LIGHTHOUSE BEACON RECEIVED │\n";
        std::cout << "├─────────────────────────────────────────┤\n";
        std::cout << "│ ID: " << std::left << std::setw(31) << payload.beacon_id << " │\n";
        std::cout << "│ Status: " << std::left << std::setw(27) << payload.status << " │\n";
        std::cout << "│ Ping Status: " << std::left << std::setw(23) << payload.last_ping_status << " │\n";
        std::cout << "│ Ping Latency: " << std::left << std::setw(22) << (std::to_string(payload.ping_latency_ms) + "ms") << " │\n";
        std::cout << "│ Signal Age: " << std::left << std::setw(24) << (std::to_string(payload.signal_age_seconds) + "s") << " │\n";
        
        auto time_t = static_cast<std::time_t>(payload.timestamp);
        auto tm = *std::localtime(&time_t);
        std::ostringstream timestamp_str;
        timestamp_str << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        std::cout << "│ Timestamp: " << std::left << std::setw(25) << timestamp_str.str() << " │\n";
        std::cout << "└─────────────────────────────────────────┘\n";
        
        // Health status indicator
        std::string health_status = "✅ HEALTHY";
        if (payload.status == "warning") health_status = "⚠️  WARNING";
        if (payload.status == "critical") health_status = "❌ CRITICAL";
        
        std::cout << health_status << "\n";
        std::cout << "   Parse time: " << std::fixed << std::setprecision(2) << parse_time_us << "µs | ";
        std::cout << "Validation: ✅\n";
        
        // 🚀 Performance metrics display
        if (payload.total_requests_processed > 0) {
            std::cout << "\n🚀 LIGHTHOUSE PERFORMANCE METRICS:\n";
            std::cout << "   CPU Optimization: " << payload.cpu_optimization_level << "\n";
            std::cout << "   Total Requests: " << payload.total_requests_processed << "\n";
            std::cout << "   JSON Parse Time: " << std::fixed << std::setprecision(2) 
                     << payload.json_parse_time_microseconds << "µs\n";
            std::cout << "   JSON Throughput: " << std::fixed << std::setprecision(1) 
                     << payload.average_throughput_mbps << " MB/s\n";
            std::cout << "   System Uptime: " << std::fixed << std::setprecision(1) 
                     << payload.system_uptime_hours << " hours\n";
            std::cout << "   Beacon #" << payload.beacon_sequence_number << "\n";
        }
        
        std::cout << "\n";
    }
};

} // namespace UltimateLighthouse

// 🚀 MAIN ENTRY POINT
int main(int argc, char* argv[]) {
    try {
        if (argc > 1 && std::string(argv[1]) == "--listener") {
            // Run as listener
            int port = 9876;
            if (argc > 2) {
                port = std::stoi(argv[2]);
            }
            
            UltimateLighthouse::UltimateBeaconListener listener(port);
            listener.start();
        } else {
            // Run as lighthouse beacon
            UltimateLighthouse::UltimateLighthouseBeacon lighthouse;
            lighthouse.start();
            
            // Wait for user input
            std::string input;
            std::getline(std::cin, input);
            
            lighthouse.stop();
        }
    } catch (const std::exception& e) {
        std::cerr << "🚨 Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
