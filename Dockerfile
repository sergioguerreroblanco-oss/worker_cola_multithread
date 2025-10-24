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
 && wget -qO- https://cmake.org/files/v3.28/cmake-3.28.3-linux-x86_64.tar.gz | tar --strip-components=1 -xz -C /opt/cmake \
 && ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake \
 && ln -s /opt/cmake/bin/ctest /usr/local/bin/ctest \
 && ln -s /opt/cmake/bin/cpack /usr/local/bin/cpack \
 && ln -s /opt/cmake/bin/ccmake /usr/local/bin/ccmake \
 && rm -rf /var/lib/apt/lists/*

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
