#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <winsock2.h>
#include <wininet.h>
#include <ctime>
#include <iomanip>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")

// Simple JSON structure for our lighthouse beacon
struct SimpleBeaconData {
    std::string beacon_id = "lighthouse-simple-001";
    long long timestamp;
    std::string status;
    std::string fastping_status;
    double latency_ms;
    int signal_age_seconds;
    
    // Simple JSON serialization (no RTC needed for this basic example)
    std::string to_json() const {
        std::ostringstream oss;
        oss << "{"
            << "\"beacon_id\":\"" << beacon_id << "\","
            << "\"timestamp\":" << timestamp << ","
            << "\"status\":\"" << status << "\","
            << "\"fastping_status\":\"" << fastping_status << "\","
            << "\"latency_ms\":" << latency_ms << ","
            << "\"signal_age_seconds\":" << signal_age_seconds
            << "}";
        return oss.str();
    }
    
    void print() const {
        auto time_t_val = static_cast<std::time_t>(timestamp);
        std::cout << "🚨 BEACON DATA:\n";
        std::cout << "   ID: " << beacon_id << "\n";
        std::cout << "   Status: " << status << "\n";
        std::cout << "   FastPing: " << fastping_status << "\n";
        std::cout << "   Latency: " << latency_ms << "ms\n";
        std::cout << "   Age: " << signal_age_seconds << "s\n";
        std::cout << "   Time: " << std::put_time(std::localtime(&time_t_val), "%H:%M:%S") << "\n\n";
    }
};

// ============================================================================
// SIMPLE BEACON BOT - Automated FastPing monitor and UDP broadcaster
// ============================================================================
class SimpleBeaconBot {
private:
    SOCKET udp_socket;
    sockaddr_in dest_addr;
    std::atomic<bool> running{true};
    SimpleBeaconData beacon_data;
    
    std::string target_ip = "161.35.248.233";  // Your beacon destination
    int target_port = 9876;
    int ping_interval_seconds = 10;
    int beacon_interval_seconds = 5;
    
public:
    SimpleBeaconBot() {
        init_networking();
        setup_socket();
        std::cout << "🏰 Simple Beacon Bot Initialized\n";
        std::cout << "   Target: " << target_ip << ":" << target_port << "\n";
        std::cout << "   Ping every " << ping_interval_seconds << "s\n";
        std::cout << "   Beacon every " << beacon_interval_seconds << "s\n\n";
    }
    
    ~SimpleBeaconBot() {
        running = false;
        if (udp_socket != INVALID_SOCKET) {
            closesocket(udp_socket);
        }
        WSACleanup();
    }
    
    void init_networking() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }
    
    void setup_socket() {
        udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (udp_socket == INVALID_SOCKET) {
            throw std::runtime_error("Socket creation failed");
        }
        
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(target_port);
        dest_addr.sin_addr.s_addr = inet_addr(target_ip.c_str());
    }
    
    // Simple HTTP request to fastping.it (no fancy optimization for this example)
    std::string fetch_fastping_simple() {
        HINTERNET hInternet = InternetOpenA("SimpleBeacon/1.0", 
                                          INTERNET_OPEN_TYPE_DIRECT, 
                                          NULL, NULL, 0);
        if (!hInternet) return "";
        
        HINTERNET hConnect = InternetOpenUrlA(hInternet, 
                                            "http://fastping.it.com/ping?format=text",
                                            NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return "";
        }
        
        std::string result;
        char buffer[1024];
        DWORD bytesRead;
        
        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            result.append(buffer, bytesRead);
        }
        
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return result;
    }
    
    // Parse simple text response from FastPing
    void parse_fastping_response(const std::string& response) {
        beacon_data.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        if (response.empty()) {
            beacon_data.fastping_status = "no_response";
            beacon_data.status = "warning";
            beacon_data.latency_ms = 0.0;
            beacon_data.signal_age_seconds = 999;
            return;
        }
        
        // Simple parsing of text response
        if (response.find("Status:") != std::string::npos) {
            if (response.find("ok") != std::string::npos || response.find("alive") != std::string::npos) {
                beacon_data.fastping_status = "ok";
                beacon_data.status = "healthy";
            } else {
                beacon_data.fastping_status = "unknown";
                beacon_data.status = "warning";
            }
        }
        
        // Extract latency if present (very simple parsing)
        size_t latency_pos = response.find("Response Time:");
        if (latency_pos != std::string::npos) {
            size_t ms_pos = response.find("ms", latency_pos);
            if (ms_pos != std::string::npos) {
                std::string latency_str = response.substr(latency_pos + 14, ms_pos - latency_pos - 14);
                try {
                    beacon_data.latency_ms = std::stod(latency_str);
                } catch (...) {
                    beacon_data.latency_ms = 0.0;
                }
            }
        }
        
        beacon_data.signal_age_seconds = 0; // Fresh signal
    }
    
    // FastPing monitoring thread
    void ping_thread() {
        std::cout << "🔍 FastPing monitor started\n";
        
        while (running) {
            auto start = std::chrono::high_resolution_clock::now();
            
            std::string response = fetch_fastping_simple();
            parse_fastping_response(response);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "📡 FastPing check: " << beacon_data.fastping_status 
                     << " (took " << duration.count() << "ms)\n";
            
            std::this_thread::sleep_for(std::chrono::seconds(ping_interval_seconds));
        }
    }
    
    // UDP beacon broadcasting thread
    void beacon_thread() {
        std::cout << "📻 Beacon broadcaster started\n";
        
        while (running) {
            // Update signal age
            auto now = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            beacon_data.signal_age_seconds = static_cast<int>(now - beacon_data.timestamp);
            
            // Create JSON payload
            std::string payload = beacon_data.to_json();
            
            // Broadcast
            int sent = sendto(udp_socket, payload.c_str(), payload.length(), 0, 
                            (sockaddr*)&dest_addr, sizeof(dest_addr));
            
            if (sent > 0) {
                std::cout << "🚨 Beacon sent: " << payload.length() << " bytes\n";
            } else {
                std::cout << "❌ Beacon failed to send\n";
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(beacon_interval_seconds));
        }
    }
    
    void run() {
        std::cout << "🏰 Starting Simple Beacon Bot...\n";
        
        std::thread ping_worker(&SimpleBeaconBot::ping_thread, this);
        std::thread beacon_worker(&SimpleBeaconBot::beacon_thread, this);
        
        std::cout << "Running! Press Enter to stop...\n";
        std::cin.get();
        
        running = false;
        ping_worker.join();
        beacon_worker.join();
        
        std::cout << "🏰 Simple Beacon Bot stopped.\n";
    }
};

// ============================================================================
// SIMPLE LISTENER BOT - UDP beacon receiver with live readout
// ============================================================================
class SimpleListenerBot {
private:
    SOCKET listen_socket;
    sockaddr_in listen_addr;
    std::atomic<bool> running{true};
    int listen_port;
    
    // Stats tracking
    size_t total_beacons_received = 0;
    size_t healthy_beacons = 0;
    std::chrono::system_clock::time_point start_time;
    
public:
    SimpleListenerBot(int port = 9876) : listen_port(port) {
        start_time = std::chrono::system_clock::now();
        init_networking();
        setup_socket();
        std::cout << "🎧 Simple Listener Bot Initialized\n";
        std::cout << "   Listening on port " << listen_port << "\n\n";
    }
    
    ~SimpleListenerBot() {
        running = false;
        if (listen_socket != INVALID_SOCKET) {
            closesocket(listen_socket);
        }
        WSACleanup();
    }
    
    void init_networking() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }
    
    void setup_socket() {
        listen_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (listen_socket == INVALID_SOCKET) {
            throw std::runtime_error("Socket creation failed");
        }
        
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_addr.s_addr = INADDR_ANY;
        listen_addr.sin_port = htons(listen_port);
        
        if (bind(listen_socket, (sockaddr*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR) {
            throw std::runtime_error("Bind failed");
        }
    }
    
    // Simple JSON parsing (just enough to extract key fields)
    SimpleBeaconData parse_beacon_json(const std::string& json) {
        SimpleBeaconData data;
        
        // Very simple JSON parsing (not production quality, but works for demo)
        auto extract_string = [&](const std::string& key) -> std::string {
            std::string search = "\"" + key + "\":\"";
            size_t pos = json.find(search);
            if (pos != std::string::npos) {
                pos += search.length();
                size_t end = json.find("\"", pos);
                if (end != std::string::npos) {
                    return json.substr(pos, end - pos);
                }
            }
            return "";
        };
        
        auto extract_number = [&](const std::string& key) -> double {
            std::string search = "\"" + key + "\":";
            size_t pos = json.find(search);
            if (pos != std::string::npos) {
                pos += search.length();
                size_t end = json.find_first_of(",}", pos);
                if (end != std::string::npos) {
                    try {
                        return std::stod(json.substr(pos, end - pos));
                    } catch (...) {}
                }
            }
            return 0.0;
        };
        
        data.beacon_id = extract_string("beacon_id");
        data.status = extract_string("status");
        data.fastping_status = extract_string("fastping_status");
        data.timestamp = static_cast<long long>(extract_number("timestamp"));
        data.latency_ms = extract_number("latency_ms");
        data.signal_age_seconds = static_cast<int>(extract_number("signal_age_seconds"));
        
        return data;
    }
    
    void display_beacon(const SimpleBeaconData& beacon, const std::string& sender_ip) {
        total_beacons_received++;
        if (beacon.status == "healthy") {
            healthy_beacons++;
        }
        
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::cout << "┌─────────────────────────────────────────┐\n";
        std::cout << "│ 🚨 LIGHTHOUSE BEACON #" << std::setw(4) << std::setfill('0') << total_beacons_received << "            │\n";
        std::cout << "├─────────────────────────────────────────┤\n";
        std::cout << "│ From: " << std::left << std::setw(30) << sender_ip << " │\n";
        std::cout << "│ ID: " << std::left << std::setw(32) << beacon.beacon_id << " │\n";
        std::cout << "│ Status: " << std::left << std::setw(28) << beacon.status << " │\n";
        std::cout << "│ FastPing: " << std::left << std::setw(26) << beacon.fastping_status << " │\n";
        std::cout << "│ Latency: " << std::left << std::setw(27) << (std::to_string(beacon.latency_ms) + "ms") << " │\n";
        std::cout << "│ Signal Age: " << std::left << std::setw(24) << (std::to_string(beacon.signal_age_seconds) + "s") << " │\n";
        std::cout << "│ Received: " << std::put_time(std::localtime(&time_t_now), "%H:%M:%S") << "                │\n";
        std::cout << "└─────────────────────────────────────────┘\n";
        
        // Health status
        if (beacon.status == "healthy" && beacon.signal_age_seconds < 60) {
            std::cout << "✅ HEALTHY LIGHTHOUSE SIGNAL\n\n";
        } else {
            std::cout << "⚠️  WARNING: Potential lighthouse issue\n\n";
        }
    }
    
    void display_stats() {
        auto now = std::chrono::system_clock::now();
        auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
        double healthy_rate = total_beacons_received > 0 ? 
            (100.0 * healthy_beacons / total_beacons_received) : 0.0;
        
        std::cout << "📊 LISTENER STATS:\n";
        std::cout << "   Uptime: " << uptime.count() << " seconds\n";
        std::cout << "   Total Beacons: " << total_beacons_received << "\n";
        std::cout << "   Healthy: " << healthy_beacons << " (" << std::fixed << std::setprecision(1) << healthy_rate << "%)\n";
        std::cout << "   Average Rate: " << std::fixed << std::setprecision(2) << (total_beacons_received / (uptime.count() + 1)) << " beacons/sec\n\n";
    }
    
    void listen() {
        std::cout << "🎧 Listening for lighthouse beacons...\n";
        std::cout << "Press Ctrl+C to stop\n\n";
        
        char buffer[4096];
        sockaddr_in sender_addr;
        int sender_len = sizeof(sender_addr);
        int stats_counter = 0;
        
        while (running) {
            int received = recvfrom(listen_socket, buffer, sizeof(buffer) - 1, 0,
                                  (sockaddr*)&sender_addr, &sender_len);
            
            if (received > 0) {
                buffer[received] = '\0';
                std::string json_data(buffer, received);
                std::string sender_ip = inet_ntoa(sender_addr.sin_addr);
                
                SimpleBeaconData beacon = parse_beacon_json(json_data);
                display_beacon(beacon, sender_ip);
                
                // Show stats every 10 beacons
                if (++stats_counter >= 10) {
                    display_stats();
                    stats_counter = 0;
                }
            }
        }
    }
    
    void run() {
        std::cout << "🏰 Starting Simple Listener Bot...\n";
        listen();
        std::cout << "🏰 Simple Listener Bot stopped.\n";
    }
};

// ============================================================================
// MAIN - Choose between beacon or listener mode
// ============================================================================
int main(int argc, char* argv[]) {
    std::cout << "🏰 SIMPLE LIGHTHOUSE BEACON/LISTENER BOTS\n";
    std::cout << "════════════════════════════════════════════\n\n";
    
    try {
        if (argc > 1 && std::string(argv[1]) == "beacon") {
            SimpleBeaconBot beacon;
            beacon.run();
        } else if (argc > 1 && std::string(argv[1]) == "listener") {
            int port = 9876;
            if (argc > 2) {
                port = std::atoi(argv[2]);
            }
            SimpleListenerBot listener(port);
            listener.run();
        } else {
            std::cout << "Usage:\n";
            std::cout << "  " << argv[0] << " beacon     - Start beacon bot\n";
            std::cout << "  " << argv[0] << " listener [port] - Start listener bot\n\n";
            
            std::cout << "Choose mode:\n";
            std::cout << "1. Beacon Bot (sends lighthouse signals)\n";
            std::cout << "2. Listener Bot (receives lighthouse signals)\n";
            std::cout << "Choice (1/2): ";
            
            int choice;
            std::cin >> choice;
            
            if (choice == 1) {
                SimpleBeaconBot beacon;
                beacon.run();
            } else if (choice == 2) {
                SimpleListenerBot listener;
                listener.run();
            } else {
                std::cout << "Invalid choice.\n";
                return 1;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
