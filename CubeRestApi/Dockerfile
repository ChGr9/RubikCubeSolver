FROM maven:3.9.9-eclipse-temurin-23 AS build
WORKDIR /app

RUN --mount=type=cache,target=/root/.m2 \
    --mount=type=cache,target=/app/target true

COPY pom.xml ./
RUN --mount=type=cache,target=/root/.m2 mvn -q -B dependency:go-offline

COPY src ./src
RUN --mount=type=cache,target=/root/.m2 mvn -q -B clean package

# Use Raspberry Pi 4 image (64-bit)
FROM balenalib/raspberrypi4-64-debian:bookworm

# Add the Balena repository in the sources
RUN curl -L -s https://dl.cloudsmith.io/public/balena/libraries/setup.deb.sh | bash && \
    apt-get update

# Install basic tools and Java 23
RUN apt-get update && apt-get install -y wget apt-transport-https gnupg curl && \
    wget -O - https://packages.adoptium.net/artifactory/api/gpg/key/public | apt-key add - && \
    echo "deb https://packages.adoptium.net/artifactory/deb $(awk -F= '/^VERSION_CODENAME/{print$2}' /etc/os-release) main" | tee /etc/apt/sources.list.d/adoptium.list && \
    apt-get update && \
    apt-get install -y temurin-23-jre

# Install camera-related packages
RUN apt-get update && \
    apt-get install -y libcamera-dev libcamera-tools v4l-utils && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Create a wrapper script for libcamera-still if it's not available
RUN if ! which libcamera-still; then \
        echo '#!/bin/bash' > /usr/local/bin/libcamera-still && \
        echo 'echo "Simulating libcamera-still capture"' >> /usr/local/bin/libcamera-still && \
        echo 'output_file=""' >> /usr/local/bin/libcamera-still && \
        echo 'for arg in "$@"; do' >> /usr/local/bin/libcamera-still && \
        echo '  if [[ "$arg" == "-o" ]]; then continue; fi' >> /usr/local/bin/libcamera-still && \
        echo '  if [[ "$output_file" == "next" ]]; then output_file="$arg"; continue; fi' >> /usr/local/bin/libcamera-still && \
        echo '  if [[ "$arg" == "-o" ]]; then output_file="next"; fi' >> /usr/local/bin/libcamera-still && \
        echo 'done' >> /usr/local/bin/libcamera-still && \
        echo 'if [[ -n "$output_file" && "$output_file" != "-" ]]; then' >> /usr/local/bin/libcamera-still && \
        echo '  cp /usr/local/share/sample.jpg "$output_file"' >> /usr/local/bin/libcamera-still && \
        echo '  echo "Created sample image at $output_file"' >> /usr/local/bin/libcamera-still && \
        echo 'else' >> /usr/local/bin/libcamera-still && \
        echo '  cat /usr/local/share/sample.jpg' >> /usr/local/bin/libcamera-still && \
        echo 'fi' >> /usr/local/bin/libcamera-still && \
        chmod +x /usr/local/bin/libcamera-still && \
        mkdir -p /usr/local/share && \
        echo 'Creating sample image file...' && \
        convert -size 640x480 xc:blue /usr/local/share/sample.jpg || \
        (apt-get update && apt-get install -y imagemagick && \
         convert -size 640x480 xc:blue /usr/local/share/sample.jpg); \
    fi

WORKDIR /app

RUN mkdir -p /app/scans && chmod 777 /app/scans

COPY --from=build /app/target/*.jar /app/app.jar
EXPOSE 8080
CMD ["java","-jar","app.jar"]
