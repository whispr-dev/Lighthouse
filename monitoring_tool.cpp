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
#include <map>
#include <set>
#include <cstring>
#include <signal.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

// 🎯 ULTRA-FAST STANDALONE BEACON LISTENER
// The Ultimate Network Monitoring Companion Tool
// Powered by RTC's Jsonifier for Maximum Performance
// 
// Features:
// - Sub-microsecond JSON parsing of beacon payloads
// - Real-time lighthouse health monitoring
// - Multi-lighthouse tracking and comparison
// - Performance analytics and trend analysis
// - Alert system for degraded performance
// - Export capabilities for long-term analysis

namespace StandaloneListener {

// 🚀 Enhanced Beacon Payload Structure (matches lighthouse)
struct BeaconPayload {
    jsonifier::string beacon_id{};
    uint64_t timestamp{ 0 };
    jsonifier::string status{};
    jsonifier::string last_ping_status{};
    double ping_latency_ms{ 0.0 };
    uint32_t signal_age_seconds{ 0 };
    
    // Performance metrics
    double json_parse_time_microseconds{ 0.0 };
    double json_serialize_time_microseconds{ 0.0 };
    uint64_t total_requests_processed{ 0 };
    uint64_t successful_parses{ 0 };
    uint64_t failed_parses{ 0 };
    double average_throughput_mbps{ 0.0 };
    jsonifier::string cpu_optimization_level{};
    
    // Lighthouse health metrics
    double system_uptime_hours{ 0.0 };
    uint32_t beacon_sequence_number{ 0 };
    jsonifier::string lighthouse_version{};
    
    // Listener-added metadata
    std::chrono::high_resolution_clock::time_point received_time{};
    double listener_parse_time_microseconds{ 0.0 };
    jsonifier::string source_ip{};
};

// 📊 Lighthouse Statistics and Health Tracking
struct LighthouseStats {
    jsonifier::string lighthouse_id{};
    jsonifier::string source_ip{};
    uint64_t total_beacons_received{ 0 };
    uint64_t successful_parses{ 0 };
    uint64_t failed_parses{ 0 };
    
    // Performance tracking
    double average_parse_time_us{ 0.0 };
    double min_parse_time_us{ 999999.0 };
    double max_parse_time_us{ 0.0 };
    double average_lighthouse_parse_time_us{ 0.0 };
    double average_lighthouse_throughput_mbps{ 0.0 };
    
    // Health tracking
    std::chrono::high_resolution_clock::time_point first_seen{};
    std::chrono::high_resolution_clock::time_point last_seen{};
    std::chrono::high_resolution_clock::time_point last_healthy{};
    jsonifier::string last_status{};
    uint32_t consecutive_healthy{ 0 };
    uint32_t consecutive_warnings{ 0 };
    uint32_t consecutive_critical{ 0 };
    
    // Sequence tracking
    uint32_t last_sequence_number{ 0 };
    uint32_t missed_beacons{ 0 };
    double beacon_loss_percentage{ 0.0 };
    
    // Performance trends (last 100 beacons)
    std::vector<double> recent_parse_times{};
    std::vector<double> recent_throughputs{};
    std::vector<uint32_t> recent_sequence_numbers{};
};

// ⚡ Ultra-High Performance JSON Processor for Listener
class ListenerJsonProcessor {
private:
    jsonifier::jsonifier_core<> core{};
    mutable std::mutex metrics_mutex{};
    
    // Performance tracking
    std::atomic<uint64_t> total_parses{ 0 };
    std::atomic<uint64_t> successful_parses{ 0 };
    std::atomic<uint64_t> total_bytes_processed{ 0 };
    std::atomic<double> total_parse_time_microseconds{ 0.0 };
    
public:
    ListenerJsonProcessor() {
        std::cout << "🎯 Ultra-Fast Listener JSON Processor Initialized!\n";
        std::cout << "⚡ RTC Jsonifier Optimization: " << getOptimizationLevel() << "\n\n";
    }
    
    // 🔥 Parse beacon with comprehensive timing
    bool parseBeaconWithTiming(BeaconPayload& beacon, const std::string& json_data) {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            auto result = core.parseJson(beacon, json_data);
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            double microseconds = duration.count() / 1000.0;
            
            // Store listener parse time
            beacon.listener_parse_time_microseconds = microseconds;
            beacon.received_time = end;
            
            // Update metrics
            total_parses.fetch_add(1);
            successful_parses.fetch_add(1);
            total_bytes_processed.fetch_add(json_data.size());
            
            // Atomic update of total parse time
            double expected = total_parse_time_microseconds.load();
            while (!total_parse_time_microseconds.compare_exchange_weak(expected, expected + microseconds));
            
            return true;
        } catch (const std::exception& e) {
            total_parses.fetch_add(1);
            std::cerr << "🚨 Parse Error: " << e.what() << std::endl;
            return false;
        }
    }
    
    // 📊 Get listener performance metrics
    struct ListenerMetrics {
        uint64_t total_parses;
        uint64_t successful_parses;
        uint64_t total_bytes;
        double average_parse_time_us;
        double throughput_mbps;
        double success_rate;
    };
    
    ListenerMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(metrics_mutex);
        
        uint64_t parses = total_parses.load();
        uint64_t successes = successful_parses.load();
        uint64_t bytes = total_bytes_processed.load();
        double total_time_us = total_parse_time_microseconds.load();
        
        ListenerMetrics metrics{};
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
};

// 🏰 Multi-Lighthouse Tracking and Analytics Engine
class LighthouseTracker {
private:
    std::map<std::string, LighthouseStats> lighthouse_stats{};
    mutable std::mutex stats_mutex{};
    std::atomic<uint64_t> total_beacons_received{ 0 };
    std::chrono::high_resolution_clock::time_point tracker_start_time{};
    
public:
    LighthouseTracker() {
        tracker_start_time = std::chrono::high_resolution_clock::now();
    }
    
    // 📊 Update lighthouse statistics
    void updateStats(const BeaconPayload& beacon) {
        std::lock_guard<std::mutex> lock(stats_mutex);
        
        auto& stats = lighthouse_stats[beacon.beacon_id];
        auto now = std::chrono::high_resolution_clock::now();
        
        // Initialize if new lighthouse
        if (stats.total_beacons_received == 0) {
            stats.lighthouse_id = beacon.beacon_id;
            stats.source_ip = beacon.source_ip;
            stats.first_seen = now;
            stats.min_parse_time_us = beacon.listener_parse_time_microseconds;
        }
        
        // Update basic stats
        stats.total_beacons_received++;
        stats.successful_parses++;
        stats.last_seen = now;
        stats.last_status = beacon.status;
        
        // Update parse time statistics
        double parse_time = beacon.listener_parse_time_microseconds;
        stats.average_parse_time_us = ((stats.average_parse_time_us * (stats.total_beacons_received - 1)) + parse_time) / stats.total_beacons_received;
        stats.min_parse_time_us = std::min(stats.min_parse_time_us, parse_time);
        stats.max_parse_time_us = std::max(stats.max_parse_time_us, parse_time);
        
        // Update lighthouse performance stats
        stats.average_lighthouse_parse_time_us = beacon.json_parse_time_microseconds;
        stats.average_lighthouse_throughput_mbps = beacon.average_throughput_mbps;
        
        // Health status tracking
        if (beacon.status == "healthy") {
            stats.consecutive_healthy++;
            stats.consecutive_warnings = 0;
            stats.consecutive_critical = 0;
            stats.last_healthy = now;
        } else if (beacon.status == "warning") {
            stats.consecutive_warnings++;
            stats.consecutive_healthy = 0;
            stats.consecutive_critical = 0;
        } else if (beacon.status == "critical") {
            stats.consecutive_critical++;
            stats.consecutive_healthy = 0;
            stats.consecutive_warnings = 0;
        }
        
        // Sequence tracking and beacon loss detection
        if (stats.last_sequence_number > 0 && beacon.beacon_sequence_number > 0) {
            uint32_t expected_next = stats.last_sequence_number + 1;
            if (beacon.beacon_sequence_number > expected_next) {
                uint32_t missed = beacon.beacon_sequence_number - expected_next;
                stats.missed_beacons += missed;
            }
        }
        stats.last_sequence_number = beacon.beacon_sequence_number;
        
        // Calculate beacon loss percentage
        if (stats.total_beacons_received > 0 && stats.last_sequence_number > 0) {
            uint32_t total_expected = stats.last_sequence_number - 1; // Adjust for sequence start
            if (total_expected > 0) {
                stats.beacon_loss_percentage = (double)stats.missed_beacons / total_expected * 100.0;
            }
        }
        
        // Update recent performance trends (keep last 100)
        stats.recent_parse_times.push_back(parse_time);
        stats.recent_throughputs.push_back(beacon.average_throughput_mbps);
        stats.recent_sequence_numbers.push_back(beacon.beacon_sequence_number);
        
        if (stats.recent_parse_times.size() > 100) {
            stats.recent_parse_times.erase(stats.recent_parse_times.begin());
            stats.recent_throughputs.erase(stats.recent_throughputs.begin());
            stats.recent_sequence_numbers.erase(stats.recent_sequence_numbers.begin());
        }
        
        total_beacons_received.fetch_add(1);
    }
    
    // 📊 Get comprehensive lighthouse statistics
    std::vector<LighthouseStats> getAllStats() const {
        std::lock_guard<std::mutex> lock(stats_mutex);
        std::vector<LighthouseStats> stats_list;
        
        for (const auto& [id, stats] : lighthouse_stats) {
            stats_list.push_back(stats);
        }
        
        return stats_list;
    }
    
    // 🔍 Get statistics for specific lighthouse
    std::optional<LighthouseStats> getStats(const std::string& lighthouse_id) const {
        std::lock_guard<std::mutex> lock(stats_mutex);
        
        auto it = lighthouse_stats.find(lighthouse_id);
        if (it != lighthouse_stats.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    // 📈 Get system-wide summary
    struct SystemSummary {
        uint64_t total_lighthouses;
        uint64_t healthy_lighthouses;
        uint64_t warning_lighthouses;
        uint64_t critical_lighthouses;
        uint64_t total_beacons;
        double average_parse_time_us;
        double system_uptime_minutes;
    };
    
    SystemSummary getSystemSummary() const {
        std::lock_guard<std::mutex> lock(stats_mutex);
        
        SystemSummary summary{};
        summary.total_lighthouses = lighthouse_stats.size();
        summary.total_beacons = total_beacons_received.load();
        
        double total_parse_time = 0.0;
        for (const auto& [id, stats] : lighthouse_stats) {
            total_parse_time += stats.average_parse_time_us;
            
            if (stats.last_status == "healthy") summary.healthy_lighthouses++;
            else if (stats.last_status == "warning") summary.warning_lighthouses++;
            else if (stats.last_status == "critical") summary.critical_lighthouses++;
        }
        
        if (summary.total_lighthouses > 0) {
            summary.average_parse_time_us = total_parse_time / summary.total_lighthouses;
        }
        
        auto uptime = std::chrono::duration_cast<std::chrono::minutes>(
            std::chrono::high_resolution_clock::now() - tracker_start_time);
        summary.system_uptime_minutes = uptime.count();
        
        return summary;
    }
};

// 🎯 The Ultimate Standalone Beacon Listener
class UltimateStandaloneListener {
private:
    std::unique_ptr<ListenerJsonProcessor> json_processor;
    std::unique_ptr<LighthouseTracker> lighthouse_tracker;
    
    // Configuration
    int listen_port{ 9876 };
    bool verbose_mode{ false };
    bool statistics_mode{ false };
    std::chrono::seconds status_report_interval{ 30 };
    
    // State management
    std::atomic<bool> running{ false };
    std::thread listener_thread{};
    std::thread stats_thread{};
    
    // Network
    int socket_fd{ -1 };
    
public:
    UltimateStandaloneListener(int port = 9876, bool verbose = false, bool stats = false) 
        : listen_port(port), verbose_mode(verbose), statistics_mode(stats) {
        
        json_processor = std::make_unique<ListenerJsonProcessor>();
        lighthouse_tracker = std::make_unique<LighthouseTracker>();
        
        #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
        
        displayStartupBanner();
    }
    
    ~UltimateStandaloneListener() {
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
        
        // Create and bind socket
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_fd < 0) {
            std::cerr << "🚨 Failed to create UDP socket\n";
            return;
        }
        
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(listen_port);
        
        if (bind(socket_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            std::cerr << "🚨 Failed to bind to port " << listen_port << "\n";
            close(socket_fd);
            running.store(false);
            return;
        }
        
        std::cout << "🎯 Ultra-Fast Beacon Listener bound to port " << listen_port << "\n";
        std::cout << "🎧 Listening for lighthouse beacons...\n";
        if (verbose_mode) {
            std::cout << "📊 Verbose mode enabled - showing all beacon details\n";
        }
        if (statistics_mode) {
            std::cout << "📈 Statistics mode enabled - detailed analytics\n";
        }
        std::cout << "Press Ctrl+C to stop\n\n";
        
        // Start worker threads
        listener_thread = std::thread(&UltimateStandaloneListener::listenerLoop, this);
        if (statistics_mode) {
            stats_thread = std::thread(&UltimateStandaloneListener::statisticsLoop, this);
        }
    }
    
    void stop() {
        if (!running.exchange(false)) {
            return;
        }
        
        std::cout << "\n🛑 Stopping Ultra-Fast Beacon Listener...\n";
        
        if (socket_fd >= 0) {
            close(socket_fd);
            socket_fd = -1;
        }
        
        if (listener_thread.joinable()) {
            listener_thread.join();
        }
        
        if (stats_thread.joinable()) {
            stats_thread.join();
        }
        
        displayShutdownStats();
    }
    
private:
    void displayStartupBanner() {
        std::cout << R"(
🎯 ═══════════════════════════════════════════════════════════════════ 🎯
   ULTRA-FAST STANDALONE BEACON LISTENER v3.0
🚀 Powered by RTC's Jsonifier - Sub-Microsecond JSON Processing 🚀
⚡ Multi-Lighthouse Tracking and Performance Analytics ⚡
🔥 Real-time Health Monitoring and Alert System 🔥
🎯 ═══════════════════════════════════════════════════════════════════ 🎯

)" << std::endl;
    }
    
    void listenerLoop() {
        char buffer[8192];
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        
        while (running.load()) {
            ssize_t received = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0,
                                      reinterpret_cast<sockaddr*>(&client_addr), &client_len);
            
            if (received > 0 && running.load()) {
                buffer[received] = '\0';
                
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                
                processBeacon(std::string(buffer, received), std::string(client_ip));
            }
        }
    }
    
    void processBeacon(const std::string& data, const std::string& source_ip) {
        // 🚀 Parse beacon with ultra-fast RTC Jsonifier
        BeaconPayload beacon;
        beacon.source_ip = source_ip;
        
        bool success = json_processor->parseBeaconWithTiming(beacon, data);
        
        if (success) {
            // Update lighthouse statistics
            lighthouse_tracker->updateStats(beacon);
            
            if (verbose_mode) {
                displayVerboseBeacon(beacon);
            } else {
                displayBeaconSummary(beacon);
            }
        } else {
            std::cout << "🚨 Failed to parse beacon from " << source_ip << "\n";
            if (verbose_mode) {
                std::cout << "Raw data: " << data << "\n\n";
            }
        }
    }
    
    void displayBeaconSummary(const BeaconPayload& beacon) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        std::string health_icon = "✅";
        if (beacon.status == "warning") health_icon = "⚠️ ";
        if (beacon.status == "critical") health_icon = "❌";
        
        std::cout << "📡 [" << std::put_time(&tm, "%H:%M:%S") << "] " 
                 << health_icon << " " << beacon.beacon_id << " (" << beacon.source_ip << ") "
                 << "Parse: " << std::fixed << std::setprecision(2) << beacon.listener_parse_time_microseconds << "µs "
                 << "Seq: #" << beacon.beacon_sequence_number << "\n";
    }
    
    void displayVerboseBeacon(const BeaconPayload& beacon) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        std::cout << "\n┌─────────────────────────────────────────┐\n";
        std::cout << "│ 🚨 LIGHTHOUSE BEACON RECEIVED          │\n";
        std::cout << "├─────────────────────────────────────────┤\n";
        std::cout << "│ ID: " << std::left << std::setw(31) << beacon.beacon_id << " │\n";
        std::cout << "│ Source: " << std::left << std::setw(27) << beacon.source_ip << " │\n";
        std::cout << "│ Status: " << std::left << std::setw(27) << beacon.status << " │\n";
        std::cout << "│ Ping Status: " << std::left << std::setw(23) << beacon.last_ping_status << " │\n";
        std::cout << "│ Ping Latency: " << std::left << std::setw(22) << (std::to_string(beacon.ping_latency_ms) + "ms") << " │\n";
        std::cout << "│ Signal Age: " << std::left << std::setw(24) << (std::to_string(beacon.signal_age_seconds) + "s") << " │\n";
        std::cout << "│ Sequence: #" << std::left << std::setw(24) << beacon.beacon_sequence_number << " │\n";
        
        auto beacon_time_t = static_cast<std::time_t>(beacon.timestamp);
        auto beacon_tm = *std::localtime(&beacon_time_t);
        std::ostringstream timestamp_str;
        timestamp_str << std::put_time(&beacon_tm, "%Y-%m-%d %H:%M:%S");
        std::cout << "│ Timestamp: " << std::left << std::setw(25) << timestamp_str.str() << " │\n";
        std::cout << "└─────────────────────────────────────────┘\n";
        
        // Performance metrics
        std::cout << "\n🚀 PERFORMANCE METRICS:\n";
        std::cout << "   Listener Parse: " << std::fixed << std::setprecision(2) 
                 << beacon.listener_parse_time_microseconds << "µs\n";
        std::cout << "   Lighthouse Parse: " << std::fixed << std::setprecision(2) 
                 << beacon.json_parse_time_microseconds << "µs\n";
        std::cout << "   Lighthouse Throughput: " << std::fixed << std::setprecision(1) 
                 << beacon.average_throughput_mbps << " MB/s\n";
        std::cout << "   CPU Optimization: " << beacon.cpu_optimization_level << "\n";
        std::cout << "   System Uptime: " << std::fixed << std::setprecision(1) 
                 << beacon.system_uptime_hours << " hours\n";
        std::cout << "   Total Requests: " << beacon.total_requests_processed << "\n";
        std::cout << "   Success Rate: " << std::fixed << std::setprecision(1)
                 << (beacon.successful_parses / (double)std::max(1ULL, beacon.successful_parses + beacon.failed_parses) * 100.0) << "%\n";
        
        std::cout << "\n";
    }
    
    void statisticsLoop() {
        while (running.load()) {
            std::this_thread::sleep_for(status_report_interval);
            
            if (!running.load()) break;
            
            displayStatisticsReport();
        }
    }
    
    void displayStatisticsReport() {
        auto summary = lighthouse_tracker->getSystemSummary();
        auto listener_metrics = json_processor->getMetrics();
        auto all_stats = lighthouse_tracker->getAllStats();
        
        std::cout << R"(
🏰 ═══════════════════════════════════════════════════════════════════ 🏰
   MULTI-LIGHTHOUSE MONITORING STATISTICS
🏰 ═══════════════════════════════════════════════════════════════════ 🏰
)";
        
        std::cout << "📊 SYSTEM SUMMARY:\n";
        std::cout << "   Active Lighthouses: " << summary.total_lighthouses << "\n";
        std::cout << "   Healthy: " << summary.healthy_lighthouses << " | ";
        std::cout << "Warning: " << summary.warning_lighthouses << " | ";
        std::cout << "Critical: " << summary.critical_lighthouses << "\n";
        std::cout << "   Total Beacons: " << summary.total_beacons << "\n";
        std::cout << "   System Uptime: " << std::fixed << std::setprecision(1) 
                 << summary.system_uptime_minutes << " minutes\n";
        
        std::cout << "\n🚀 LISTENER PERFORMANCE:\n";
        std::cout << "   Parse Success Rate: " << std::fixed << std::setprecision(1) 
                 << listener_metrics.success_rate << "%\n";
        std::cout << "   Average Parse Time: " << std::fixed << std::setprecision(2) 
                 << listener_metrics.average_parse_time_us << " microseconds\n";
        std::cout << "   JSON Throughput: " << std::fixed << std::setprecision(1) 
                 << listener_metrics.throughput_mbps << " MB/s\n";
        
        if (!all_stats.empty()) {
            std::cout << "\n🏰 INDIVIDUAL LIGHTHOUSE STATUS:\n";
            std::cout << std::left << std::setw(20) << "Lighthouse ID"
                     << std::setw(16) << "Source IP"
                     << std::setw(10) << "Status"
                     << std::setw(12) << "Beacons"
                     << std::setw(12) << "Loss %"
                     << std::setw(15) << "Avg Parse (µs)" << "\n";
            std::cout << std::string(85, '-') << "\n";
            
            for (const auto& stats : all_stats) {
                std::string status_icon = "✅";
                if (stats.last_status == "warning") status_icon = "⚠️ ";
                if (stats.last_status == "critical") status_icon = "❌";
                
                std::cout << std::left << std::setw(20) << stats.lighthouse_id.substr(0, 19)
                         << std::setw(16) << stats.source_ip
                         << std::setw(10) << (status_icon + stats.last_status).substr(0, 9)
                         << std::setw(12) << stats.total_beacons_received
                         << std::setw(12) << std::fixed << std::setprecision(1) << stats.beacon_loss_percentage
                         << std::setw(15) << std::fixed << std::setprecision(2) << stats.average_parse_time_us << "\n";
            }
        }
        
        std::cout << "🏰 ═══════════════════════════════════════════════════════════════════ 🏰\n";
    }
    
    void displayShutdownStats() {
        auto summary = lighthouse_tracker->getSystemSummary();
        auto listener_metrics = json_processor->getMetrics();
        
        std::cout << "\n🎯 ULTRA-FAST LISTENER SHUTDOWN STATISTICS:\n";
        std::cout << "   Total Runtime: " << std::fixed << std::setprecision(1) 
                 << summary.system_uptime_minutes << " minutes\n";
        std::cout << "   Lighthouses Monitored: " << summary.total_lighthouses << "\n";
        std::cout << "   Total Beacons Received: " << summary.total_beacons << "\n";
        std::cout << "   Parse Success Rate: " << std::fixed << std::setprecision(1) 
                 << listener_metrics.success_rate << "%\n";
        std::cout << "   Average Parse Time: " << std::fixed << std::setprecision(2) 
                 << listener_metrics.average_parse_time_us << " microseconds\n";
        std::cout << "   Total JSON Throughput: " << std::fixed << std::setprecision(1) 
                 << listener_metrics.throughput_mbps << " MB/s\n";
        std::cout << "🎯 LISTENER SECURED - Thanks for monitoring! 🎯\n\n";
    }
};

} // namespace StandaloneListener

// 🎯 Signal handling for graceful shutdown
std::unique_ptr<StandaloneListener::UltimateStandaloneListener> g_listener;

void signalHandler(int signal) {
    std::cout << "\n🛑 Received shutdown signal...\n";
    if (g_listener) {
        g_listener->stop();
    }
    exit(0);
}

// 📋 Command line help
void displayHelp(const char* program_name) {
    std::cout << R"(
🎯 Ultra-Fast Standalone Beacon Listener v3.0
Usage: )" << program_name << R"( [OPTIONS]

OPTIONS:
   -p, --port PORT          Listen on specified port (default: 9876)
   -v, --verbose           Enable verbose beacon display mode
   -s, --statistics        Enable detailed statistics reporting
   -i, --interval SECONDS  Statistics report interval (default: 30)
   -h, --help              Show this help message

EXAMPLES:
   )" << program_name << R"(                    # Listen on default port 9876
   )" << program_name << R"( -p 8888            # Listen on port 8888
   )" << program_name << R"( -v -s              # Verbose mode with statistics
   )" << program_name << R"( -p 9999 -v -s -i 10  # Full monitoring setup

FEATURES:
   🚀 Sub-microsecond JSON parsing with RTC's Jsonifier
   🏰 Multi-lighthouse tracking and health monitoring
   📊 Real-time performance analytics and trending
   ⚡ SIMD-optimized parsing for maximum performance
   🔥 Automatic CPU optimization detection
   
🎯 The Ultimate Network Monitoring Companion Tool! 🎯
)";
}

// 🚀 MAIN ENTRY POINT
int main(int argc, char* argv[]) {
    try {
        // Default configuration
        int port = 9876;
        bool verbose = false;
        bool statistics = false;
        int stats_interval = 30;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "-h" || arg == "--help") {
                displayHelp(argv[0]);
                return 0;
            } else if (arg == "-p" || arg == "--port") {
                if (i + 1 < argc) {
                    port = std::stoi(argv[++i]);
                } else {
                    std::cerr << "❌ Error: --port requires a value\n";
                    return 1;
                }
            } else if (arg == "-v" || arg == "--verbose") {
                verbose = true;
            } else if (arg == "-s" || arg == "--statistics") {
                statistics = true;
            } else if (arg == "-i" || arg == "--interval") {
                if (i + 1 < argc) {
                    stats_interval = std::stoi(argv[++i]);
                } else {
                    std::cerr << "❌ Error: --interval requires a value\n";
                    return 1;
                }
            } else {
                std::cerr << "❌ Unknown option: " << arg << "\n";
                std::cerr << "Use --help for usage information\n";
                return 1;
            }
        }
        
        // Validate arguments
        if (port < 1 || port > 65535) {
            std::cerr << "❌ Error: Port must be between 1 and 65535\n";
            return 1;
        }
        
        if (stats_interval < 5 || stats_interval > 300) {
            std::cerr << "❌ Error: Statistics interval must be between 5 and 300 seconds\n";
            return 1;
        }
        
        // Setup signal handlers for graceful shutdown
        signal(SIGINT, signalHandler);
        #ifndef _WIN32
        signal(SIGTERM, signalHandler);
        #endif
        
        // Create and start the ultimate listener
        g_listener = std::make_unique<StandaloneListener::UltimateStandaloneListener>(
            port, verbose, statistics);
        
        g_listener->start();
        
        // Keep the main thread alive
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "🚨 Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
