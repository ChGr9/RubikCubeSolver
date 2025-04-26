package com.chgr.cuberestapi.controllers;

import com.chgr.cuberestapi.services.CameraService;
import com.chgr.cuberestapi.services.ImageProcessor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.server.ResponseStatusException;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.InputStream;

@RestController
@RequestMapping("/scan")
public class ScanController {
    private static final Logger logger = LoggerFactory.getLogger(ScanController.class);

    private final ImageProcessor imageProcessor;
    private final CameraService cameraService;

    public ScanController(ImageProcessor imageProcessor, CameraService cameraService) {
        this.imageProcessor = imageProcessor;
        this.cameraService = cameraService;
    }

    @GetMapping()
    public String scan() {
        try {
            InputStream inputStream = cameraService.capture();
            if (inputStream == null) {
                throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Image not found");
            }
            BufferedImage image = ImageIO.read(inputStream);
            if (image == null) {
                throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "Invalid image format");
            }
            String result = imageProcessor.process(image);
            File outFile = new File("scans/" + result + ".jpg");
            outFile.getParentFile().mkdirs();
            ImageIO.write(image, "jpg", outFile);
            return result;
        } catch (Exception e) {
            logger.error("Error during scan: {}", e.getMessage(), e);
            throw new ResponseStatusException(HttpStatus.INTERNAL_SERVER_ERROR, "Error during scan: " + e.getMessage(), e);
        }
    }

    @PostMapping(value = "/debug", produces = MediaType.IMAGE_JPEG_VALUE, consumes = MediaType.MULTIPART_FORM_DATA_VALUE)
    public byte[] scanDebug(@RequestParam("image") MultipartFile imageFile) {
        try {
            InputStream inputStream = imageFile.getInputStream();
            BufferedImage image = ImageIO.read(inputStream);
            if (image == null) {
                throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "Invalid image format");
            }
            return imageProcessor.processDebug(image);
        } catch (Exception e) {
            logger.error("Error during debug scan: {}", e.getMessage(), e);
            throw new ResponseStatusException(HttpStatus.INTERNAL_SERVER_ERROR, "Error during debug scan: " + e.getMessage(), e);
        }
    }
}
