FROM --platform=$BUILDPLATFORM maven:3.9.9-eclipse-temurin-23 AS build
WORKDIR /app

RUN --mount=type=cache,target=/root/.m2 \
    --mount=type=cache,target=/app/target true

COPY pom.xml ./
RUN --mount=type=cache,target=/root/.m2 mvn -q -B dependency:go-offline

COPY src ./src
RUN --mount=type=cache,target=/root/.m2 mvn -q -B clean package

FROM --platform=$TARGETPLATFORM balenalib/raspberrypi3-debian:bullseye-build AS pi-base
RUN apt-get update && \
    apt-get install -y --no-install-recommends libcamera-apps && \
    rm -rf /var/lib/apt/lists/*

FROM --platform=$TARGETPLATFORM eclipse-temurin:23-jre
WORKDIR /app

COPY --from=pi-base /usr/bin/libcamera-still /usr/bin/libcamera-still
COPY --from=pi-base /usr/lib/libcamera* /usr/lib/
COPY --from=pi-base /usr/share/libcamera/ipa /usr/share/libcamera/ipa

RUN apt-get update && \
    apt-get install -y --no-install-recommends libdrm2 libudev1 && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir -p /app/scans && chmod 777 /app/scans

COPY --from=build /app/target/*.jar /app/app.jar
EXPOSE 8080
CMD ["java","-jar","app.jar"]
