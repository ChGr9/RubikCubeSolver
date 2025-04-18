FROM maven:3.9.9-eclipse-temurin-23 AS build
WORKDIR /app
RUN --mount=type=cache,target=/root/.m2 \
    --mount=type=cache,target=/app/target true
COPY pom.xml ./
RUN --mount=type=cache,target=/root/.m2 mvn -q -B dependency:go-offline
COPY src ./src
RUN --mount=type=cache,target=/root/.m2 mvn -q -B clean package

ARG TARGETPLATFORM
FROM --platform=$TARGETPLATFORM balenalib/raspberrypi5-debian:bullseye-build AS pi-base

RUN apt-get update && \
    # 1) Install APT helpers
    apt-get install -y --no-install-recommends \
      apt-transport-https ca-certificates gnupg curl && \
    # 2) Add the Raspbian archive key
    curl -fsSL https://archive.raspberrypi.org/debian/raspberrypi.gpg.key \
      | apt-key add - && \
    # 3) Point at the Raspbian Bullseye repo (main + contrib + non-free + rpi)
    printf "deb http://raspbian.raspberrypi.org/raspbian/ bullseye main contrib non-free rpi\n" \
      > /etc/apt/sources.list.d/raspi.list && \
    # 4) Install the Pi‑camera apps
    apt-get update && \
    apt-get install -y --no-install-recommends rpicam-apps udev && \
    rm -rf /var/lib/apt/lists/*

FROM ibm-semeru-runtimes:open-23-jre-jammy
WORKDIR /app

COPY --from=pi-base /usr/bin/libcamera-still /usr/bin/
COPY --from=pi-base /usr/lib/aarch64-linux-gnu/libcamera* /usr/lib/aarch64-linux-gnu/
COPY --from=pi-base /usr/share/libcamera /usr/share/libcamera

RUN apt-get update && \
    apt-get install -y --no-install-recommends libdrm2 libudev1 && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir -p /app/scans && chmod 777 /app/scans

COPY --from=build /app/target/*.jar /app/app.jar
EXPOSE 8080
CMD ["java","-jar","app.jar"]
