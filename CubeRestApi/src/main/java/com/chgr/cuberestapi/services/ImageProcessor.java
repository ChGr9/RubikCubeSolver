package com.chgr.cuberestapi.services;

import org.springframework.stereotype.Service;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Map;

@Service
public class ImageProcessor {

    private static final Map<Character, Color> COLOR_MAP = Map.of(
            'W', new Color(240, 240, 240), // Off-white
            'Y', new Color(210, 210, 30),  // Yellow
            'R', new Color(200, 20, 20),   // Red
            'O', new Color(230, 100, 20),  // Brighter orange with higher value
            'G', new Color(30, 160, 30),   // Green
            'B', new Color(20, 20, 200)    // Blue
    );

    private static final Rectangle[] SCAN_AREAS = {
            new Rectangle(925, 150, 350, 275),
            new Rectangle(1475, 150, 300, 250),
            new Rectangle(1925, 225, 200, 250),
            new Rectangle(900, 550, 375, 425),
            new Rectangle(1450, 550, 375, 450),
            new Rectangle(2000, 600, 250, 375),
            new Rectangle(875, 1100, 400, 450),
            new Rectangle(1450, 1125, 400, 425),
            new Rectangle(1975, 1100, 275, 350),
    };

    public String process(BufferedImage image) {
        StringBuilder sb = new StringBuilder();
        for(Rectangle area : SCAN_AREAS) {
            Color averageColor = getAverageColor(image, area);

            float[] hsb = Color.RGBtoHSB(
                    averageColor.getRed(),
                    averageColor.getGreen(),
                    averageColor.getBlue(),
                    null);

            char colorChar = colorToChar(hsb);
            sb.append(colorChar);
            System.out.printf("Average color for area %s: %s, HSV: [%.2f, %.2f, %.2f], mapped to %s\n",
                    fmtRect(area), fmtColor(averageColor), hsb[0], hsb[1], hsb[2], colorChar);
        }
        return sb.toString();
    }

    public byte[] processDebug(BufferedImage image) throws IOException {
        Graphics2D graphics = image.createGraphics();
        graphics.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        graphics.setStroke(new BasicStroke(5));
        graphics.setFont(new Font("Arial", Font.BOLD, 40));
        for(Rectangle area : SCAN_AREAS) {
            Color averageColor = getAverageColor(image, area);
            
            float[] hsb = Color.RGBtoHSB(
                    averageColor.getRed(), 
                    averageColor.getGreen(), 
                    averageColor.getBlue(), 
                    null);
            
            char colorChar = colorToChar(hsb);
            graphics.setColor(averageColor);
            graphics.fillRect(area.x, area.y, area.width, area.height);
            graphics.setColor(Color.BLACK);
            graphics.drawString(String.valueOf(colorChar), area.x + area.width / 2, area.y + area.height / 2);
            
            // Add HSV overlay
            graphics.setFont(new Font("Arial", Font.PLAIN, 16));
            graphics.drawString(String.format("H:%.2f S:%.2f V:%.2f", 
                    hsb[0], hsb[1], hsb[2]), 
                    area.x + 10, area.y + area.height - 10);
            graphics.setFont(new Font("Arial", Font.BOLD, 40));

            System.out.printf("Average color for area %s: %s, HSV: [%.2f, %.2f, %.2f], mapped to %s\n", 
                    fmtRect(area), fmtColor(averageColor), hsb[0], hsb[1], hsb[2], colorChar);
        }
        graphics.dispose();
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        ImageIO.write(image, "jpg", outputStream);
        return outputStream.toByteArray();
    }

    private char colorToChar(float[] hsb) {
        System.out.printf("HSV: [%.3f, %.3f, %.3f]\n", hsb[0], hsb[1], hsb[2]);

        if (hsb[1] < 0.1 && hsb[2] > 0.7) {
            return 'W'; // Low saturation, high brightness = white
        }
        if (hsb[1] < 0.2 && hsb[2] < 0.5) {
            return 'W'; // Low saturation, low brightness = likely white in shadow
        }

        if ((hsb[0] >= 0.025 && hsb[0] < 0.08) && hsb[1] > 0.6 && hsb[2] > 0.7) {
            return 'O';
        }
        

        if ((hsb[0] >= 0.95 || hsb[0] < 0.025) && hsb[1] > 0.4) {
            return 'R'; // Red (narrower range at beginning of hue circle)
        }
        // Orange has more yellow in it than red
        if (hsb[0] >= 0.025 && hsb[0] < 0.1 && hsb[1] > 0.4) {
            return 'O'; // Orange (wider range - 0.025-0.1)
        }
        if (hsb[0] >= 0.1 && hsb[0] < 0.2 && hsb[1] > 0.4) {
            return 'Y'; // Yellow (around 0.1-0.2) 
        }
        if (hsb[0] >= 0.2 && hsb[0] < 0.45 && hsb[1] > 0.3) {
            return 'G'; // Green (around 0.2-0.45)
        }
        if (hsb[0] >= 0.45 && hsb[0] < 0.7 && hsb[1] > 0.3) {
            return 'B'; // Blue (around 0.45-0.7)
        }
        
        return findClosestColorHSV(hsb);
    }
    
    private char findClosestColorHSV(float[] hsb) {
        double minDistance = Double.MAX_VALUE;
        char closestColor = 'W'; // Default
        
        boolean nearRedOrangeBoundary = (hsb[0] >= 0.01 && hsb[0] <= 0.05) || hsb[0] >= 0.95;
        
        for (Map.Entry<Character, Color> entry : COLOR_MAP.entrySet()) {
            Color refColor = entry.getValue();
            float[] refHsb = Color.RGBtoHSB(
                    refColor.getRed(), 
                    refColor.getGreen(), 
                    refColor.getBlue(), 
                    null);

            double hueDiff = Math.min(
                    Math.abs(hsb[0] - refHsb[0]), 
                    1 - Math.abs(hsb[0] - refHsb[0])
            );
            
            double hueWeight = 5.0;
            double satWeight = 3.0;
            double valWeight = 2.0;

            if (nearRedOrangeBoundary && entry.getKey() == 'O') {
                hueWeight = 3.0;
                valWeight = 4.0;
            }
            
            double distance = hueDiff * hueWeight + 
                    Math.abs(hsb[1] - refHsb[1]) * satWeight + 
                    Math.abs(hsb[2] - refHsb[2]) * valWeight;
            
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = entry.getKey();
            }
        }
        
        return closestColor;
    }

    private Color getAverageColor(BufferedImage image, Rectangle area) {
        int red = 0, green = 0, blue = 0;
        int count = 0;
        for (int x = area.x; x < area.x + area.width; x++) {
            for (int y = area.y; y < area.y + area.height; y++) {
                Color color = new Color(image.getRGB(x, y));
                red += color.getRed();
                green += color.getGreen();
                blue += color.getBlue();
                count++;
            }
        }
        return new Color(red / count, green / count, blue / count);
    }

    public static String fmtRect(Rectangle r) {
        return String.format("[%d,%d %dx%d]",
                r.x, r.y, r.width, r.height);
    }

    public static String fmtColor(Color c) {
        return String.format("[r=%d,g=%d,b=%d]",
                c.getRed(), c.getGreen(), c.getBlue());
    }
}
