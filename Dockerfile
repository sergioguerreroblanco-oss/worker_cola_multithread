# Stable base image
FROM ubuntu:22.04

# Avoid apt interactives prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install C++ and CMake toolchain
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
 && rm -rf /var/lib/apt/lists/*

# Copy the code into the container
WORKDIR /app
COPY . .

# Compilation out of the tree (build/) in release mode
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON \
 && cmake --build build -j \
 && ctest --test-dir build --output-on-failure

# Show the tests errors directly in the output
ENV CTEST_OUTPUT_ON_FAILURE=1

# By default, execute the container, run the tests
CMD ["ctest", "--test-dir", "build"]
