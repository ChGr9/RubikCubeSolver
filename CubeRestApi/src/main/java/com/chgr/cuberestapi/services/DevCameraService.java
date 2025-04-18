package com.chgr.cuberestapi.services;

import org.springframework.context.annotation.Profile;
import org.springframework.stereotype.Service;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Random;

@Service
@Profile("dev")
public class DevCameraService implements CameraService {

    private final List<Path> images;
    private final Random random = new Random();

    public DevCameraService() throws IOException {
        images = Files.list(Path.of("src/test/resources"))
                .filter(path -> path.toString().endsWith(".jpg"))
                .toList();
    }

    @Override
    public InputStream capture() {
        try {
            Path imagePath = images.get(random.nextInt(images.size()));
            return Files.newInputStream(imagePath);
        } catch (IOException e) {
            throw new RuntimeException("Failed to capture image", e);
        }
    }
}
