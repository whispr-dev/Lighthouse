# 🏰 Lighthouse Beacon System

A high-performance beacon/listener system that monitors **fastping.it.com** and broadcasts trustworthy status signals across the network. Think of it as a lighthouse for the internet - constantly scanning for safety and broadcasting reliable navigation signals.

## 🎯 System Overview

The Lighthouse Beacon System consists of:

1. **🔍 Listener**: Continuously polls `fastping.it.com/ping?format=json` for network status
2. **📡 Beacon**: Broadcasts UDP status signals with parsed data 
3. **🕐 Speaking Clock**: Regular status announcements for monitoring
4. **⚡ High-Performance Parser**: AVX2-optimized JSON processing with custom tokenizer

## 🚀 Key Features

- **Ultra-Fast JSON Parsing**: Custom tokenizer + AVX2 minification
- **Multi-threaded Architecture**: Separate threads for listening, beaconing, and status updates
- **Real-time Network Monitoring**: 10-second ping intervals with sub-millisecond parsing
- **Reliable UDP Broadcasting**: Minified JSON payloads for maximum efficiency
- **Health Status Validation**: Smart signal analysis and age tracking
- **Speaking Clock**: Human-readable status reports every 30 seconds

## 📂 Project Structure

```
lighthouse_beacon/
├── lighthouse_main.cpp          # Main lighthouse application
├── beacon_listener.cpp          # UDP listener for testing
├── json_test.cpp               # Parser test suite
├── tokenizer.hpp               # Enhanced JSON tokenizer
├── avx2_minifier_core.hpp/.cpp # AVX2 JSON minification
├── delta_demo.cpp              # Delta encoding utilities
├── CMakeLists.txt              # Build configuration
└── README.md                   # This file
```

## 🛠️ Building the System

### Prerequisites
- **Windows**: Visual Studio 2019+ or MinGW with C++17 support
- **Linux**: GCC 8+ or Clang 10+ with C++17 support
- **CMake**: Version 3.20 or higher
- **Libraries**: WinSock2 + WinINet (Windows) or libcurl (Linux)

### Build Commands

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
cmake --build . --config Release

# Or build specific targets
cmake --build . --target lighthouse_beacon
cmake --build . --target beacon_listener
cmake --build . --target json_parser_test
```

### Quick Build & Run
```bash
# Build and run lighthouse
cmake --build . --target run_lighthouse

# Build and run listener (separate terminal)
cmake --build . --target run_listener
```

## 🎮 Usage

### Running the Lighthouse Beacon
```bash
# Start the lighthouse system
./bin/lighthouse_beacon

# Output:
🏰 Lighthouse Beacon System Starting...
   FastPing URL: http://fastping.it.com/ping?format=json
   Beacon Target: 161.35.248.233:9876
   Ping Interval: 10000ms
   Beacon Interval: 5000ms

🔍 Listener thread started - monitoring fastping.it.com
📻 Beacon thread started - broadcasting to 161.35.248.233:9876
🕐 Speaking clock started - status every 30 seconds
Press Enter to stop the lighthouse...
```

### Testing with Beacon Listener
```bash
# In another terminal, start the listener
./bin/beacon_listener

# Or specify custom port
./bin/beacon_listener 9999

# Output:
🎯 Beacon listener bound to port 9876
🎧 Listening for lighthouse beacons...
Press Ctrl+C to stop

📡 [14:30:15] Received 156 bytes from 192.168.1.100
Raw data: {"beacon_id":"lighthouse-001","timestamp":1672531200,"status":"healthy"...}

┌─────────────────────────────────────────┐
│ 🚨 LIGHTHOUSE BEACON RECEIVED          │
├─────────────────────────────────────────┤
│ ID: lighthouse-001                      │
│ Status: healthy                         │
│ Ping Status: ok                         │
│ Ping Latency: 12.34ms                  │
│ Signal Age: 5s                          │
│ Timestamp: 2023-01-01 12:00:00         │
└─────────────────────────────────────────┘

✅ Beacon status: HEALTHY
```

### Running Performance Tests
```bash
# Test the JSON parser performance
./bin/json_parser_test

# Output includes:
🧪 JSON Parser Test Suite
============================

⚡ Testing AVX2 JSON Minifier...
📊 Performance Benchmark...
Running 10000 iterations on 180 byte JSON...
Throughput: 45.67 MB/second
🎉 All tests completed successfully!
```

## 📡 Network Protocol

### FastPing Response Format
```json
{
    "status": "ok",
    "connecting_ip": "192.168.1.100", 
    "anonymity_level": "high",
    "speed_hint": "fast",
    "server_processing_latency_ms": 12.34,
    "client_ip_from_headers": "203.0.113.45",
    "message": "All systems operational"
}
```

### Beacon Payload Format
```json
{
    "beacon_id": "lighthouse-001",
    "timestamp": 1672531200,
    "status": "healthy",
    "last_ping_status": "ok", 
    "ping_latency": 12.34,
    "signal_age_seconds": 5
}
```

## ⚡ Performance Characteristics

### Parser Benchmarks
- **JSON Minification**: ~45MB/s with AVX2 optimization
- **Token Parsing**: Sub-microsecond for typical payloads
- **End-to-end Processing**: <100µs for complete FastPing response
- **Memory Usage**: Zero-copy string_view tokenization

### Network Performance  
- **Ping Frequency**: 10 seconds (configurable)
- **Beacon Frequency**: 5 seconds (configurable)
- **UDP Payload Size**: ~150 bytes (minified)
- **Network Latency**: Measures server round-trip time

## 🔧 Configuration

### Environment Variables
```bash
export FASTPING_URL="http://fastping.it.com/ping?format=json"
export BEACON_IP="161.35.248.233"
export BEACON_PORT="9876" 
export PING_INTERVAL_MS="10000"
export BEACON_INTERVAL_MS="5000"
```

### Code Configuration
Edit `lighthouse_main.cpp` constants:
```cpp
std::string fastping_url = "http://fastping.it.com/ping?format=json";
std::string beacon_ip = "161.35.248.233";
int beacon_port = 9876;
int ping_interval_ms = 10000;  // 10 seconds
int beacon_interval_ms = 5000; // 5 seconds
```

## 🏥 Health Monitoring

### Status Indicators
- **✅ Healthy**: Recent successful ping, good response time
- **⚠️ Warning**: Old signal, degraded performance, or failed pings
- **❌ Critical**: No response from FastPing server

### Speaking Clock Output
```
🏰 LIGHTHOUSE STATUS REPORT:
   Signal Health: ✅ HEALTHY
   Last Status: ok
   Current IP: 192.168.1.100
   Anonymity: high
   Speed Hint: fast
   Signal Age: 8 seconds
   ════════════════════════════════════════
```

## 🧪 Testing & Validation

### Unit Tests
```bash
# Run all parser tests
./bin/json_parser_test

# Verify tokenizer accuracy
# Benchmark minification performance  
# Test full parsing pipeline
# Performance stress testing
```

### Integration Tests
```bash
# Terminal 1: Start lighthouse
./bin/lighthouse_beacon

# Terminal 2: Start listener  
./bin/beacon_listener

# Verify beacon reception and parsing
# Check status health validation
# Monitor speaking clock announcements
```

## 🛡️ Security Considerations

- **No Authentication**: UDP beacons are broadcast in plaintext
- **IP Exposure**: FastPing reveals your public IP address
- **Rate Limiting**: Respect FastPing server limits (10s intervals)
- **Network Visibility**: UDP broadcasts may be visible to network monitors

## 🚀 Future Enhancements

- **🔐 Encryption**: Add beacon payload encryption
- **📊 Metrics**: Prometheus/Grafana integration
- **🌐 Multi-Server**: Support multiple FastPing endpoints
- **💾 Persistence**: SQLite logging for historical analysis
- **🔔 Alerting**: Email/SMS notifications for service degradation
- **📡 Discovery**: Automatic beacon endpoint discovery

## 🤝 Contributing

This lighthouse system uses the **Problem-First Protocol** for development:

1. **Context Loading**: Understand the networking problem and constraints
2. **Complete Solutions**: Full working code artifacts, not fragments  
3. **Performance First**: AVX2 optimization and zero-copy parsing
4. **Real-world Testing**: Actual FastPing integration, not mock data

## 📄 License

Built for RYO Modular and whispr.dev with ❤️ by the engineering team.

---

*"A lighthouse doesn't fire cannons to call attention to its shining - it just shines."* - Keep your networks safe, husklyfren! 🏰✨
