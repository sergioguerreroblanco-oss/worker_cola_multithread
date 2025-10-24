# ===========================================================
# === Stage 1: Build and Test ===============================
# ===========================================================
FROM ubuntu:22.04 AS builder

# Avoid apt interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install toolchain and latest CMake
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    ninja-build \
    wget \
 && mkdir -p /opt/cmake \
 && wget -qO /tmp/cmake.tar.gz https://github.com/Kitware/CMake/releases/download/v3.28.3/cmake-3.28.3-linux-x86_64.tar.gz \
 && tar -xzf /tmp/cmake.tar.gz -C /opt/cmake --strip-components=0 \
 && cp -r /opt/cmake/cmake-3.28.3-linux-x86_64/bin/* /usr/local/bin/ \
 && cp -r /opt/cmake/cmake-3.28.3-linux-x86_64/share/* /usr/local/share/ \
 && rm -rf /tmp/cmake.tar.gz /opt/cmake /var/lib/apt/lists/*

# Working directory
WORKDIR /app

# Copy source
COPY . .

# Configure and build using CMake presets
RUN cmake --preset release \
 && cmake --build --preset release -j$(nproc) \
 && ctest --preset release --output-on-failure

# ===========================================================
# === Stage 2: Runtime image (minimal) ======================
# ===========================================================
FROM ubuntu:22.04

# Copy compiled binaries only (lightweight runtime)
WORKDIR /app
COPY --from=builder /app/build/release/cola_worker /usr/local/bin/cola_worker

# Default command: run the compiled binary
CMD ["cola_worker"]
