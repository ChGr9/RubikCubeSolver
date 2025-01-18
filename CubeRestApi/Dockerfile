FROM maven:3.9.9-eclipse-temurin-23 as build
WORKDIR /app

COPY pom.xml ./
RUN mvn dependency:go-offline

COPY src /app/src
RUN mvn clean package

FROM eclipse-temurin:23-jdk
WORKDIR /app

COPY --from=build /app/target/*.jar /app/app.jar
EXPOSE 8080

CMD ["java", "-jar", "app.jar"]