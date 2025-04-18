package com.chgr.cuberestapi.services;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Profile;
import org.springframework.stereotype.Service;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

@Service
@Profile("raspberrypi")
public class PiCameraService implements CameraService {
    private static final Logger logger = LoggerFactory.getLogger(PiCameraService.class);
    
    private final String[] baseCmd = {
            "libcamera-still",
            "--nopreview",
            "-t", "500",
            "-o", "-"
    };
    
    private boolean checkCommand(String command) {
        try {
            ProcessBuilder which = new ProcessBuilder("which", command);
            Process process = which.start();
            int exitCode = process.waitFor();
            return exitCode == 0;
        } catch (Exception e) {
            logger.warn("Error checking for command {}: {}", command, e.getMessage());
            return false;
        }
    }

    @Override
    public InputStream capture() throws IOException {
        // First check if libcamera-still exists
        if (!checkCommand("libcamera-still")) {
            logger.error("libcamera-still command not found. Cannot capture image.");
            throw new IOException("libcamera-still command not found. Make sure it's installed in the container.");
        }
        
        try {
            logger.info("Executing camera command: {}", Arrays.toString(baseCmd));
            ProcessBuilder processBuilder = new ProcessBuilder(baseCmd);
            processBuilder.redirectErrorStream(true);
            Process process = processBuilder.start();
            return process.getInputStream();
        } catch (Exception e) {
            logger.error("Failed to capture image", e);
            throw new IOException("Failed to capture image: " + e.getMessage(), e);
        }
    }
}