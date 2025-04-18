package com.chgr.cuberestapi.controllers;

import com.chgr.cuberestapi.services.CameraService;
import com.chgr.cuberestapi.services.ImageProcessor;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.server.ResponseStatusException;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;

@RestController
@RequestMapping("/scan")
public class ScanController {

    private final ImageProcessor imageProcessor;
    private final CameraService cameraService;

    public ScanController(ImageProcessor imageProcessor, CameraService cameraService) {
        this.imageProcessor = imageProcessor;
        this.cameraService = cameraService;
    }

    @GetMapping()
    public String scan() throws IOException {

        InputStream inputStream = cameraService.capture();
        if (inputStream == null) {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Image not found");
        }
        BufferedImage image = ImageIO.read(inputStream);
        String result = imageProcessor.process(image);
        File outFile = new File("scans/" + result + ".jpg");
        outFile.getParentFile().mkdirs();
        ImageIO.write(image, "jpg", outFile);
        return result;
    }

    @GetMapping(value = "/debug", produces = MediaType.IMAGE_JPEG_VALUE, consumes = MediaType.MULTIPART_FORM_DATA_VALUE)
    public byte[] scanDebug(@RequestParam("image")MultipartFile imageFile) throws IOException {
        InputStream inputStream = imageFile.getInputStream();
        BufferedImage image = ImageIO.read(inputStream);
        return imageProcessor.processDebug(image);
    }
}
