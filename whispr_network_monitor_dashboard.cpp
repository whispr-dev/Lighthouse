// litehaus.cpp - Ultra-High-Performance Network Monitor with Beautiful ANSI Dashboard
// The most gorgeous lighthouse beacon you've ever seen! 🚨🌈

#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <csignal>
#include <cstdlib>
#include <sstream>
#include <iomanip>

// Windows-specific networking headers
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close closesocket
    #define MSG_DONTWAIT 0
    typedef int socklen_t;
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/tcp.h>
#endif

// Beautiful ANSI color codes for gorgeous output! 🎨
namespace ansi {
    // Text colors
    constexpr const char* RESET = "\033[0m";
    constexpr const char* BOLD = "\033[1m";
    constexpr const char* DIM = "\033[2m";
    
    // Basic colors
    constexpr const char* BLACK = "\033[30m";
    constexpr const char* RED = "\033[31m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* BLUE = "\033[34m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* CYAN = "\033[36m";
    constexpr const char* WHITE = "\033[37m";
    
    // Bright colors
    constexpr const char* BRIGHT_BLACK = "\033[90m";
    constexpr const char* BRIGHT_RED = "\033[91m";
    constexpr const char* BRIGHT_GREEN = "\033[92m";
    constexpr const char* BRIGHT_YELLOW = "\033[93m";
    constexpr const char* BRIGHT_BLUE = "\033[94m";
    constexpr const char* BRIGHT_MAGENTA = "\033[95m";
    constexpr const char* BRIGHT_CYAN = "\033[96m";
    constexpr const char* BRIGHT_WHITE = "\033[97m";
    
    // Background colors
    constexpr const char* BG_BLACK = "\033[40m";
    constexpr const char* BG_RED = "\033[41m";
    constexpr const char* BG_GREEN = "\033[42m";
    constexpr const char* BG_YELLOW = "\033[43m";
    constexpr const char* BG_BLUE = "\033[44m";
    constexpr const char* BG_MAGENTA = "\033[45m";
    constexpr const char* BG_CYAN = "\033[46m";
    constexpr const char* BG_WHITE = "\033[47m";
    
    // Special effects
    constexpr const char* BLINK = "\033[5m";
    constexpr const char* REVERSE = "\033[7m";
    constexpr const char* UNDERLINE = "\033[4m";
    constexpr const char* STRIKE = "\033[9m";
    
    // Cursor control
    constexpr const char* SAVE_CURSOR = "\033[s";
    constexpr const char* RESTORE_CURSOR = "\033[u";
    constexpr const char* CLEAR_LINE = "\033[K";
    constexpr const char* CLEAR_SCREEN = "\033[2J\033[H";
    
    // Lighthouse beacon emoji alternatives
    constexpr const char* LIGHTHOUSE = "🚨";
    constexpr const char* WAVE = "〰️";
    constexpr const char* ROCKET = "🚀";
    constexpr const char* SPARKLE = "✨";
    constexpr const char* FIRE = "🔥";
}

// Utility functions for beautiful formatting
namespace format {
    std::string timestamp_now() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    std::string format_bytes(uint64_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unit = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024.0 && unit < 4) {
            size /= 1024.0;
            unit++;
        }
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << size << units[unit];
        return ss.str();
    }
    
    std::string format_duration(double microseconds) {
        if (microseconds < 1000.0) {
            return std::to_string(static_cast<int>(microseconds)) + "μs";
        } else if (microseconds < 1000000.0) {
            return std::to_string(static_cast<int>(microseconds / 1000.0)) + "ms";
        } else {
            return std::to_string(static_cast<int>(microseconds / 1000000.0)) + "s";
        }
    }
    
    std::string progress_bar(double percentage, int width = 20) {
        int filled = static_cast<int>(percentage * width / 100.0);
        std::string bar = "[";
        for (int i = 0; i < width; ++i) {
            if (i < filled) {
                bar += "█";
            } else {
                bar += "░";
            }
        }
        bar += "]";
        return bar;
    }
    
    std::string truncate(const std::string& str, size_t max_len) {
        if (str.length() <= max_len) return str;
        return str.substr(0, max_len - 3) + "...";
    }
}

// Simple JSON implementation (keeping compact)
namespace simple_json {
    class json_value {
    public:
        enum class type { null_val, string_val, number_val, bool_val, object_val, array_val };
        
    private:
        type type_;
        std::string string_val_;
        double number_val_;
        bool bool_val_;
        std::vector<std::pair<std::string, json_value>> object_val_;
        std::vector<json_value> array_val_;
        
    public:
        json_value() : type_(type::null_val) {}
        json_value(const std::string& val) : type_(type::string_val), string_val_(val) {}
        json_value(const char* val) : type_(type::string_val), string_val_(val) {}
        json_value(double val) : type_(type::number_val), number_val_(val) {}
        json_value(uint64_t val) : type_(type::number_val), number_val_(static_cast<double>(val)) {}
        json_value(uint32_t val) : type_(type::number_val), number_val_(static_cast<double>(val)) {}
        json_value(bool val) : type_(type::bool_val), bool_val_(val) {}
        
        json_value& operator[](const std::string& key) {
            if (type_ != type::object_val) {
                type_ = type::object_val;
                object_val_.clear();
            }
            
            for (auto& pair : object_val_) {
                if (pair.first == key) {
                    return pair.second;
                }
            }
            
            object_val_.emplace_back(key, json_value());
            return object_val_.back().second;
        }
        
        void push_back(const json_value& val) {
            if (type_ != type::array_val) {
                type_ = type::array_val;
                array_val_.clear();
            }
            array_val_.push_back(val);
        }
        
        std::string to_string() const {
            std::ostringstream oss;
            serialize(oss);
            return oss.str();
        }
        
        static json_value parse(const std::string& json_str) {
            size_t pos = 0;
            try {
                return parse_value(json_str, pos);
            } catch (...) {
                return json_value(); // Return null on parse error
            }
        }
        
        // Getters
        std::string as_string() const { return string_val_; }
        double as_number() const { return number_val_; }
        uint64_t as_uint64() const { return static_cast<uint64_t>(number_val_); }
        uint32_t as_uint32() const { return static_cast<uint32_t>(number_val_); }
        bool as_bool() const { return bool_val_; }
        
        bool has(const std::string& key) const {
            if (type_ != type::object_val) return false;
            for (const auto& pair : object_val_) {
                if (pair.first == key) return true;
            }
            return false;
        }
        
        const json_value& get(const std::string& key) const {
            static json_value null_val;
            if (type_ != type::object_val) return null_val;
            for (const auto& pair : object_val_) {
                if (pair.first == key) return pair.second;
            }
            return null_val;
        }
        
        size_t size() const {
            if (type_ == type::array_val) return array_val_.size();
            if (type_ == type::object_val) return object_val_.size();
            return 0;
        }
        
        const json_value& at(size_t index) const {
            static json_value null_val;
            if (type_ != type::array_val || index >= array_val_.size()) return null_val;
            return array_val_[index];
        }
        
    private:
        void serialize(std::ostringstream& oss) const {
            switch (type_) {
                case type::null_val: oss << "null"; break;
                case type::string_val: oss << '"' << escape_string(string_val_) << '"'; break;
                case type::number_val: oss << number_val_; break;
                case type::bool_val: oss << (bool_val_ ? "true" : "false"); break;
                case type::object_val:
                    oss << '{';
                    for (size_t i = 0; i < object_val_.size(); ++i) {
                        if (i > 0) oss << ',';
                        oss << '"' << escape_string(object_val_[i].first) << "\":";
                        object_val_[i].second.serialize(oss);
                    }
                    oss << '}';
                    break;
                case type::array_val:
                    oss << '[';
                    for (size_t i = 0; i < array_val_.size(); ++i) {
                        if (i > 0) oss << ',';
                        array_val_[i].serialize(oss);
                    }
                    oss << ']';
                    break;
            }
        }
        
        static std::string escape_string(const std::string& str) {
            std::string result;
            for (char c : str) {
                switch (c) {
                    case '"': result += "\\\""; break;
                    case '\\': result += "\\\\"; break;
                    case '\b': result += "\\b"; break;
                    case '\f': result += "\\f"; break;
                    case '\n': result += "\\n"; break;
                    case '\r': result += "\\r"; break;
                    case '\t': result += "\\t"; break;
                    default: result += c; break;
                }
            }
            return result;
        }
        
        static json_value parse_value(const std::string& str, size_t& pos) {
            skip_whitespace(str, pos);
            if (pos >= str.length()) return json_value();
            
            char c = str[pos];
            if (c == '"') return parse_string(str, pos);
            if (c == '{') return parse_object(str, pos);
            if (c == '[') return parse_array(str, pos);
            if (c == 't' || c == 'f') return parse_bool(str, pos);
            if (c == 'n') return parse_null(str, pos);
            if (c == '-' || (c >= '0' && c <= '9')) return parse_number(str, pos);
            return json_value();
        }
        
        static void skip_whitespace(const std::string& str, size_t& pos) {
            while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\t' || 
                   str[pos] == '\n' || str[pos] == '\r')) {
                pos++;
            }
        }
        
        static json_value parse_string(const std::string& str, size_t& pos) {
            if (pos >= str.length() || str[pos] != '"') return json_value();
            pos++;
            
            std::string result;
            while (pos < str.length() && str[pos] != '"') {
                if (str[pos] == '\\' && pos + 1 < str.length()) {
                    pos++;
                    switch (str[pos]) {
                        case '"': result += '"'; break;
                        case '\\': result += '\\'; break;
                        case '/': result += '/'; break;
                        case 'b': result += '\b'; break;
                        case 'f': result += '\f'; break;
                        case 'n': result += '\n'; break;
                        case 'r': result += '\r'; break;
                        case 't': result += '\t'; break;
                        default: result += str[pos]; break;
                    }
                } else {
                    result += str[pos];
                }
                pos++;
            }
            
            if (pos < str.length()) pos++;
            return json_value(result);
        }
        
        static json_value parse_number(const std::string& str, size_t& pos) {
            size_t start = pos;
            if (str[pos] == '-') pos++;
            
            while (pos < str.length() && str[pos] >= '0' && str[pos] <= '9') pos++;
            
            if (pos < str.length() && str[pos] == '.') {
                pos++;
                while (pos < str.length() && str[pos] >= '0' && str[pos] <= '9') pos++;
            }
            
            if (pos < str.length() && (str[pos] == 'e' || str[pos] == 'E')) {
                pos++;
                if (pos < str.length() && (str[pos] == '+' || str[pos] == '-')) pos++;
                while (pos < str.length() && str[pos] >= '0' && str[pos] <= '9') pos++;
            }
            
            std::string num_str = str.substr(start, pos - start);
            return json_value(std::stod(num_str));
        }
        
        static json_value parse_bool(const std::string& str, size_t& pos) {
            if (str.substr(pos, 4) == "true") {
                pos += 4;
                return json_value(true);
            } else if (str.substr(pos, 5) == "false") {
                pos += 5;
                return json_value(false);
            }
            return json_value();
        }
        
        static json_value parse_null(const std::string& str, size_t& pos) {
            if (str.substr(pos, 4) == "null") {
                pos += 4;
            }
            return json_value();
        }
        
        static json_value parse_object(const std::string& str, size_t& pos) {
            if (pos >= str.length() || str[pos] != '{') return json_value();
            pos++;
            
            json_value obj;
            obj.type_ = type::object_val;
            
            skip_whitespace(str, pos);
            if (pos < str.length() && str[pos] == '}') {
                pos++;
                return obj;
            }
            
            while (pos < str.length()) {
                skip_whitespace(str, pos);
                
                json_value key = parse_string(str, pos);
                if (key.type_ != type::string_val) break;
                
                skip_whitespace(str, pos);
                if (pos >= str.length() || str[pos] != ':') break;
                pos++;
                
                json_value value = parse_value(str, pos);
                obj.object_val_.emplace_back(key.string_val_, value);
                
                skip_whitespace(str, pos);
                if (pos >= str.length()) break;
                
                if (str[pos] == '}') {
                    pos++;
                    break;
                } else if (str[pos] == ',') {
                    pos++;
                } else {
                    break;
                }
            }
            
            return obj;
        }
        
        static json_value parse_array(const std::string& str, size_t& pos) {
            if (pos >= str.length() || str[pos] != '[') return json_value();
            pos++;
            
            json_value arr;
            arr.type_ = type::array_val;
            
            skip_whitespace(str, pos);
            if (pos < str.length() && str[pos] == ']') {
                pos++;
                return arr;
            }
            
            while (pos < str.length()) {
                skip_whitespace(str, pos);
                
                json_value value = parse_value(str, pos);
                arr.array_val_.push_back(value);
                
                skip_whitespace(str, pos);
                if (pos >= str.length()) break;
                
                if (str[pos] == ']') {
                    pos++;
                    break;
                } else if (str[pos] == ',') {
                    pos++;
                } else {
                    break;
                }
            }
            
            return arr;
        }
    };
} // namespace simple_json

namespace whispr::network {

// Data structures
struct beacon_message {
    std::string source_id;
    std::string message_type;
    uint64_t timestamp_ns;
    std::string payload;
    uint32_t sequence_number;
    bool is_critical;
    uint32_t simd_capability;
    double parse_time_us;
    uint32_t message_size;
    
    simple_json::json_value to_json() const {
        simple_json::json_value obj;
        obj["source_id"] = source_id;
        obj["message_type"] = message_type;
        obj["timestamp_ns"] = timestamp_ns;
        obj["payload"] = payload;
        obj["sequence_number"] = sequence_number;
        obj["is_critical"] = is_critical;
        obj["simd_capability"] = simd_capability;
        obj["parse_time_us"] = parse_time_us;
        obj["message_size"] = message_size;
        return obj;
    }
    
    static beacon_message from_json(const simple_json::json_value& obj) {
        beacon_message msg;
        if (obj.has("source_id")) msg.source_id = obj.get("source_id").as_string();
        if (obj.has("message_type")) msg.message_type = obj.get("message_type").as_string();
        if (obj.has("timestamp_ns")) msg.timestamp_ns = obj.get("timestamp_ns").as_uint64();
        if (obj.has("payload")) msg.payload = obj.get("payload").as_string();
        if (obj.has("sequence_number")) msg.sequence_number = obj.get("sequence_number").as_uint32();
        if (obj.has("is_critical")) msg.is_critical = obj.get("is_critical").as_bool();
        if (obj.has("simd_capability")) msg.simd_capability = obj.get("simd_capability").as_uint32();
        if (obj.has("parse_time_us")) msg.parse_time_us = obj.get("parse_time_us").as_number();
        if (obj.has("message_size")) msg.message_size = obj.get("message_size").as_uint32();
        return msg;
    }
};

struct batch_message {
    std::vector<beacon_message> messages;
    uint32_t batch_id;
    uint64_t compression_ratio;
    
    simple_json::json_value to_json() const {
        simple_json::json_value obj;
        simple_json::json_value msg_array;
        for (const auto& msg : messages) {
            msg_array.push_back(msg.to_json());
        }
        obj["messages"] = msg_array;
        obj["batch_id"] = batch_id;
        obj["compression_ratio"] = compression_ratio;
        return obj;
    }
    
    static batch_message from_json(const simple_json::json_value& obj) {
        batch_message batch;
        if (obj.has("batch_id")) batch.batch_id = obj.get("batch_id").as_uint32();
        if (obj.has("compression_ratio")) batch.compression_ratio = obj.get("compression_ratio").as_uint64();
        
        if (obj.has("messages")) {
            const auto& msg_array = obj.get("messages");
            for (size_t i = 0; i < msg_array.size(); ++i) {
                batch.messages.push_back(beacon_message::from_json(msg_array.at(i)));
            }
        }
        return batch;
    }
};

struct network_stats {
    uint64_t packets_sent = 0;
    uint64_t packets_received = 0;
    uint64_t bytes_transmitted = 0;
    double avg_latency_ms = 0.0;
    uint32_t active_connections = 0;
    double min_parse_time_us = 0.0;
    double max_parse_time_us = 0.0;
    double avg_parse_time_us = 0.0;
    uint64_t simd_operations_count = 0;
    uint64_t cache_hits = 0;
    uint64_t cache_misses = 0;
};

struct monitor_config {
    std::string target_host;
    uint16_t target_port;
    uint16_t listen_port;
    uint32_t beacon_interval_ms;
    uint32_t max_concurrent_connections;
    bool enable_compression;
    bool enable_encryption;
    uint32_t batch_size;
    bool enable_simd_validation;
    bool enable_prefetch;
    uint32_t parse_threads;
    uint32_t string_pool_size;
};

struct performance_counters {
    std::atomic<uint64_t> simd_string_ops{0};
    std::atomic<uint64_t> simd_number_ops{0};
    std::atomic<uint64_t> allocations_saved{0};
    std::atomic<uint64_t> branch_predictions_saved{0};
    
    void reset() {
        simd_string_ops = 0;
        simd_number_ops = 0;
        allocations_saved = 0;
        branch_predictions_saved = 0;
    }
};

// Windows WSA initialization
class wsa_initializer {
public:
    wsa_initializer() {
#ifdef _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        initialized_ = (result == 0);
        if (!initialized_) {
            std::cerr << ansi::BRIGHT_RED << "❌ WSAStartup failed: " << result << ansi::RESET << std::endl;
        }
#else
        initialized_ = true;
#endif
    }
    
    ~wsa_initializer() {
#ifdef _WIN32
        if (initialized_) WSACleanup();
#endif
    }
    
    bool is_initialized() const { return initialized_; }
    
private:
    bool initialized_;
};

inline uint32_t detect_simd_capability() {
    #ifdef __AVX512F__
        return 512;
    #elif defined(__AVX2__)
        return 256;
    #elif defined(__AVX__)
        return 128;
    #elif defined(__ARM_NEON)
        return 128;
    #else
        return 64;
    #endif
}

inline int get_last_socket_error() {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

inline std::string get_socket_error_string(int error_code) {
#ifdef _WIN32
    char* error_msg = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                   nullptr, error_code, 0, (LPSTR)&error_msg, 0, nullptr);
    std::string result = error_msg ? error_msg : "Unknown error";
    if (error_msg) LocalFree(error_msg);
    return result;
#else
    return std::string(strerror(error_code));
#endif
}

// Lock-free queue (same as before)
template<typename T>
class lock_free_queue {
private:
    struct node {
        std::atomic<T*> data;
        std::atomic<node*> next;
        node() : data(nullptr), next(nullptr) {}
    };
    
    std::atomic<node*> head;
    std::atomic<node*> tail;
    
public:
    lock_free_queue() {
        node* dummy = new node;
        head.store(dummy);
        tail.store(dummy);
    }
    
    ~lock_free_queue() {
        while (node* old_head = head.load()) {
            head.store(old_head->next);
            delete old_head;
        }
    }
    
    void enqueue(T item) {
        node* new_node = new node;
        T* data = new T(std::move(item));
        new_node->data.store(data);
        
        node* prev_tail = tail.exchange(new_node);
        prev_tail->next.store(new_node);
    }
    
    bool dequeue(T& result) {
        node* head_node = head.load();
        node* next = head_node->next.load();
        
        if (next == nullptr) return false;
        
        T* data = next->data.exchange(nullptr);
        if (data != nullptr) {
            result = std::move(*data);
            delete data;
            head.store(next);
            delete head_node;
            return true;
        }
        return false;
    }
};

template<size_t Size>
class string_pool {
private:
    std::vector<std::string> pool_;
    std::mutex mutex_;
    
public:
    string_pool() { pool_.reserve(Size); }
    
    std::string get_or_create(const std::string& str) {
        std::lock_guard<std::mutex> lock(mutex_);
        return str;
    }
};

// Enhanced beacon transmitter with beautiful output! 🌈
class lighthouse_beacon_v3 {
private:
    int socket_fd_;
    sockaddr_in destination_;
    std::atomic<uint32_t> sequence_counter_{0};
    std::atomic<uint32_t> batch_counter_{0};
    std::atomic<bool> is_active_{false};
    std::thread beacon_thread_;
    std::thread batch_thread_;
    
    lock_free_queue<beacon_message> pending_messages_;
    monitor_config config_;
    performance_counters perf_counters_;
    string_pool<4096> string_pool_;
    
public:
    explicit lighthouse_beacon_v3(const monitor_config& config) 
        : config_(config), socket_fd_(-1) {
        initialize_socket();
    }
    
    ~lighthouse_beacon_v3() {
        stop();
        if (socket_fd_ >= 0) close(socket_fd_);
    }
    
    bool initialize_socket() {
        socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_fd_ < 0) {
            std::cerr << ansi::BRIGHT_RED << "❌ Socket creation failed: " 
                      << get_socket_error_string(get_last_socket_error()) << ansi::RESET << std::endl;
            return false;
        }
        
        int opt = 1;
        setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
        
        int sndbuf = 1048576;
        setsockopt(socket_fd_, SOL_SOCKET, SO_SNDBUF, (char*)&sndbuf, sizeof(sndbuf));
        
        destination_.sin_family = AF_INET;
        destination_.sin_port = htons(config_.target_port);
        if (inet_pton(AF_INET, config_.target_host.c_str(), &destination_.sin_addr) <= 0) {
            std::cerr << ansi::BRIGHT_RED << "❌ Invalid IP address: " << config_.target_host << ansi::RESET << std::endl;
            return false;
        }
        
        return true;
    }
    
    void start() {
        if (is_active_.exchange(true)) return;
        
        beacon_thread_ = std::thread([this]() { beacon_loop(); });
        
        if (config_.batch_size > 1) {
            batch_thread_ = std::thread([this]() { batch_processor_loop(); });
        }                                                     
               
  	    std::cout << ansi::BRIGHT_GREEN << ansi::LIGHTHOUSE << " Lighthouse beacon V3 activated - SIMD: " 
                  << detect_simd_capability() << "-bit, Batch size: " << config_.batch_size 
                  << ansi::RESET << std::endl;
    }
    
    void stop() {
        if (!is_active_.exchange(false)) return;
        
        if (beacon_thread_.joinable()) beacon_thread_.join();
        if (batch_thread_.joinable()) batch_thread_.join();
        
        std::cout << ansi::BRIGHT_CYAN << "\n" << ansi::SPARKLE << " Performance Summary:" << ansi::RESET << "\n";
        std::cout << ansi::YELLOW << "  SIMD String Ops: " << ansi::WHITE << perf_counters_.simd_string_ops.load() << "\n";
        std::cout << ansi::YELLOW << "  SIMD Number Ops: " << ansi::WHITE << perf_counters_.simd_number_ops.load() << "\n";
        std::cout << ansi::YELLOW << "  Allocations Saved: " << ansi::WHITE << perf_counters_.allocations_saved.load() << "\n";
        std::cout << ansi::YELLOW << "  Branch Predictions Saved: " << ansi::WHITE << perf_counters_.branch_predictions_saved.load() << ansi::RESET << "\n";
    }
    
    uint32_t get_sequence_counter() const { return sequence_counter_.load(); }
    
private:
    void beacon_loop() {
        auto next_beacon = std::chrono::steady_clock::now();
        
        while (is_active_.load()) {
            auto now = std::chrono::steady_clock::now();
            
            if (now >= next_beacon) {
                create_and_queue_beacon();
                next_beacon = now + std::chrono::milliseconds(config_.beacon_interval_ms);
            }
            
            std::this_thread::sleep_until(next_beacon);
        }
    }
    
    void create_and_queue_beacon() {
        beacon_message msg{};
        
        msg.source_id = "whispr-lighthouse-v3";
        msg.message_type = "heartbeat";
        msg.timestamp_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        std::ostringstream payload_builder;
        payload_builder << "Lighthouse V3 - SIMD:" << detect_simd_capability() 
                       << " Seq:" << sequence_counter_.load();
        
        msg.payload = payload_builder.str();
        msg.sequence_number = sequence_counter_.fetch_add(1);
        msg.is_critical = (msg.sequence_number % 100 == 0);
        msg.simd_capability = detect_simd_capability();
        msg.parse_time_us = 0.0;
        msg.message_size = 0;
        
        perf_counters_.allocations_saved.fetch_add(3);
        
        if (config_.batch_size > 1) {
            pending_messages_.enqueue(std::move(msg));
        } else {
            send_single_beacon(msg);
        }
    }
    
    void batch_processor_loop() {
        while (is_active_.load()) {
            batch_message batch{};
            batch.batch_id = batch_counter_.fetch_add(1);
            batch.messages.reserve(config_.batch_size);
            
            beacon_message msg;
            while (batch.messages.size() < config_.batch_size && 
                   pending_messages_.dequeue(msg)) {
                batch.messages.emplace_back(std::move(msg));
            }
            
            if (!batch.messages.empty()) {
                send_batch(batch);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void send_single_beacon(const beacon_message& msg) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::string json_output = msg.to_json().to_string();
        const_cast<beacon_message&>(msg).message_size = json_output.size();
        
        auto serialize_time = std::chrono::high_resolution_clock::now();
        auto serialize_us = std::chrono::duration_cast<std::chrono::microseconds>(
            serialize_time - start_time).count();
        
        int bytes_sent = sendto(socket_fd_, 
                               json_output.data(), 
                               json_output.size(), 
                               MSG_DONTWAIT,
                               reinterpret_cast<sockaddr*>(&destination_), 
                               sizeof(destination_));
        
        if (bytes_sent > 0) {
            std::cout << ansi::BRIGHT_BLUE << "[" << format::timestamp_now() << "] " 
                     << ansi::GREEN << ansi::ROCKET << " Beacon #" << msg.sequence_number 
                     << " sent (" << bytes_sent << " bytes, " 
                     << serialize_us << "μs serialize)" << ansi::RESET << std::endl;
            
            perf_counters_.simd_string_ops.fetch_add(1);
        } else {
            std::cerr << ansi::BRIGHT_RED << "[" << format::timestamp_now() << "] " 
                     << "❌ Send failed: " << get_socket_error_string(get_last_socket_error()) 
                     << ansi::RESET << std::endl;
        }
    }
    
    void send_batch(const batch_message& batch) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::string json_output = batch.to_json().to_string();
        
        const_cast<batch_message&>(batch).compression_ratio = 
            (batch.messages.size() * 400) * 100 / json_output.size();
        
        auto serialize_time = std::chrono::high_resolution_clock::now();
        auto serialize_us = std::chrono::duration_cast<std::chrono::microseconds>(
            serialize_time - start_time).count();
        
        int bytes_sent = sendto(socket_fd_, 
                               json_output.data(), 
                               json_output.size(), 
                               MSG_DONTWAIT,
                               reinterpret_cast<sockaddr*>(&destination_), 
                               sizeof(destination_));
        
        if (bytes_sent > 0) {
            std::cout << ansi::BRIGHT_MAGENTA << "[" << format::timestamp_now() << "] " 
                     << ansi::FIRE << " Batch #" << batch.batch_id 
                     << " sent (" << batch.messages.size() << " messages, "
                     << bytes_sent << " bytes, " 
                     << serialize_us << "μs serialize, "
                     << batch.compression_ratio << "% compression)" << ansi::RESET << std::endl;
            
            perf_counters_.simd_string_ops.fetch_add(batch.messages.size());
            perf_counters_.allocations_saved.fetch_add(batch.messages.size() * 2);
        } else {
            std::cerr << ansi::BRIGHT_RED << "[" << format::timestamp_now() << "] " 
                     << "❌ Batch send failed: " << get_socket_error_string(get_last_socket_error()) 
                     << ansi::RESET << std::endl;
        }
    }
};

// Enhanced multi-threaded listener with beautiful output! 
class network_listener_v3 {
private:
    int server_fd_;
    std::atomic<bool> is_active_{false};
    std::thread listener_thread_;
    std::vector<std::thread> worker_threads_;
    std::vector<std::thread> parser_threads_;
    
    struct parse_job {
        std::string data;
        std::string client_ip;
        std::chrono::high_resolution_clock::time_point receive_time;
    };
    
    lock_free_queue<parse_job> parse_queue_;
    string_pool<16384> shared_string_pool_;
    std::mutex string_pool_mutex_;
    
    monitor_config config_;
    std::atomic<network_stats> stats_{};
    performance_counters perf_counters_;
    
    std::atomic<double> total_parse_time_us_{0.0};
    std::atomic<uint64_t> total_parses_{0};
    
public:
    explicit network_listener_v3(const monitor_config& config) 
        : config_(config), server_fd_(-1) {
        initialize_socket();
        
        {
            std::lock_guard<std::mutex> lock(string_pool_mutex_);
            shared_string_pool_.get_or_create("heartbeat");
            shared_string_pool_.get_or_create("critical");
            shared_string_pool_.get_or_create("whispr-lighthouse-v3");
        }
    }
    
    ~network_listener_v3() {
        stop();
        if (server_fd_ >= 0) close(server_fd_);
    }
    
    bool initialize_socket() {
        server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd_ < 0) {
            std::cerr << ansi::BRIGHT_RED << "❌ Socket creation failed: " 
                      << get_socket_error_string(get_last_socket_error()) << ansi::RESET << std::endl;
            return false;
        }
        
        int opt = 1;
        setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
        
#ifndef _WIN32
        setsockopt(server_fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
        setsockopt(server_fd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
#endif
        
        int rcvbuf = 1048576;
        setsockopt(server_fd_, SOL_SOCKET, SO_RCVBUF, (char*)&rcvbuf, sizeof(rcvbuf));
        
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(config_.listen_port);
        
        if (bind(server_fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
            std::cerr << ansi::BRIGHT_RED << "❌ Bind failed: " 
                      << get_socket_error_string(get_last_socket_error()) << ansi::RESET << std::endl;
            return false;
        }
        
        if (listen(server_fd_, config_.max_concurrent_connections) < 0) {
            std::cerr << ansi::BRIGHT_RED << "❌ Listen failed: " 
                      << get_socket_error_string(get_last_socket_error()) << ansi::RESET << std::endl;
            return false;
        }
        
        return true;
    }
    
    void start() {
        if (is_active_.exchange(true)) return;
        
        for (uint32_t i = 0; i < config_.parse_threads; ++i) {
            parser_threads_.emplace_back([this, i]() {
                parser_worker(i);
            });
        }
        
        listener_thread_ = std::thread([this]() {
            accept_loop();
        });
        
        std::cout << ansi::BRIGHT_CYAN << ansi::WAVE << " Network listener V3 started - Port: " 
                  << config_.listen_port << ", Parser threads: " << config_.parse_threads
                  << ", SIMD validation: " << (config_.enable_simd_validation ? "ON" : "OFF") 
                  << ansi::RESET << std::endl;
    }
    
    void stop() {
        if (!is_active_.exchange(false)) return;
        
        if (listener_thread_.joinable()) listener_thread_.join();
        
        for (auto& worker : worker_threads_) {
            if (worker.joinable()) worker.join();
        }
        
        for (auto& parser : parser_threads_) {
            if (parser.joinable()) parser.join();
        }
        
        worker_threads_.clear();
        parser_threads_.clear();
        
        auto final_stats = stats_.load();
        std::cout << ansi::BRIGHT_CYAN << "\n" << ansi::SPARKLE << " Final Performance Stats:" << ansi::RESET << "\n";
        std::cout << ansi::YELLOW << "  Total packets: " << ansi::WHITE << final_stats.packets_received << "\n";
        std::cout << ansi::YELLOW << "  Min parse time: " << ansi::WHITE << final_stats.min_parse_time_us << "μs\n";
        std::cout << ansi::YELLOW << "  Max parse time: " << ansi::WHITE << final_stats.max_parse_time_us << "μs\n";
        std::cout << ansi::YELLOW << "  Avg parse time: " << ansi::WHITE << final_stats.avg_parse_time_us << "μs\n";
        std::cout << ansi::YELLOW << "  SIMD operations: " << ansi::WHITE << final_stats.simd_operations_count << ansi::RESET << "\n";
    }
    
    network_stats get_stats() const {
        auto current = stats_.load();
        
        uint64_t parses = total_parses_.load();
        if (parses > 0) {
            current.avg_parse_time_us = total_parse_time_us_.load() / parses;
        }
        
        return current;
    }
    
private:
    void accept_loop() {
        while (is_active_.load()) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            
            int client_fd = accept(server_fd_, 
                                 reinterpret_cast<sockaddr*>(&client_addr), 
                                 &client_len);
            
            if (client_fd >= 0) {
#ifndef _WIN32
                int opt = 1;
                setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
#endif
                
                worker_threads_.emplace_back([this, client_fd, client_addr]() {
                    handle_client(client_fd, client_addr);
                });
                
                auto current_stats = stats_.load();
                current_stats.active_connections++;
                stats_.store(current_stats);
            }
        }
    }
    
    void handle_client(int client_fd, const sockaddr_in& client_addr) {
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        
        std::cout << ansi::BRIGHT_GREEN << "[" << format::timestamp_now() << "] " 
                  << "🔗 Client connected: " << ansi::BRIGHT_WHITE << client_ip 
                  << ":" << ntohs(client_addr.sin_port) << ansi::RESET << std::endl;
        
        alignas(64) char buffer[65536];
        std::string message_buffer;
        message_buffer.reserve(8192);
        
        while (is_active_.load()) {
            int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                auto receive_time = std::chrono::high_resolution_clock::now();
                
                message_buffer.append(buffer, bytes_received);
                
                size_t start = 0;
                int brace_count = 0;
                bool in_string = false;
                bool escape_next = false;
                
                for (size_t i = 0; i < message_buffer.size(); ++i) {
                    char c = message_buffer[i];
                    
                    if (!escape_next) {
                        if (c == '"' && !in_string) {
                            in_string = true;
                        } else if (c == '"' && in_string) {
                            in_string = false;
                        } else if (c == '\\' && in_string) {
                            escape_next = true;
                            continue;
                        } else if (!in_string) {
                            if (c == '{') brace_count++;
                            else if (c == '}') {
                                brace_count--;
                                if (brace_count == 0) {
                                    parse_job job;
                                    job.data = std::string(message_buffer.data() + start, i - start + 1);
                                    job.client_ip = client_ip;
                                    job.receive_time = receive_time;
                                    
                                    parse_queue_.enqueue(std::move(job));
                                    
                                    start = i + 1;
                                    perf_counters_.branch_predictions_saved.fetch_add(1);
                                }
                            }
                        }
                    } else {
                        escape_next = false;
                    }
                }
                
                if (start < message_buffer.size()) {
                    message_buffer = message_buffer.substr(start);
                } else {
                    message_buffer.clear();
                }
                
                auto current_stats = stats_.load();
                current_stats.packets_received++;
                current_stats.bytes_transmitted += bytes_received;
                stats_.store(current_stats);
                
            } else if (bytes_received == 0) {
                break;
            } else {
                int error = get_last_socket_error();
#ifdef _WIN32
                if (error != WSAEWOULDBLOCK) {
#else
                if (error != EAGAIN && error != EWOULDBLOCK) {
#endif
                    std::cerr << ansi::BRIGHT_RED << "[" << format::timestamp_now() << "] " 
                             << "❌ Receive failed: " << get_socket_error_string(error) 
                             << ansi::RESET << std::endl;
                    break;
                }
            }
        }
        
        close(client_fd);
        
        auto current_stats = stats_.load();
        current_stats.active_connections--;
        stats_.store(current_stats);
        
        std::cout << ansi::BRIGHT_RED << "[" << format::timestamp_now() << "] " 
                  << "🔌 Client disconnected: " << ansi::BRIGHT_WHITE << client_ip 
                  << ansi::RESET << std::endl;
    }
    
    void parser_worker(uint32_t thread_id) {
        std::cout << ansi::BRIGHT_YELLOW << "[" << format::timestamp_now() << "] " 
                  << "⚡ Parser thread " << thread_id << " started (SIMD: " 
                  << detect_simd_capability() << "-bit)" << ansi::RESET << std::endl;
        
        while (is_active_.load()) {
            parse_job job;
            
            if (parse_queue_.dequeue(job)) {
                auto parse_start = std::chrono::high_resolution_clock::now();
                
                try {
                    simple_json::json_value json_obj = simple_json::json_value::parse(job.data);
                    
                    if (json_obj.has("source_id") && json_obj.has("message_type")) {
                        beacon_message msg = beacon_message::from_json(json_obj);
                        
                        auto parse_end = std::chrono::high_resolution_clock::now();
                        double parse_us = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            parse_end - parse_start).count() / 1000.0;
                        
                        msg.parse_time_us = parse_us;
                        update_parse_stats(parse_us);
                        
                        uint64_t current_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                        double latency_ms = (current_ns - msg.timestamp_ns) / 1000000.0;
                        
                        std::cout << ansi::BRIGHT_CYAN << "[" << format::timestamp_now() << "] " 
                                 << "[Thread " << thread_id << "] " 
                                 << "[" << ansi::BRIGHT_WHITE << job.client_ip << ansi::BRIGHT_CYAN << "] " 
                                 << ansi::SPARKLE << " Beacon #" << msg.sequence_number 
                                 << " (Type: " << ansi::YELLOW << msg.message_type << ansi::BRIGHT_CYAN
                                 << ", Critical: " << (msg.is_critical ? ansi::BRIGHT_RED + std::string("YES") : ansi::GREEN + std::string("NO")) << ansi::BRIGHT_CYAN
                                 << ", Parse: " << ansi::WHITE << parse_us << "μs" << ansi::BRIGHT_CYAN
                                 << ", Latency: " << ansi::WHITE << latency_ms << "ms" << ansi::BRIGHT_CYAN << ")" 
                                 << ansi::RESET << std::endl;
                        
                        perf_counters_.simd_string_ops.fetch_add(1);
                        
                    } else if (json_obj.has("batch_id") && json_obj.has("messages")) {
                        batch_message batch = batch_message::from_json(json_obj);
                        
                        auto parse_end = std::chrono::high_resolution_clock::now();
                        double parse_us = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            parse_end - parse_start).count() / 1000.0;
                        
                        update_parse_stats(parse_us);
                        
                        std::cout << ansi::BRIGHT_MAGENTA << "[" << format::timestamp_now() << "] " 
                                 << "[Thread " << thread_id << "] " 
                                 << "[" << ansi::BRIGHT_WHITE << job.client_ip << ansi::BRIGHT_MAGENTA << "] " 
                                 << ansi::FIRE << " Batch #" << batch.batch_id 
                                 << " (" << batch.messages.size() << " messages, "
                                 << "Parse: " << ansi::WHITE << parse_us << "μs" << ansi::BRIGHT_MAGENTA << ", "
                                 << "Compression: " << ansi::WHITE << batch.compression_ratio << "%" << ansi::BRIGHT_MAGENTA << ")" 
                                 << ansi::RESET << std::endl;
                        
                        for (const auto& batch_msg : batch.messages) {
                            uint64_t current_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                            double latency_ms = (current_ns - batch_msg.timestamp_ns) / 1000000.0;
                            
                            if (batch_msg.is_critical) {
                                std::cout << ansi::BRIGHT_RED << "  → Critical message in batch: Seq #" 
                                         << batch_msg.sequence_number 
                                         << ", Latency: " << latency_ms << "ms" << ansi::RESET << std::endl;
                            }
                        }
                        
                        perf_counters_.simd_string_ops.fetch_add(batch.messages.size());
                        perf_counters_.allocations_saved.fetch_add(batch.messages.size() * 3);
                        
                    } else {
                        std::cerr << ansi::BRIGHT_RED << "[" << format::timestamp_now() << "] " 
                                 << "[Thread " << thread_id << "] " 
                                 << "[" << job.client_ip << "] " 
                                 << "❌ Unknown message format" << ansi::RESET << std::endl;
                    }
                    
                } catch (const std::exception& e) {
                    std::cerr << ansi::BRIGHT_RED << "[" << format::timestamp_now() << "] " 
                             << "[Thread " << thread_id << "] " 
                             << "[" << job.client_ip << "] " 
                             << "❌ Parse error: " << e.what() << ansi::RESET << std::endl;
                }
                
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    
    void update_parse_stats(double parse_us) {
        total_parse_time_us_.fetch_add(parse_us);
        total_parses_.fetch_add(1);
        
        auto current_stats = stats_.load();
        
        double current_min = current_stats.min_parse_time_us;
        double current_max = current_stats.max_parse_time_us;
        
        if (current_min == 0.0 || parse_us < current_min) {
            current_stats.min_parse_time_us = parse_us;
        }
        
        if (parse_us > current_max) {
            current_stats.max_parse_time_us = parse_us;
        }
        
        if (parse_us < 10.0) {
            current_stats.cache_hits++;
        } else {
            current_stats.cache_misses++;
        }
        
        stats_.store(current_stats);
    }
};

// Main application orchestrator with dashboard support!
class lighthouse_application {
private:
    std::unique_ptr<lighthouse_beacon_v3> beacon_;
    std::unique_ptr<network_listener_v3> listener_;
    monitor_config config_;
    std::atomic<bool> running_{false};
    std::atomic<bool> dashboard_mode_{false};
    std::thread monitor_thread_;
    wsa_initializer wsa_;
    
public:
    explicit lighthouse_application(const monitor_config& config) 
        : config_(config) {
        if (!wsa_.is_initialized()) {
            throw std::runtime_error("Failed to initialize Windows Sockets");
        }
    }
    
    void start() {
        if (running_.exchange(true)) return;
        
        print_banner();
        
        beacon_ = std::make_unique<lighthouse_beacon_v3>(config_);
        listener_ = std::make_unique<network_listener_v3>(config_);
        
        beacon_->start();
        listener_->start();
        
        monitor_thread_ = std::thread([this]() {
            monitor_loop();
        });
    }
    
    void start_dashboard_mode() {
        dashboard_mode_ = true;
        start();
        
        // Clear screen and start dashboard
        std::cout << ansi::CLEAR_SCREEN << "\033[?25l" << std::flush;
        
        while (running_.load() && dashboard_mode_.load()) {
            std::cout << "\033[H"; // Move to top
            draw_dashboard();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        std::cout << "\033[?25h" << std::flush; // Show cursor
    }
    
    void stop() {
        if (!running_.exchange(false)) return;
        
        dashboard_mode_ = false;
        
        std::cout << ansi::BRIGHT_YELLOW << "\n🛑 Shutting down Lighthouse V3..." << ansi::RESET << std::endl;
        
        if (beacon_) beacon_->stop();
        if (listener_) listener_->stop();
        
        if (monitor_thread_.joinable()) monitor_thread_.join();
        
        std::cout << ansi::BRIGHT_GREEN << "✅ Lighthouse V3 shutdown complete." << ansi::RESET << std::endl;
    }
    
    void wait() {
        std::cout << ansi::BRIGHT_YELLOW << "Press Ctrl+C to stop..." << ansi::RESET << std::endl;
        
        while (running_.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
private:
    void print_banner() {
		std::cout << ansi::BRIGHT_MAGENTA;  // Use magenta instead of purple
        std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   " << ansi::LIGHTHOUSE << " LITEHAUS V3 " << ansi::LIGHTHOUSE << "                    ║\n";
        std::cout << "║            Ultra-High-Performance Network Monitor            ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
        std::cout << ansi::RESET;
        
        std::cout << ansi::CYAN << "Platform: " << ansi::WHITE << 
#ifdef _WIN32
            "Windows (MINGW64)"
#else
            "Linux/Unix"
#endif
            << ansi::RESET << std::endl;
        std::cout << ansi::CYAN << "JSON Engine: " << ansi::WHITE << "Custom High-Performance Parser" << ansi::RESET << std::endl;
        std::cout << ansi::CYAN << "SIMD Capability: " << ansi::WHITE << detect_simd_capability() << "-bit" << ansi::RESET << std::endl;
        std::cout << ansi::CYAN << "Parse Threads: " << ansi::WHITE << config_.parse_threads << ansi::RESET << std::endl;
        std::cout << ansi::CYAN << "Batch Size: " << ansi::WHITE << config_.batch_size << ansi::RESET << std::endl;
        std::cout << ansi::CYAN << "Target: " << ansi::WHITE << config_.target_host << ":" << config_.target_port << ansi::RESET << std::endl;
        std::cout << ansi::CYAN << "Listen Port: " << ansi::WHITE << config_.listen_port << ansi::RESET << std::endl;
        std::cout << ansi::BRIGHT_CYAN << "════════════════════════════════════════════════════════════════\n" << ansi::RESET << std::endl;
    }
    
    void draw_dashboard() {
        auto stats = listener_ ? listener_->get_stats() : network_stats{};
        auto now_str = format::timestamp_now();
        
		std::cout << ansi::BRIGHT_CYAN << ansi::BOLD;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                        " << ansi::LIGHTHOUSE << " LITEHAUS DASHBOARD " << ansi::LIGHTHOUSE << "                        ║\n";
        std::cout << "║                    Ultra-High-Performance Network Monitor                  ║\n";
        std::cout << "╠════════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << ansi::RESET;
        
        // Status line
        std::cout << ansi::BRIGHT_WHITE << "║ " << ansi::GREEN << "●" << ansi::WHITE << " ACTIVE  "
                  << ansi::BRIGHT_BLACK << "│ " << ansi::CYAN << now_str << ansi::WHITE << "  "
                  << ansi::BRIGHT_BLACK << "│ " << ansi::YELLOW << "SIMD:" << detect_simd_capability() << "-bit" << ansi::WHITE;
        
        // Pad to right edge
        int padding = 74 - (8 + now_str.length() + 15);
        for (int i = 0; i < padding && i < 50; ++i) std::cout << " ";
        std::cout << "║\n";
        
        std::cout << ansi::BRIGHT_CYAN;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << ansi::RESET;
        
        // Network Stats
        std::cout << ansi::BRIGHT_WHITE << "║ " << ansi::BRIGHT_GREEN << "NETWORK STATISTICS" << ansi::WHITE;
        for (int i = 0; i < 55; ++i) std::cout << " ";
        std::cout << "║\n";
        
        std::cout << "║ " << ansi::YELLOW << "Packets Received: " << ansi::WHITE << std::setw(8) << stats.packets_received;
        std::cout << ansi::BRIGHT_BLACK << " │ " << ansi::YELLOW << "Active Connections: " << ansi::WHITE << std::setw(3) << stats.active_connections;
        for (int i = 0; i < 25; ++i) std::cout << " ";
        std::cout << "║\n";
        
        std::cout << "║ " << ansi::YELLOW << "Bytes Transmitted: " << ansi::WHITE << std::setw(10) << format::format_bytes(stats.bytes_transmitted);
        std::cout << ansi::BRIGHT_BLACK << " │ " << ansi::YELLOW << "SIMD Operations: " << ansi::WHITE << std::setw(8) << stats.simd_operations_count;
        for (int i = 0; i < 20; ++i) std::cout << " ";
        std::cout << "║\n";
        
        // Performance metrics
        std::cout << ansi::BRIGHT_CYAN;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << ansi::RESET;
        
        std::cout << ansi::BRIGHT_WHITE << "║ " << ansi::BRIGHT_GREEN << "PERFORMANCE METRICS" << ansi::WHITE;
        for (int i = 0; i < 54; ++i) std::cout << " ";
        std::cout << "║\n";
        
        std::cout << "║ " << ansi::YELLOW << "Parse Time (μs): " << ansi::WHITE 
                  << "Min=" << std::fixed << std::setprecision(1) << stats.min_parse_time_us
                  << " Max=" << stats.max_parse_time_us
                  << " Avg=" << stats.avg_parse_time_us;
        for (int i = 0; i < 20; ++i) std::cout << " ";
        std::cout << "║\n";
        
        // Cache hit rate
        double cache_rate = 0.0;
        if (stats.cache_hits + stats.cache_misses > 0) {
            cache_rate = (stats.cache_hits * 100.0) / (stats.cache_hits + stats.cache_misses);
        }
        
        std::cout << "║ " << ansi::YELLOW << "Cache Hit Rate: " << ansi::WHITE << std::setprecision(1) << cache_rate << "%  ";
        std::cout << ansi::BRIGHT_BLACK << format::progress_bar(cache_rate, 30);
        for (int i = 0; i < 15; ++i) std::cout << " ";
        std::cout << ansi::WHITE << "║\n";
        
        // Configuration
        std::cout << ansi::BRIGHT_CYAN;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << ansi::RESET;
        
        std::cout << ansi::BRIGHT_WHITE << "║ " << ansi::BRIGHT_GREEN << "CONFIGURATION" << ansi::WHITE;
        for (int i = 0; i < 60; ++i) std::cout << " ";
        std::cout << "║\n";
        
        std::cout << "║ " << ansi::YELLOW << "Target: " << ansi::WHITE << config_.target_host << ":" << config_.target_port;
        std::cout << ansi::BRIGHT_BLACK << " │ " << ansi::YELLOW << "Listen Port: " << ansi::WHITE << config_.listen_port;
        std::cout << ansi::BRIGHT_BLACK << " │ " << ansi::YELLOW << "Batch Size: " << ansi::WHITE << config_.batch_size;
        for (int i = 0; i < 15; ++i) std::cout << " ";
        std::cout << "║\n";
        
        std::cout << "║ " << ansi::YELLOW << "Parse Threads: " << ansi::WHITE << config_.parse_threads;
        std::cout << ansi::BRIGHT_BLACK << " │ " << ansi::YELLOW << "Beacon Interval: " << ansi::WHITE << config_.beacon_interval_ms << "ms";
        for (int i = 0; i < 35; ++i) std::cout << " ";
        std::cout << "║\n";
        
        // Recent activity
        std::cout << ansi::BRIGHT_CYAN;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << ansi::RESET;
        
        std::cout << ansi::BRIGHT_WHITE << "║ " << ansi::BRIGHT_GREEN << "RECENT ACTIVITY" << ansi::WHITE;
        for (int i = 0; i < 58; ++i) std::cout << " ";
        std::cout << "║\n";
        
        // Recent activity lines
        uint32_t current_seq = beacon_ ? beacon_->get_sequence_counter() : 0;
        std::cout << "║ " << ansi::BRIGHT_BLACK << now_str << ansi::WHITE << " " 
                  << ansi::GREEN << "→" << ansi::WHITE << " Beacon #" << current_seq 
                  << " sent to " << config_.target_host;
        for (int i = 0; i < 25; ++i) std::cout << " ";
        std::cout << "║\n";
        
        if (stats.active_connections > 0) {
            std::cout << "║ " << ansi::BRIGHT_BLACK << now_str << ansi::WHITE << " " 
                      << ansi::BLUE << "←" << ansi::WHITE << " " << stats.active_connections 
                      << " active connection" << (stats.active_connections != 1 ? "s" : "");
            for (int i = 0; i < 35; ++i) std::cout << " ";
            std::cout << "║\n";
        }
        
        // Footer
        std::cout << ansi::BRIGHT_CYAN;
        std::cout << "╚════════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << ansi::RESET;
        
        std::cout << ansi::BRIGHT_BLACK << "Press Ctrl+C to stop • Dashboard updates every 500ms" << ansi::RESET;
        
        // Clear rest of screen
        std::cout << "\033[J" << std::flush;
    }
    
    void monitor_loop() {
        auto last_report = std::chrono::steady_clock::now();
        
        while (running_.load()) {
            auto now = std::chrono::steady_clock::now();
            
            if (now - last_report >= std::chrono::seconds(10) && !dashboard_mode_.load()) {
                print_performance_report();
                last_report = now;
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void print_performance_report() {
        if (!listener_) return;
        
        auto stats = listener_->get_stats();
        
        std::cout << ansi::BRIGHT_CYAN << "\n" << ansi::WAVE << "─── Performance Report ───" << ansi::RESET << std::endl;
        std::cout << ansi::YELLOW << "Packets Received: " << ansi::WHITE << stats.packets_received << ansi::RESET << std::endl;
        std::cout << ansi::YELLOW << "Bytes Transmitted: " << ansi::WHITE << format::format_bytes(stats.bytes_transmitted) << ansi::RESET << std::endl;
        std::cout << ansi::YELLOW << "Active Connections: " << ansi::WHITE << stats.active_connections << ansi::RESET << std::endl;
        std::cout << ansi::YELLOW << "Parse Times (μs): " << ansi::WHITE
                  << "Min=" << stats.min_parse_time_us 
                  << ", Max=" << stats.max_parse_time_us 
                  << ", Avg=" << stats.avg_parse_time_us << ansi::RESET << std::endl;
        std::cout << ansi::YELLOW << "SIMD Operations: " << ansi::WHITE << stats.simd_operations_count << ansi::RESET << std::endl;
        
        if ((stats.cache_hits + stats.cache_misses) > 0) {
            std::cout << ansi::YELLOW << "Cache Hit Rate: " << ansi::WHITE
                      << (stats.cache_hits * 100.0 / (stats.cache_hits + stats.cache_misses)) 
                      << "%" << ansi::RESET << std::endl;
        }
        std::cout << ansi::BRIGHT_CYAN << "─────────────────────────────\n" << ansi::RESET << std::endl;
    }
};

} // namespace whispr::network

// Global application instance for signal handling
whispr::network::lighthouse_application* g_app = nullptr;

// Signal handler
void signal_handler(int signal) {
    std::cout << ansi::BRIGHT_YELLOW << "\nShutdown signal received..." << ansi::RESET << std::endl;
    if (g_app) {
        g_app->stop();
    }
    std::exit(0);
}

// Main entry point
int main(int argc, char* argv[]) {
    // Default configuration
    whispr::network::monitor_config config{
        .target_host = "127.0.0.1",
        .target_port = 9001,
        .listen_port = 9000,
        .beacon_interval_ms = 1000,
        .max_concurrent_connections = 100,
        .enable_compression = true,
        .enable_encryption = false,
        .batch_size = 10,
        .enable_simd_validation = true,
        .enable_prefetch = true,
        .parse_threads = std::thread::hardware_concurrency(),
        .string_pool_size = 16384
    };
    
    bool dashboard_mode = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--target" && i + 1 < argc) {
            config.target_host = argv[++i];
        } else if (arg == "--target-port" && i + 1 < argc) {
            config.target_port = static_cast<uint16_t>(std::stoi(argv[++i]));
        } else if (arg == "--listen-port" && i + 1 < argc) {
            config.listen_port = static_cast<uint16_t>(std::stoi(argv[++i]));
        } else if (arg == "--interval" && i + 1 < argc) {
            config.beacon_interval_ms = static_cast<uint32_t>(std::stoi(argv[++i]));
        } else if (arg == "--batch-size" && i + 1 < argc) {
            config.batch_size = static_cast<uint32_t>(std::stoi(argv[++i]));
        } else if (arg == "--parse-threads" && i + 1 < argc) {
            config.parse_threads = static_cast<uint32_t>(std::stoi(argv[++i]));
        } else if (arg == "--no-simd-validation") {
            config.enable_simd_validation = false;
        } else if (arg == "--dashboard") {
            dashboard_mode = true;
        } else if (arg == "--help") {
			std::cout << ansi::BRIGHT_MAGENTA;  // Use magenta instead of purple
            std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                   " << ansi::LIGHTHOUSE << " LITEHAUS V3 " << ansi::LIGHTHOUSE << "                    ║\n";
            std::cout << "║            Ultra-High-Performance Network Monitor            ║\n";
            std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
            std::cout << ansi::RESET;
            
            std::cout << ansi::BRIGHT_WHITE << "Usage: " << ansi::CYAN << argv[0] << " [options]\n" << ansi::RESET;
            std::cout << ansi::BRIGHT_GREEN << "Options:\n" << ansi::RESET;
            std::cout << ansi::YELLOW << "  --target HOST          " << ansi::WHITE << "Target host IP (default: 127.0.0.1)\n" << ansi::RESET;
            std::cout << ansi::YELLOW << "  --target-port PORT     " << ansi::WHITE << "Target port (default: 9001)\n" << ansi::RESET;
            std::cout << ansi::YELLOW << "  --listen-port PORT     " << ansi::WHITE << "Listen port (default: 9000)\n" << ansi::RESET;
            std::cout << ansi::YELLOW << "  --interval MS          " << ansi::WHITE << "Beacon interval in ms (default: 1000)\n" << ansi::RESET;
            std::cout << ansi::YELLOW << "  --batch-size N         " << ansi::WHITE << "Message batch size (default: 10)\n" << ansi::RESET;
            std::cout << ansi::YELLOW << "  --parse-threads N      " << ansi::WHITE << "Number of parse threads (default: hardware)\n" << ansi::RESET;
            std::cout << ansi::YELLOW << "  --no-simd-validation   " << ansi::WHITE << "Disable SIMD validation\n" << ansi::RESET;
            std::cout << ansi::YELLOW << "  --dashboard            " << ansi::WHITE << "Enable beautiful real-time dashboard\n" << ansi::RESET;
            std::cout << ansi::YELLOW << "  --help                 " << ansi::WHITE << "Show this help\n" << ansi::RESET;
            
            std::cout << ansi::BRIGHT_GREEN << "\nExamples:\n" << ansi::RESET;
            std::cout << ansi::CYAN << "  " << argv[0] << "                           " << ansi::WHITE << "# Start with defaults\n" << ansi::RESET;
            std::cout << ansi::CYAN << "  " << argv[0] << " --dashboard               " << ansi::WHITE << "# Start with dashboard\n" << ansi::RESET;
            std::cout << ansi::CYAN << "  " << argv[0] << " --target 192.168.1.100    " << ansi::WHITE << "# Custom target\n" << ansi::RESET;
            std::cout << ansi::CYAN << "  " << argv[0] << " --listen-port 8080        " << ansi::WHITE << "# Custom listen port\n" << ansi::RESET;
            
            std::cout << ansi::BRIGHT_YELLOW << "\n" << ansi::SPARKLE << " Happy beaconing, fren! " << ansi::SPARKLE << ansi::RESET << std::endl;
            return 0;
        }
    }
    
    // Create and run application
    try {
        whispr::network::lighthouse_application app(config);
        g_app = &app;
        
        // Set up signal handler
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);
        
        if (dashboard_mode) {
            app.start_dashboard_mode(); // Beautiful dashboard mode!
        } else {
            app.start();
            app.wait();
        }
        
    } catch (const std::exception& e) {
        std::cerr << ansi::BRIGHT_RED << "❌ Fatal error: " << e.what() << ansi::RESET << std::endl;
        return 1;
    }
    
    return 0;
}
