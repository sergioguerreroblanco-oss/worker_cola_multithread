# ===========================================================
# === Stage 1: Build and Test ===============================
# ===========================================================
FROM ubuntu:22.04 AS builder

# Avoid apt interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install toolchain and CMake
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    ninja-build \
    wget \
    ca-certificates \
    tar \
 && wget -q https://github.com/Kitware/CMake/releases/download/v3.28.3/cmake-3.28.3-linux-x86_64.sh \
 && chmod +x cmake-3.28.3-linux-x86_64.sh \
 && bash ./cmake-3.28.3-linux-x86_64.sh --skip-license --prefix=/usr/local \
 && rm cmake-3.28.3-linux-x86_64.sh \
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
FROM ubuntu:22.04 AS runtime
RUN apt-get update && apt-get install -y --no-install-recommends cmake && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=builder /app/build /app/build
ENTRYPOINT ["/bin/bash"]

# Default command: run the compiled binary
CMD ["cola_worker"]

