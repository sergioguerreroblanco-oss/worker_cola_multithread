# ---------------------------------------------------------------
# Stage 1: Builder
# ---------------------------------------------------------------
FROM ubuntu:22.04 AS builder
WORKDIR /app

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git ninja-build wget \
 && rm -rf /var/lib/apt/lists/*

# Copy project
COPY . .

# Build and run tests
RUN cmake --preset release \
 && cmake --build --preset release -j$(nproc) \
 && ctest --preset release --output-on-failure

# ---------------------------------------------------------------
# Stage 2: Runtime
# ---------------------------------------------------------------
FROM ubuntu:22.04 AS runtime
WORKDIR /app

# Copy only the compiled binary
COPY --from=builder /app/build/release/cola_worker /usr/local/bin/cola_worker

# Default entrypoint
ENTRYPOINT ["cola_worker"]