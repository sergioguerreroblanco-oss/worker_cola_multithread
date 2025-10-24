# ---------------------------------------------------------------
# Stage 1: Builder
# ---------------------------------------------------------------
FROM ubuntu:22.04 AS builder
WORKDIR /app

# Install toolchain + CMake from apt (Kitware backport)
RUN apt-get update && apt-get install -y --no-install-recommends \
    software-properties-common ca-certificates gnupg build-essential git ninja-build wget tar \
 && mkdir -p /etc/apt/keyrings \
 && wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor -o /etc/apt/keyrings/kitware-archive-keyring.gpg \
 && echo "deb [signed-by=/etc/apt/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main" > /etc/apt/sources.list.d/kitware.list \
 && apt-get update \
 && apt-get install -y cmake \
 && cmake --version \
 && rm -rf /var/lib/apt/lists/*

# Copy project
COPY . .

# Build & run tests
RUN cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release -G "Ninja" \
 && cmake --build build/release -j$(nproc) \
 && cd build/release && ctest --output-on-failure

# ---------------------------------------------------------------
# Stage 2: Runtime
# ---------------------------------------------------------------
FROM ubuntu:22.04 AS runtime
WORKDIR /app
COPY --from=builder /app/build/release/cola_worker /usr/local/bin/cola_worker
ENTRYPOINT ["cola_worker"]
