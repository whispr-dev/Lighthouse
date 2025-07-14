#include <jsonifier/Index.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include <iomanip>
#include <fstream>
#include <thread>
#include <atomic>
#include <memory>
#include <sstream>

// 🏰 ULTIMATE JSON PERFORMANCE BENCHMARK
// Showcasing RTC's Jsonifier - The Fastest JSON Library in Existence
// Comprehensive testing of all performance characteristics

namespace UltimateBenchmark {

// 🚀 Test data structures for comprehensive benchmarking
struct SimpleFastPingResponse {
    jsonifier::string status{};
    jsonifier::string connecting_ip{};
    jsonifier::string anonymity_level{};
    jsonifier::string speed_hint{};
    double server_processing_latency_ms{ 0.0 };
};

struct ComplexFastPingResponse {
    jsonifier::string status{};
    jsonifier::string connecting_ip{};
    jsonifier::string anonymity_level{};
    jsonifier::string speed_hint{};
    double server_processing_latency_ms{ 0.0 };
    jsonifier::string client_ip_from_headers{};
    jsonifier::string message{};
    std::vector<jsonifier::string> additional_headers{};
    std::map<jsonifier::string, jsonifier::string> metadata{};
};

struct NestedPerformanceTest {
    struct InnerData {
        std::vector<double> measurements{};
        std::map<jsonifier::string, int> counters{};
        bool enabled{ true };
    };
    
    jsonifier::string test_name{};
    std::vector<InnerData> data_sets{};
    std::map<jsonifier::string, InnerData> named_sets{};
    uint64_t timestamp{ 0 };
};

// 🔥 Benchmark Result Structures
struct BenchmarkResult {
    jsonifier::string test_name{};
    uint64_t iterations{ 0 };
    double total_time_microseconds{ 0.0 };
    double average_time_microseconds{ 0.0 };
    double min_time_microseconds{ 0.0 };
    double max_time_microseconds{ 0.0 };
    double throughput_mbps{ 0.0 };
    uint64_t total_bytes{ 0 };
    bool success{ false };
};

struct ComprehensiveResults {
    std::vector<BenchmarkResult> parse_results{};
    std::vector<BenchmarkResult> serialize_results{};
    std::vector<BenchmarkResult> roundtrip_results{};
    jsonifier::string cpu_optimization_level{};
    jsonifier::string system_info{};
    double total_benchmark_time_seconds{ 0.0 };
};

// ⚡ Ultra-High Performance Benchmark Engine
class UltimateBenchmarkEngine {
private:
    jsonifier::jsonifier_core<> json_core{};
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    
public:
    UltimateBenchmarkEngine() {
        std::cout << "🚀 RTC Jsonifier Ultimate Benchmark Engine Initialized!\n";
        std::cout << "⚡ SIMD Optimization: " << getOptimizationInfo() << "\n";
        std::cout << "🔥 CPU Features: " << getCPUInfo() << "\n\n";
    }
    
    // 🔥 Comprehensive parse benchmark
    template<typename T>
    BenchmarkResult benchmarkParsing(const std::string& test_name, 
                                    const std::string& json_data, 
                                    uint64_t iterations = 10000) {
        std::cout << "🧪 Running parse benchmark: " << test_name 
                 << " (" << iterations << " iterations)\n";
        
        BenchmarkResult result{};
        result.test_name = test_name;
        result.iterations = iterations;
        result.total_bytes = json_data.size() * iterations;
        
        std::vector<double> times;
        times.reserve(iterations);
        
        T test_object{};
        bool all_successful = true;
        
        auto benchmark_start = std::chrono::high_resolution_clock::now();
        
        // Warm-up runs
        for (int i = 0; i < 100; ++i) {
            json_core.parseJson(test_object, json_data);
        }
        
        // Actual benchmark
        for (uint64_t i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                json_core.parseJson(test_object, json_data);
            } catch (...) {
                all_successful = false;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            times.push_back(duration.count() / 1000.0); // Convert to microseconds
        }
        
        auto benchmark_end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(benchmark_end - benchmark_start);
        
        // Calculate statistics
        result.success = all_successful;
        result.total_time_microseconds = total_time.count();
        result.average_time_microseconds = result.total_time_microseconds / iterations;
        result.min_time_microseconds = *std::min_element(times.begin(), times.end());
        result.max_time_microseconds = *std::max_element(times.begin(), times.end());
        
        // Calculate throughput in MB/s
        double total_seconds = result.total_time_microseconds / 1000000.0;
        double total_mb = result.total_bytes / (1024.0 * 1024.0);
        result.throughput_mbps = total_mb / total_seconds;
        
        displayResult(result);
        return result;
    }
    
    // 🚀 Comprehensive serialize benchmark
    template<typename T>
    BenchmarkResult benchmarkSerialization(const std::string& test_name, 
                                          const T& test_object, 
                                          uint64_t iterations = 10000) {
        std::cout << "📤 Running serialize benchmark: " << test_name 
                 << " (" << iterations << " iterations)\n";
        
        BenchmarkResult result{};
        result.test_name = test_name + " (Serialize)";
        result.iterations = iterations;
        
        std::vector<double> times;
        times.reserve(iterations);
        
        bool all_successful = true;
        uint64_t total_bytes = 0;
        
        auto benchmark_start = std::chrono::high_resolution_clock::now();
        
        // Warm-up runs
        for (int i = 0; i < 100; ++i) {
            std::string output;
            json_core.serializeJson(test_object, output);
        }
        
        // Actual benchmark
        for (uint64_t i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                std::string output;
                json_core.serializeJson(test_object, output);
                total_bytes += output.size();
            } catch (...) {
                all_successful = false;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            times.push_back(duration.count() / 1000.0); // Convert to microseconds
        }
        
        auto benchmark_end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(benchmark_end - benchmark_start);
        
        // Calculate statistics
        result.success = all_successful;
        result.total_bytes = total_bytes;
        result.total_time_microseconds = total_time.count();
        result.average_time_microseconds = result.total_time_microseconds / iterations;
        result.min_time_microseconds = *std::min_element(times.begin(), times.end());
        result.max_time_microseconds = *std::max_element(times.begin(), times.end());
        
        // Calculate throughput in MB/s
        double total_seconds = result.total_time_microseconds / 1000000.0;
        double total_mb = result.total_bytes / (1024.0 * 1024.0);
        result.throughput_mbps = total_mb / total_seconds;
        
        displayResult(result);
        return result;
    }
    
    // 🔄 Round-trip benchmark (parse + serialize)
    template<typename T>
    BenchmarkResult benchmarkRoundTrip(const std::string& test_name, 
                                      const std::string& json_data, 
                                      uint64_t iterations = 5000) {
        std::cout << "🔄 Running round-trip benchmark: " << test_name 
                 << " (" << iterations << " iterations)\n";
        
        BenchmarkResult result{};
        result.test_name = test_name + " (Round-trip)";
        result.iterations = iterations;
        
        std::vector<double> times;
        times.reserve(iterations);
        
        bool all_successful = true;
        uint64_t total_bytes = 0;
        
        auto benchmark_start = std::chrono::high_resolution_clock::now();
        
        // Actual benchmark
        for (uint64_t i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                T test_object{};
                json_core.parseJson(test_object, json_data);
                
                std::string output;
                json_core.serializeJson(test_object, output);
                total_bytes += json_data.size() + output.size();
            } catch (...) {
                all_successful = false;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            times.push_back(duration.count() / 1000.0); // Convert to microseconds
        }
        
        auto benchmark_end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(benchmark_end - benchmark_start);
        
        // Calculate statistics
        result.success = all_successful;
        result.total_bytes = total_bytes;
        result.total_time_microseconds = total_time.count();
        result.average_time_microseconds = result.total_time_microseconds / iterations;
        result.min_time_microseconds = *std::min_element(times.begin(), times.end());
        result.max_time_microseconds = *std::max_element(times.begin(), times.end());
        
        // Calculate throughput in MB/s
        double total_seconds = result.total_time_microseconds / 1000000.0;
        double total_mb = result.total_bytes / (1024.0 * 1024.0);
        result.throughput_mbps = total_mb / total_seconds;
        
        displayResult(result);
        return result;
    }
    
    // 🚀 Stress test with massive concurrency
    template<typename T>
    BenchmarkResult stressTest(const std::string& test_name, 
                              const std::string& json_data, 
                              uint64_t total_iterations = 100000,
                              uint32_t thread_count = 0) {
        if (thread_count == 0) {
            thread_count = std::thread::hardware_concurrency();
        }
        
        std::cout << "💪 Running stress test: " << test_name 
                 << " (" << total_iterations << " iterations, " 
                 << thread_count << " threads)\n";
        
        BenchmarkResult result{};
        result.test_name = test_name + " (Stress Test)";
        result.iterations = total_iterations;
        result.total_bytes = json_data.size() * total_iterations;
        
        std::atomic<uint64_t> completed_iterations{ 0 };
        std::atomic<uint64_t> successful_iterations{ 0 };
        std::vector<std::thread> workers;
        
        auto benchmark_start = std::chrono::high_resolution_clock::now();
        
        uint64_t iterations_per_thread = total_iterations / thread_count;
        
        for (uint32_t i = 0; i < thread_count; ++i) {
            workers.emplace_back([&, iterations_per_thread]() {
                jsonifier::jsonifier_core<> local_core{};
                
                for (uint64_t j = 0; j < iterations_per_thread; ++j) {
                    try {
                        T test_object{};
                        local_core.parseJson(test_object, json_data);
                        successful_iterations.fetch_add(1);
                    } catch (...) {
                        // Error counted in success rate
                    }
                    completed_iterations.fetch_add(1);
                }
            });
        }
        
        // Wait for all workers to complete
        for (auto& worker : workers) {
            worker.join();
        }
        
        auto benchmark_end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(benchmark_end - benchmark_start);
        
        result.total_time_microseconds = total_time.count();
        result.average_time_microseconds = result.total_time_microseconds / total_iterations;
        result.success = (successful_iterations.load() == total_iterations);
        
        // Calculate throughput in MB/s
        double total_seconds = result.total_time_microseconds / 1000000.0;
        double total_mb = result.total_bytes / (1024.0 * 1024.0);
        result.throughput_mbps = total_mb / total_seconds;
        
        std::cout << "   Completed: " << completed_iterations.load() << "/" << total_iterations << "\n";
        std::cout << "   Success Rate: " << std::fixed << std::setprecision(1) 
                 << (double)successful_iterations.load() / total_iterations * 100.0 << "%\n";
        
        displayResult(result);
        return result;
    }
    
    // 🧪 Generate test data
    std::string generateSimpleFastPingJson() {
        return R"({
            "status": "ok",
            "connecting_ip": "192.168.1.100",
            "anonymity_level": "high",
            "speed_hint": "fast",
            "server_processing_latency_ms": 12.34
        })";
    }
    
    std::string generateComplexFastPingJson() {
        return R"({
            "status": "ok",
            "connecting_ip": "192.168.1.100",
            "anonymity_level": "high",
            "speed_hint": "fast",
            "server_processing_latency_ms": 12.34,
            "client_ip_from_headers": "203.0.113.45",
            "message": "All systems operational and performing at peak efficiency",
            "additional_headers": ["X-Forwarded-For", "X-Real-IP", "User-Agent"],
            "metadata": {
                "region": "us-east-1",
                "datacenter": "primary",
                "load_balancer": "nginx-001"
            }
        })";
    }
    
    NestedPerformanceTest generateComplexNestedData() {
        NestedPerformanceTest test;
        test.test_name = "Complex Nested Performance Test";
        test.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        // Generate random data
        std::uniform_real_distribution<double> real_dist(0.0, 1000.0);
        std::uniform_int_distribution<int> int_dist(1, 100);
        
        for (int i = 0; i < 10; ++i) {
            NestedPerformanceTest::InnerData data;
            
            for (int j = 0; j < 50; ++j) {
                data.measurements.push_back(real_dist(gen));
            }
            
            for (int j = 0; j < 20; ++j) {
                data.counters["counter_" + std::to_string(j)] = int_dist(gen);
            }
            
            data.enabled = (i % 2 == 0);
            test.data_sets.push_back(data);
            test.named_sets["set_" + std::to_string(i)] = data;
        }
        
        return test;
    }
    
private:
    void displayResult(const BenchmarkResult& result) {
        std::cout << "   ✅ " << result.test_name << "\n";
        std::cout << "      Average: " << std::fixed << std::setprecision(2) 
                 << result.average_time_microseconds << "µs\n";
        std::cout << "      Min/Max: " << std::fixed << std::setprecision(2) 
                 << result.min_time_microseconds << "µs / " 
                 << result.max_time_microseconds << "µs\n";
        std::cout << "      Throughput: " << std::fixed << std::setprecision(1) 
                 << result.throughput_mbps << " MB/s\n";
        std::cout << "      Success: " << (result.success ? "✅" : "❌") << "\n\n";
    }
    
    std::string getOptimizationInfo() {
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
    
    std::string getCPUInfo() {
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

// 🎯 Comprehensive Benchmark Runner
class ComprehensiveBenchmarkRunner {
private:
    std::unique_ptr<UltimateBenchmarkEngine> engine;
    
public:
    ComprehensiveBenchmarkRunner() {
        engine = std::make_unique<UltimateBenchmarkEngine>();
    }
    
    void runAllBenchmarks() {
        std::cout << R"(
🏰 ═══════════════════════════════════════════════════════════════════ 🏰
   RTC JSONIFIER COMPREHENSIVE PERFORMANCE BENCHMARK
🚀 Testing the Absolute Limits of JSON Processing Performance 🚀
🏰 ═══════════════════════════════════════════════════════════════════ 🏰

)" << std::endl;
        
        auto overall_start = std::chrono::high_resolution_clock::now();
        
        ComprehensiveResults results{};
        
        // 📊 PARSING BENCHMARKS
        std::cout << "📊 PARSING PERFORMANCE BENCHMARKS:\n";
        std::cout << "═══════════════════════════════════════════════════════════════════\n\n";
        
        auto simple_json = engine->generateSimpleFastPingJson();
        auto complex_json = engine->generateComplexFastPingJson();
        auto nested_data = engine->generateComplexNestedData();
        
        results.parse_results.push_back(
            engine->benchmarkParsing<SimpleFastPingResponse>("Simple FastPing", simple_json, 50000));
        
        results.parse_results.push_back(
            engine->benchmarkParsing<ComplexFastPingResponse>("Complex FastPing", complex_json, 25000));
        
        // Serialize nested data to JSON for parsing benchmark
        std::string nested_json;
        jsonifier::jsonifier_core<> temp_core;
        temp_core.serializeJson(nested_data, nested_json);
        
        results.parse_results.push_back(
            engine->benchmarkParsing<NestedPerformanceTest>("Large Nested JSON", nested_json, 10000));
        
        // 📤 SERIALIZATION BENCHMARKS  
        std::cout << "📤 SERIALIZATION PERFORMANCE BENCHMARKS:\n";
        std::cout << "═══════════════════════════════════════════════════════════════════\n\n";
        
        SimpleFastPingResponse simple_obj;
        temp_core.parseJson(simple_obj, simple_json);
        
        ComplexFastPingResponse complex_obj;
        temp_core.parseJson(complex_obj, complex_json);
        
        results.serialize_results.push_back(
            engine->benchmarkSerialization("Simple FastPing", simple_obj, 50000));
        
        results.serialize_results.push_back(
            engine->benchmarkSerialization("Complex FastPing", complex_obj, 25000));
        
        results.serialize_results.push_back(
            engine->benchmarkSerialization("Large Nested JSON", nested_data, 10000));
        
        // 🔄 ROUND-TRIP BENCHMARKS
        std::cout << "🔄 ROUND-TRIP PERFORMANCE BENCHMARKS:\n";
        std::cout << "═══════════════════════════════════════════════════════════════════\n\n";
        
        results.roundtrip_results.push_back(
            engine->benchmarkRoundTrip<SimpleFastPingResponse>("Simple FastPing", simple_json, 25000));
        
        results.roundtrip_results.push_back(
            engine->benchmarkRoundTrip<ComplexFastPingResponse>("Complex FastPing", complex_json, 15000));
        
        results.roundtrip_results.push_back(
            engine->benchmarkRoundTrip<NestedPerformanceTest>("Large Nested JSON", nested_json, 5000));
        
        // 💪 STRESS TESTS
        std::cout << "💪 STRESS TEST BENCHMARKS:\n";
        std::cout << "═══════════════════════════════════════════════════════════════════\n\n";
        
        engine->stressTest<SimpleFastPingResponse>("Multi-threaded Parse", simple_json, 100000);
        
        auto overall_end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::seconds>(overall_end - overall_start);
        results.total_benchmark_time_seconds = total_time.count();
        
        displayFinalResults(results);
    }
    
    void runQuickBenchmark() {
        std::cout << "🚀 Running Quick Performance Benchmark...\n\n";
        
        auto simple_json = engine->generateSimpleFastPingJson();
        engine->benchmarkParsing<SimpleFastPingResponse>("Quick Parse Test", simple_json, 10000);
        
        SimpleFastPingResponse obj;
        jsonifier::jsonifier_core<> core;
        core.parseJson(obj, simple_json);
        engine->benchmarkSerialization("Quick Serialize Test", obj, 10000);
    }
    
    void runStressTest() {
        std::cout << "💪 Running Comprehensive Stress Test...\n\n";
        
        auto simple_json = engine->generateSimpleFastPingJson();
        engine->stressTest<SimpleFastPingResponse>("Stress Test", simple_json, 500000, 16);
    }
    
private:
    void displayFinalResults(const ComprehensiveResults& results) {
        std::cout << R"(
🏆 ═══════════════════════════════════════════════════════════════════ 🏆
   COMPREHENSIVE BENCHMARK RESULTS SUMMARY
🏆 ═══════════════════════════════════════════════════════════════════ 🏆

)";
        
        std::cout << "📊 PARSING RESULTS:\n";
        displayResultsTable(results.parse_results);
        
        std::cout << "\n📤 SERIALIZATION RESULTS:\n";
        displayResultsTable(results.serialize_results);
        
        std::cout << "\n🔄 ROUND-TRIP RESULTS:\n";
        displayResultsTable(results.roundtrip_results);
        
        // Calculate overall statistics
        double total_throughput = 0.0;
        size_t total_tests = results.parse_results.size() + results.serialize_results.size() + results.roundtrip_results.size();
        
        for (const auto& result : results.parse_results) {
            total_throughput += result.throughput_mbps;
        }
        for (const auto& result : results.serialize_results) {
            total_throughput += result.throughput_mbps;
        }
        for (const auto& result : results.roundtrip_results) {
            total_throughput += result.throughput_mbps;
        }
        
        double average_throughput = total_throughput / total_tests;
        
        std::cout << "\n🏆 SUMMARY:\n";
        std::cout << "   Total Benchmark Time: " << std::fixed << std::setprecision(1) 
                 << results.total_benchmark_time_seconds << " seconds\n";
        std::cout << "   Average Throughput: " << std::fixed << std::setprecision(1) 
                 << average_throughput << " MB/s\n";
        
        #if JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX512)
            std::cout << "   🚀 Optimization Level: MAXIMUM (AVX-512)\n";
        #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX2)
            std::cout << "   ⚡ Optimization Level: HIGH (AVX2)\n";
        #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX)
            std::cout << "   🔥 Optimization Level: GOOD (AVX)\n";
        #else
            std::cout << "   📊 Optimization Level: STANDARD\n";
        #endif
        
        std::cout << "\n🚀 RTC Jsonifier: THE FASTEST JSON LIBRARY! 🚀\n";
        std::cout << "🏰 ═══════════════════════════════════════════════════════════════════ 🏰\n\n";
    }
    
    void displayResultsTable(const std::vector<BenchmarkResult>& results) {
        std::cout << std::left << std::setw(20) << "Test Name"
                 << std::setw(12) << "Avg (µs)"
                 << std::setw(12) << "Min (µs)" 
                 << std::setw(12) << "Max (µs)"
                 << std::setw(15) << "Throughput"
                 << std::setw(10) << "Success" << "\n";
        std::cout << std::string(80, '-') << "\n";
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(20) << result.test_name.substr(0, 19)
                     << std::setw(12) << std::fixed << std::setprecision(2) << result.average_time_microseconds
                     << std::setw(12) << std::fixed << std::setprecision(2) << result.min_time_microseconds
                     << std::setw(12) << std::fixed << std::setprecision(2) << result.max_time_microseconds
                     << std::setw(15) << (std::to_string(static_cast<int>(result.throughput_mbps)) + " MB/s")
                     << std::setw(10) << (result.success ? "✅" : "❌") << "\n";
        }
    }
};

} // namespace UltimateBenchmark

// 🎯 Interactive Menu System
void displayMenu() {
    std::cout << R"(
🏰 RTC Jsonifier Ultimate Benchmark Menu:
   1. Comprehensive Benchmark (All Tests)
   2. Quick Performance Test
   3. Stress Test (High Load)
   4. CPU Feature Test
   5. Save Results to File
   6. Exit

Enter your choice (1-6): )";
}

void runCPUFeatureTest() {
    std::cout << "\n🔍 CPU FEATURE DETECTION:\n";
    std::cout << "═══════════════════════════════════════════════════════════════════\n";
    
    #if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_POPCNT)
        std::cout << "✅ POPCNT instruction support detected\n";
    #else
        std::cout << "❌ POPCNT instruction support not available\n";
    #endif
    
    #if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_BMI)
        std::cout << "✅ BMI instruction support detected\n";
    #else
        std::cout << "❌ BMI instruction support not available\n";
    #endif
    
    #if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_BMI2)
        std::cout << "✅ BMI2 instruction support detected\n";
    #else
        std::cout << "❌ BMI2 instruction support not available\n";
    #endif
    
    #if JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX)
        std::cout << "✅ AVX instruction support detected\n";
    #else
        std::cout << "❌ AVX instruction support not available\n";
    #endif
    
    #if JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX2)
        std::cout << "✅ AVX2 instruction support detected\n";
    #else
        std::cout << "❌ AVX2 instruction support not available\n";
    #endif
    
    #if JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX512)
        std::cout << "🚀 AVX-512 instruction support detected - MAXIMUM POWER!\n";
    #else
        std::cout << "❌ AVX-512 instruction support not available\n";
    #endif
    
    #if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_NEON)
        std::cout << "✅ ARM NEON instruction support detected\n";
    #else
        std::cout << "❌ ARM NEON instruction support not available\n";
    #endif
    
    std::cout << "\n🔥 Optimization Level: ";
    #if JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX512)
        std::cout << "MAXIMUM (AVX-512 + AVX2 + BMI2)\n";
    #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX2)
        std::cout << "HIGH (AVX2 + BMI2)\n";
    #elif JSONIFIER_CHECK_FOR_AVX(JSONIFIER_AVX)
        std::cout << "GOOD (AVX + SIMD)\n";
    #elif JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_NEON)
        std::cout << "ARM OPTIMIZED (NEON SIMD)\n";
    #else
        std::cout << "STANDARD (Fallback mode)\n";
    #endif
    
    std::cout << "═══════════════════════════════════════════════════════════════════\n\n";
}

int main(int argc, char* argv[]) {
    try {
        // Check for command line arguments
        if (argc > 1) {
            std::string arg = argv[1];
            if (arg == "--benchmark" || arg == "-b") {
                UltimateBenchmark::ComprehensiveBenchmarkRunner runner;
                runner.runAllBenchmarks();
                return 0;
            } else if (arg == "--quick" || arg == "-q") {
                UltimateBenchmark::ComprehensiveBenchmarkRunner runner;
                runner.runQuickBenchmark();
                return 0;
            } else if (arg == "--stress" || arg == "-s") {
                UltimateBenchmark::ComprehensiveBenchmarkRunner runner;
                runner.runStressTest();
                return 0;
            }
        }
        
        // Interactive menu
        UltimateBenchmark::ComprehensiveBenchmarkRunner runner;
        
        while (true) {
            displayMenu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore(); // Clear newline
            
            switch (choice) {
                case 1:
                    runner.runAllBenchmarks();
                    break;
                case 2:
                    runner.runQuickBenchmark();
                    break;
                case 3:
                    runner.runStressTest();
                    break;
                case 4:
                    runCPUFeatureTest();
                    break;
                case 5:
                    std::cout << "📝 Results saving not implemented in this demo\n\n";
                    break;
                case 6:
                    std::cout << "🏰 Benchmark complete! Thanks for testing RTC's Jsonifier! 🚀\n";
                    return 0;
                default:
                    std::cout << "❌ Invalid choice. Please select 1-6.\n\n";
                    break;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "🚨 Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
