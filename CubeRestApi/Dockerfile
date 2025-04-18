FROM --platform=$BUILDPLATFORM maven:3.9.9-eclipse-temurin-23 AS build
WORKDIR /app

RUN --mount=type=cache,target=/root/.m2 \
    --mount=type=cache,target=/app/target \
    true

COPY pom.xml ./
RUN --mount=type=cache,target=/root/.m2 mvn -B -q dependency:go-offline

COPY src ./src
RUN --mount=type=cache,target=/root/.m2 mvn -B -q clean package

FROM debian:bookworm AS cam-build
WORKDIR /src

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        git build-essential meson ninja-build pkg-config python3-yaml \
        libcamera-dev  libdrm-dev  libudev-dev \
        libjpeg-dev    libtiff5-dev  libpng-dev && \
    rm -rf /var/lib/apt/lists/*

RUN git clone --depth=1 https://github.com/raspberrypi/rpicam-apps /src/rpicam
WORKDIR /src/rpicam

RUN meson setup build -Dbuildtype=release && \
    ninja -C build && \
    DESTDIR=/opt/rpicam ninja -C build install

FROM eclipse-temurin:23-jre
WORKDIR /app

COPY --from=cam-build /usr/lib/          /usr/lib/
COPY --from=cam-build /usr/share/libcamera /usr/share/libcamera

RUN apt-get update && \
    apt-get install -y --no-install-recommends libudev1 libdrm2 && \
    rm -rf /var/lib/apt/lists/* \

RUN mkdir -p /app/scans && \
    chmod -R 777 /app/scans

COPY --from=build /app/target/*.jar /app/app.jar
EXPOSE 8080
CMD ["java","-jar","app.jar"]
