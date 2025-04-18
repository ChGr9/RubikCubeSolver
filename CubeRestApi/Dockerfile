FROM --platform=$BUILDPLATFORM maven:3.9.9-eclipse-temurin-23 AS build
WORKDIR /app

RUN --mount=type=cache,target=/root/.m2 \
    --mount=type=cache,target=/app/target \
    true

COPY pom.xml ./
RUN --mount=type=cache,target=/root/.m2 mvn -B -q dependency:go-offline

COPY src ./src
RUN --mount=type=cache,target=/root/.m2 mvn -B -q clean package

FROM eclipse-temurin:23-jre
WORKDIR /app
COPY --from=build /app/target/*-SNAPSHOT.jar /app/app.jar
EXPOSE 8080
CMD ["java","-jar","app.jar"]
