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

