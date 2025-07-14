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

