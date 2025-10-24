# ===========================================================
# === Stage 1: Build and Test ===============================
# ===========================================================
FROM ubuntu:22.04 AS builder

# Avoid apt interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build essentials and CMake
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
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
