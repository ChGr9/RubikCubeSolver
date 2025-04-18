package com.chgr.cuberestapi.services;

import org.springframework.context.annotation.Profile;
import org.springframework.stereotype.Service;

import java.io.InputStream;

@Service
@Profile("raspberrypi")
public class PiCameraService implements CameraService {
    private final String[] baseCmd = {
            "libcamera-still",
            "--nopreview",
            "-t", "500",
            "-o", "-"
    };

    public InputStream capture() {
        try {
            ProcessBuilder processBuilder = new ProcessBuilder(baseCmd);
            processBuilder.redirectErrorStream(true);
            Process process = processBuilder.start();
            return process.getInputStream();
        } catch (Exception e) {
            throw new RuntimeException("Failed to capture image", e);
        }
    }
}