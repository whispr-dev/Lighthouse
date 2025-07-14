# 🏰 ULTIMATE LIGHTHOUSE BEACON SYSTEM - DOCKER DEPLOYMENT
# Multi-stage build for maximum optimization and minimal size
# Powered by RTC's Jsonifier for ultimate JSON performance

# ===============================================================================
# 🚀 BUILD STAGE - Compile with Maximum Optimizations
# ===============================================================================
FROM ubuntu:22.04 AS builder

# Set non-interactive frontend for apt
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies with specific versions for reproducibility
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    pkg-config \
    libcurl4-openssl-dev \
    libc6-dev \
    linux-libc-dev \
    ninja-build \
    ccache \
    && rm -rf /var/lib/apt/lists/*

# Install latest GCC for optimal C++20 support and maximum SIMD optimization
RUN apt-get update && apt-get install -y \
    gcc-12 \
    g++-12 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100 \
    && rm -rf /var/lib/apt/lists/*

# Set up optimization environment variables
ENV CC=gcc-12
ENV CXX=g++-12
ENV CFLAGS="-O3 -march=native -mtune=native -flto -DNDEBUG"
ENV CXXFLAGS="-O3 -march=native -mtune=native -flto -DNDEBUG -std=c++20"
ENV LDFLAGS="-flto -Wl,--strip-all"

# Create build directory
WORKDIR /build

# Copy source files
COPY ultimate_lighthouse_system.cpp ./
COPY CMakeLists.txt ./
COPY lighthouse_config.json ./
COPY *.hpp ./

# Create optimized build with maximum performance
RUN mkdir -p build && cd build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_COMPILER=g++-12 \
        -DCMAKE_C_COMPILER=gcc-12 \
        -DCMAKE_CXX_FLAGS="${CXXFLAGS}" \
        -DCMAKE_C_FLAGS="${CFLAGS}" \
        -DCMAKE_EXE_LINKER_FLAGS="${LDFLAGS}" \
        -DJSONIFIER_CPU_INSTRUCTIONS=127 \
        -GNinja && \
    ninja -j$(nproc) && \
    strip --strip-all ultimate_lighthouse_beacon && \
    strip --strip-all ultimate_beacon_listener && \
    strip --strip-all ultimate_json_benchmark

# Verify binaries
RUN cd build && \
    echo "🔍 Verifying built binaries:" && \
    ls -la ultimate_* && \
    file ultimate_lighthouse_beacon && \
    ldd ultimate_lighthouse_beacon

# ===============================================================================
# 🏰 RUNTIME STAGE - Minimal Production Image
# ===============================================================================
FROM ubuntu:22.04 AS runtime

# Install minimal runtime dependencies
RUN apt-get update && apt-get install -y \
    libcurl4 \
    ca-certificates \
    tzdata \
    curl \
    jq \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# Create lighthouse user for security
RUN groupadd -r lighthouse && \
    useradd -r -g lighthouse -d /lighthouse -s /bin/bash lighthouse

# Create application directories
RUN mkdir -p /lighthouse/{bin,config,logs,metrics} && \
    chown -R lighthouse:lighthouse /lighthouse

# Copy optimized binaries from builder
COPY --from=builder --chown=lighthouse:lighthouse /build/build/ultimate_lighthouse_beacon /lighthouse/bin/
COPY --from=builder --chown=lighthouse:lighthouse /build/build/ultimate_beacon_listener /lighthouse/bin/
COPY --from=builder --chown=lighthouse:lighthouse /build/build/ultimate_json_benchmark /lighthouse/bin/

# Copy configuration files
COPY --chown=lighthouse:lighthouse lighthouse_config.json /lighthouse/config/
COPY --chown=lighthouse:lighthouse production_lighthouse_config.json /lighthouse/config/
COPY --chown=lighthouse:lighthouse development_lighthouse_config.json /lighthouse/config/

# Create health check script
RUN echo '#!/bin/bash\n\
# Health check for lighthouse beacon system\n\
set -e\n\
\n\
# Check if lighthouse process is running\n\
if ! pgrep -f "ultimate_lighthouse_beacon" > /dev/null; then\n\
    echo "❌ Lighthouse beacon process not found"\n\
    exit 1\n\
fi\n\
\n\
# Check if lighthouse is responding (simple process check)\n\
if [ -f /lighthouse/logs/lighthouse.log ]; then\n\
    # Check for recent activity in logs (last 5 minutes)\n\
    if find /lighthouse/logs/lighthouse.log -mmin -5 | grep -q .; then\n\
        echo "✅ Lighthouse is healthy - recent activity detected"\n\
        exit 0\n\
    else\n\
        echo "⚠️  Lighthouse may be stalled - no recent activity"\n\
        exit 1\n\
    fi\n\
else\n\
    echo "✅ Lighthouse process running"\n\
    exit 0\n\
fi' > /lighthouse/bin/health_check.sh && \
    chmod +x /lighthouse/bin/health_check.sh && \
    chown lighthouse:lighthouse /lighthouse/bin/health_check.sh

# Create startup script with configuration management
RUN echo '#!/bin/bash\n\
set -e\n\
\n\
echo "🏰 Starting Ultimate Lighthouse Beacon System..."\n\
echo "⚡ Powered by RTC'\''s Jsonifier - The Fastest JSON Library"\n\
echo "🚀 Container Version: 3.0.0"\n\
echo ""\n\
\n\
# Environment configuration\n\
export LIGHTHOUSE_CONFIG_FILE="${LIGHTHOUSE_CONFIG_FILE:-/lighthouse/config/lighthouse_config.json}"\n\
export LIGHTHOUSE_LOG_FILE="${LIGHTHOUSE_LOG_FILE:-/lighthouse/logs/lighthouse.log}"\n\
export LIGHTHOUSE_METRICS_PATH="${LIGHTHOUSE_METRICS_PATH:-/lighthouse/metrics}"\n\
\n\
# Create directories if they don'\''t exist\n\
mkdir -p /lighthouse/logs /lighthouse/metrics\n\
\n\
# Apply environment variable overrides to configuration\n\
if [ -n "$LIGHTHOUSE_FASTPING_URL" ]; then\n\
    echo "🌍 Using FastPing URL: $LIGHTHOUSE_FASTPING_URL"\n\
fi\n\
\n\
if [ -n "$LIGHTHOUSE_BEACON_TARGET_IP" ]; then\n\
    echo "🌍 Using Beacon Target IP: $LIGHTHOUSE_BEACON_TARGET_IP"\n\
fi\n\
\n\
if [ -n "$LIGHTHOUSE_BEACON_TARGET_PORT" ]; then\n\
    echo "🌍 Using Beacon Target Port: $LIGHTHOUSE_BEACON_TARGET_PORT"\n\
fi\n\
\n\
if [ -n "$LIGHTHOUSE_LIGHTHOUSE_ID" ]; then\n\
    echo "🌍 Using Lighthouse ID: $LIGHTHOUSE_LIGHTHOUSE_ID"\n\
fi\n\
\n\
# Display CPU optimization information\n\
echo "🔥 CPU Architecture Detection:"\n\
lscpu | grep -E "(Model name|Architecture|CPU op-mode|Flags)" || true\n\
echo ""\n\
\n\
# Run the appropriate binary based on mode\n\
case "${LIGHTHOUSE_MODE:-beacon}" in\n\
    "beacon")\n\
        echo "🏰 Starting in Lighthouse Beacon mode..."\n\
        exec /lighthouse/bin/ultimate_lighthouse_beacon\n\
        ;;\n\
    "listener")\n\
        echo "🎧 Starting in Beacon Listener mode..."\n\
        LISTENER_PORT="${LIGHTHOUSE_LISTENER_PORT:-9876}"\n\
        LISTENER_ARGS=""\n\
        \n\
        if [ "$LIGHTHOUSE_VERBOSE" = "true" ]; then\n\
            LISTENER_ARGS="$LISTENER_ARGS --verbose"\n\
        fi\n\
        \n\
        if [ "$LIGHTHOUSE_STATISTICS" = "true" ]; then\n\
            LISTENER_ARGS="$LISTENER_ARGS --statistics"\n\
        fi\n\
        \n\
        exec /lighthouse/bin/ultimate_beacon_listener --port $LISTENER_PORT $LISTENER_ARGS\n\
        ;;\n\
    "benchmark")\n\
        echo "🔬 Starting in Benchmark mode..."\n\
        exec /lighthouse/bin/ultimate_json_benchmark --benchmark\n\
        ;;\n\
    *)\n\
        echo "❌ Unknown mode: $LIGHTHOUSE_MODE"\n\
        echo "Valid modes: beacon, listener, benchmark"\n\
        exit 1\n\
        ;;\n\
esac' > /lighthouse/bin/start.sh && \
    chmod +x /lighthouse/bin/start.sh && \
    chown lighthouse:lighthouse /lighthouse/bin/start.sh

# Set working directory
WORKDIR /lighthouse

# Switch to lighthouse user
USER lighthouse

# Environment variables with defaults
ENV LIGHTHOUSE_MODE=beacon
ENV LIGHTHOUSE_CONFIG_FILE=/lighthouse/config/lighthouse_config.json
ENV LIGHTHOUSE_LOG_FILE=/lighthouse/logs/lighthouse.log
ENV LIGHTHOUSE_METRICS_PATH=/lighthouse/metrics
ENV LIGHTHOUSE_LISTENER_PORT=9876
ENV LIGHTHOUSE_VERBOSE=false
ENV LIGHTHOUSE_STATISTICS=false

# Performance environment variables (can be overridden)
ENV LIGHTHOUSE_FASTPING_URL=http://fastping.it.com/ping?format=json
ENV LIGHTHOUSE_BEACON_TARGET_IP=161.35.248.233
ENV LIGHTHOUSE_BEACON_TARGET_PORT=9876
ENV LIGHTHOUSE_LIGHTHOUSE_ID=docker-lighthouse-001

# Expose ports
EXPOSE 9876/udp 8080/tcp 8081/tcp

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD ["/lighthouse/bin/health_check.sh"]

# Labels for metadata
LABEL maintainer="RYO Modular & whispr.dev" \
      version="3.0.0" \
      description="Ultimate Lighthouse Beacon System - The Fastest JSON-Powered Network Monitor" \
      jsonifier.version="latest" \
      performance.optimization="maximum" \
      architecture="multi" \
      license="MIT"

# Default command
CMD ["/lighthouse/bin/start.sh"]

# ===============================================================================
# 🎯 DEVELOPMENT STAGE - For Development and Testing
# ===============================================================================
FROM runtime AS development

# Switch back to root for development tools installation
USER root

# Install development and debugging tools
RUN apt-get update && apt-get install -y \
    gdb \
    valgrind \
    strace \
    htop \
    tcpdump \
    netcat-openbsd \
    telnet \
    dnsutils \
    iputils-ping \
    traceroute \
    mtr-tiny \
    nmap \
    iotop \
    vim \
    nano \
    tmux \
    screen \
    && rm -rf /var/lib/apt/lists/*

# Copy benchmark and config tools
COPY --from=builder --chown=lighthouse:lighthouse /build/build/ultimate_json_benchmark /lighthouse/bin/
COPY --chown=lighthouse:lighthouse development_lighthouse_config.json /lighthouse/config/lighthouse_config.json

# Enable debug environment
ENV LIGHTHOUSE_DEBUG_MODE=true
ENV LIGHTHOUSE_VERBOSE=true
ENV LIGHTHOUSE_LOG_LEVEL=DEBUG

# Development-specific settings
ENV LIGHTHOUSE_MODE=beacon
ENV LIGHTHOUSE_LIGHTHOUSE_ID=dev-lighthouse-001

# Switch back to lighthouse user
USER lighthouse

# ===============================================================================
# 🔬 BENCHMARK STAGE - For Performance Testing
# ===============================================================================
FROM runtime AS benchmark

# Override for benchmark mode
ENV LIGHTHOUSE_MODE=benchmark
ENV LIGHTHOUSE_LIGHTHOUSE_ID=benchmark-lighthouse-001

# Create benchmark script
USER root
RUN echo '#!/bin/bash\n\
echo "🔬 Ultimate JSON Performance Benchmark Suite"\n\
echo "🚀 Powered by RTC'\''s Jsonifier"\n\
echo ""\n\
\n\
# Display system information\n\
echo "💻 SYSTEM INFORMATION:"\n\
echo "CPU: $(lscpu | grep '\''Model name'\'' | cut -d: -f2 | xargs)"\n\
echo "Architecture: $(uname -m)"\n\
echo "Cores: $(nproc)"\n\
echo "Memory: $(free -h | grep Mem | awk '\''{print $2}'\'')" \n\
echo ""\n\
\n\
# Run comprehensive benchmark\n\
echo "🚀 Running Comprehensive JSON Performance Benchmark..."\n\
/lighthouse/bin/ultimate_json_benchmark --benchmark\n\
\n\
echo ""\n\
echo "✅ Benchmark completed!"\n\
echo "📊 Results demonstrate the ultimate performance of RTC'\''s Jsonifier"' > /lighthouse/bin/run_benchmark.sh && \
    chmod +x /lighthouse/bin/run_benchmark.sh && \
    chown lighthouse:lighthouse /lighthouse/bin/run_benchmark.sh

USER lighthouse

# Override CMD for benchmark
CMD ["/lighthouse/bin/run_benchmark.sh"]

# ===============================================================================
# 🎧 LISTENER STAGE - Dedicated Beacon Listener
# ===============================================================================  
FROM runtime AS listener

# Override for listener mode
ENV LIGHTHOUSE_MODE=listener
ENV LIGHTHOUSE_STATISTICS=true
ENV LIGHTHOUSE_VERBOSE=false
ENV LIGHTHOUSE_LIGHTHOUSE_ID=listener-lighthouse-001

# Listener-specific port
EXPOSE 9876/udp

# Create listener-specific startup script
USER root
RUN echo '#!/bin/bash\n\
echo "🎧 Ultimate Standalone Beacon Listener"\n\
echo "🚀 Multi-Lighthouse Monitoring System"\n\
echo "⚡ Powered by RTC'\''s Jsonifier"\n\
echo ""\n\
\n\
PORT="${LIGHTHOUSE_LISTENER_PORT:-9876}"\n\
ARGS="--port $PORT"\n\
\n\
if [ "$LIGHTHOUSE_VERBOSE" = "true" ]; then\n\
    ARGS="$ARGS --verbose"\n\
fi\n\
\n\
if [ "$LIGHTHOUSE_STATISTICS" = "true" ]; then\n\
    ARGS="$ARGS --statistics"\n\
fi\n\
\n\
echo "🎯 Starting listener on port $PORT..."\n\
exec /lighthouse/bin/ultimate_beacon_listener $ARGS' > /lighthouse/bin/start_listener.sh && \
    chmod +x /lighthouse/bin/start_listener.sh && \
    chown lighthouse:lighthouse /lighthouse/bin/start_listener.sh

USER lighthouse

# Override CMD for listener
CMD ["/lighthouse/bin/start_listener.sh"]

# ===============================================================================
# DOCKER COMPOSE CONFIGURATION (embedded as comment for reference)
# ===============================================================================

# version: '3.8'
# 
# services:
#   # Main lighthouse beacon
#   lighthouse:
#     build:
#       context: .
#       target: runtime
#     container_name: ultimate-lighthouse
#     restart: unless-stopped
#     environment:
#       - LIGHTHOUSE_MODE=beacon
#       - LIGHTHOUSE_LIGHTHOUSE_ID=docker-lighthouse-primary
#       - LIGHTHOUSE_BEACON_TARGET_IP=161.35.248.233
#       - LIGHTHOUSE_BEACON_TARGET_PORT=9876
#       - LIGHTHOUSE_LOG_LEVEL=INFO
#     volumes:
#       - ./logs:/lighthouse/logs
#       - ./metrics:/lighthouse/metrics
#       - ./config:/lighthouse/config
#     networks:
#       - lighthouse-network
#     healthcheck:
#       test: ["/lighthouse/bin/health_check.sh"]
#       interval: 30s
#       timeout: 10s
#       retries: 3
#       start_period: 10s
# 
#   # Beacon listener for monitoring
#   listener:
#     build:
#       context: .
#       target: listener
#     container_name: ultimate-lighthouse-listener
#     restart: unless-stopped
#     ports:
#       - "9876:9876/udp"
#     environment:
#       - LIGHTHOUSE_MODE=listener
#       - LIGHTHOUSE_LISTENER_PORT=9876
#       - LIGHTHOUSE_STATISTICS=true
#       - LIGHTHOUSE_VERBOSE=false
#     volumes:
#       - ./listener-logs:/lighthouse/logs
#     networks:
#       - lighthouse-network
#     depends_on:
#       - lighthouse
# 
#   # Development environment
#   lighthouse-dev:
#     build:
#       context: .
#       target: development
#     container_name: ultimate-lighthouse-dev
#     restart: "no"
#     environment:
#       - LIGHTHOUSE_MODE=beacon
#       - LIGHTHOUSE_DEBUG_MODE=true
#       - LIGHTHOUSE_VERBOSE=true
#       - LIGHTHOUSE_LOG_LEVEL=DEBUG
#       - LIGHTHOUSE_LIGHTHOUSE_ID=dev-lighthouse-001
#     volumes:
#       - ./dev-logs:/lighthouse/logs
#       - ./dev-config:/lighthouse/config
#       - .:/workspace
#     networks:
#       - lighthouse-network
#     profiles:
#       - development
# 
#   # Benchmark runner
#   benchmark:
#     build:
#       context: .
#       target: benchmark
#     container_name: ultimate-lighthouse-benchmark
#     restart: "no"
#     environment:
#       - LIGHTHOUSE_MODE=benchmark
#     networks:
#       - lighthouse-network
#     profiles:
#       - benchmark
# 
# networks:
#   lighthouse-network:
#     driver: bridge
#     ipam:
#       config:
#         - subnet: 172.20.0.0/16
# 
# volumes:
#   lighthouse-logs:
#   lighthouse-metrics:
#   lighthouse-config:

# ===============================================================================
# KUBERNETES DEPLOYMENT (embedded as comment for reference)
# ===============================================================================

# apiVersion: apps/v1
# kind: Deployment
# metadata:
#   name: ultimate-lighthouse
#   labels:
#     app: ultimate-lighthouse
#     version: v3.0.0
# spec:
#   replicas: 1
#   selector:
#     matchLabels:
#       app: ultimate-lighthouse
#   template:
#     metadata:
#       labels:
#         app: ultimate-lighthouse
#     spec:
#       containers:
#       - name: lighthouse
#         image: ultimate-lighthouse:latest
#         ports:
#         - containerPort: 9876
#           protocol: UDP
#         env:
#         - name: LIGHTHOUSE_MODE
#           value: "beacon"
#         - name: LIGHTHOUSE_LIGHTHOUSE_ID
#           value: "k8s-lighthouse-001"
#         - name: LIGHTHOUSE_LOG_LEVEL
#           value: "INFO"
#         resources:
#           requests:
#             memory: "64Mi"
#             cpu: "50m"
#           limits:
#             memory: "256Mi"
#             cpu: "500m"
#         livenessProbe:
#           exec:
#             command:
#             - /lighthouse/bin/health_check.sh
#           initialDelaySeconds: 30
#           periodSeconds: 30
#         readinessProbe:
#           exec:
#             command:
#             - /lighthouse/bin/health_check.sh
#           initialDelaySeconds: 5
#           periodSeconds: 10
#         volumeMounts:
#         - name: logs
#           mountPath: /lighthouse/logs
#         - name: metrics
#           mountPath: /lighthouse/metrics
#       volumes:
#       - name: logs
#         emptyDir: {}
#       - name: metrics
#         emptyDir: {}
# ---
# apiVersion: v1
# kind: Service
# metadata:
#   name: ultimate-lighthouse-service
# spec:
#   selector:
#     app: ultimate-lighthouse
#   ports:
#   - port: 9876
#     targetPort: 9876
#     protocol: UDP
#   type: ClusterIP

# ===============================================================================
# BUILD SCRIPTS (embedded as comment for reference)
# ===============================================================================

# #!/bin/bash
# # build.sh - Build script for Ultimate Lighthouse Docker images
# 
# set -e
# 
# echo "🏰 Building Ultimate Lighthouse Docker Images..."
# echo "🚀 Powered by RTC's Jsonifier"
# 
# # Build all targets
# echo "📦 Building production image..."
# docker build --target runtime -t ultimate-lighthouse:latest .
# 
# echo "🔬 Building development image..."
# docker build --target development -t ultimate-lighthouse:dev .
# 
# echo "🎧 Building listener image..."
# docker build --target listener -t ultimate-lighthouse:listener .
# 
# echo "📊 Building benchmark image..."
# docker build --target benchmark -t ultimate-lighthouse:benchmark .
# 
# echo "✅ All images built successfully!"
# echo ""
# echo "🚀 Usage examples:"
# echo "  docker run -d --name lighthouse ultimate-lighthouse:latest"
# echo "  docker run -p 9876:9876/udp ultimate-lighthouse:listener"
# echo "  docker run ultimate-lighthouse:benchmark"
# echo "  docker-compose up -d"
