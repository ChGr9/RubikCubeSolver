package com.chgr.cuberestapi.services;

import org.apache.commons.io.FilenameUtils;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.stream.Stream;

import static org.junit.jupiter.api.Assertions.*;

class ImageProcessorTest {

    static Stream<Path> imageFiles() throws IOException, URISyntaxException {
        URL url = ImageProcessorTest.class.getResource("/");
        assertNotNull(url);

        Path dir = Path.of(url.toURI());
        return Files.list(dir).filter(path -> FilenameUtils.isExtension(path.toString(), "jpg", "jpeg", "png"));
    }

    @ParameterizedTest
    @MethodSource("imageFiles")
    void process(Path imagePath) throws IOException {
        assertNotNull(imagePath);
        assertTrue(Files.exists(imagePath));

        ImageProcessor imageProcessor = new ImageProcessor();
        BufferedImage image = ImageIO.read(imagePath.toFile());
        String result = imageProcessor.process(image);
        assertNotNull(result);
        assertFalse(result.isEmpty());
        assertEquals(9, result.length(), "Result should be 9 characters long");
        assertTrue(FilenameUtils.getBaseName(imagePath.getFileName().toString()).equalsIgnoreCase(result), "Result should match the file name");
    }
}