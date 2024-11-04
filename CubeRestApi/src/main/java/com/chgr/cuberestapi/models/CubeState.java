package com.chgr.cuberestapi.models;

import lombok.Getter;

import java.util.*;
import java.util.stream.Collectors;

@Getter
public class CubeState {
    private final int[] cubeletPermutations = new int[26];
    private final int[] cubeletOrientations = new int[26];
    private final int[] faceOrientations = new int[6];

    private static final int[][] coordinates = {
            {38, 9, 20},
            {44, 18, 2},
            {42, 0, 29},
            {36, 27, 11},
            {53, 26, 15},
            {47, 8, 24},
            {45, 35, 6},
            {51, 17, 33},
            {41, 19},
            {43, 1},
            {39, 28},
            {37, 10},
            {23, 12},
            {21, 5},
            {32, 3},
            {30, 14},
            {50, 25},
            {46, 7},
            {48, 34},
            {52, 16}
    };

    private static final CubeSides[][] pieces = {
            {CubeSides.UP, CubeSides.BACK, CubeSides.RIGHT},
            {CubeSides.UP, CubeSides.RIGHT, CubeSides.FRONT},
            {CubeSides.UP, CubeSides.FRONT, CubeSides.LEFT},
            {CubeSides.UP, CubeSides.LEFT, CubeSides.BACK},
            {CubeSides.DOWN, CubeSides.RIGHT, CubeSides.BACK},
            {CubeSides.DOWN, CubeSides.FRONT, CubeSides.RIGHT},
            {CubeSides.DOWN, CubeSides.LEFT, CubeSides.FRONT},
            {CubeSides.DOWN, CubeSides.BACK, CubeSides.LEFT},
            {CubeSides.UP, CubeSides.RIGHT},
            {CubeSides.UP, CubeSides.FRONT},
            {CubeSides.UP, CubeSides.LEFT},
            {CubeSides.UP, CubeSides.BACK},
            {CubeSides.RIGHT, CubeSides.BACK},
            {CubeSides.RIGHT, CubeSides.FRONT},
            {CubeSides.LEFT, CubeSides.FRONT},
            {CubeSides.LEFT, CubeSides.BACK},
            {CubeSides.DOWN, CubeSides.RIGHT},
            {CubeSides.DOWN, CubeSides.FRONT},
            {CubeSides.DOWN, CubeSides.LEFT},
            {CubeSides.DOWN, CubeSides.BACK}
    };

    public CubeState(String cubeStr){
        Map<Character, Integer> colorCounter = new HashMap<>();
        Map<Character, Integer> colorMap = new HashMap<>();
        for(int i = 0; i < cubeStr.length(); i++){
            char color = cubeStr.charAt(i);
            colorCounter.put(color, colorCounter.getOrDefault(color, 0) + 1);

            if(i % 9 == 4){
                if(colorMap.containsKey(color))
                    throw new IllegalArgumentException("Cube has duplicate centers");
                colorMap.put(color, i / 9);
            }
        }

        if(colorCounter.size() != 6)
            throw new IllegalArgumentException("Cube has missing centers");

        Optional<String> incorrectNumberOfColors = colorCounter.entrySet().stream()
                .filter(entry -> entry.getValue() != 9)
                .map(entry -> entry.getKey().toString())
                .reduce((a, b) -> a + ", " + b);
        if(incorrectNumberOfColors.isPresent())
            throw new IllegalArgumentException("Cube has incorrect number of colors: " + incorrectNumberOfColors.get());

        for(int i = 0; i < coordinates.length; i++){
            Set<Integer> currentCubelet = Arrays.stream(coordinates[i])
                    .mapToObj(j -> colorMap.get(cubeStr.charAt(j)))
                    .collect(Collectors.toSet());
            int firstColor = colorMap.get(cubeStr.charAt(coordinates[i][0]));

            boolean found = false;
            for(int j = 0; j < pieces.length; j++){
                Set<Integer> currentPiece = Arrays.stream(pieces[j])
                        .map(CubeSides::ordinal)
                        .collect(Collectors.toSet());
                if(currentCubelet.equals(currentPiece)){
                    cubeletPermutations[i] = j;
                    cubeletOrientations[i] = Arrays.stream(pieces[j]).map(Enum::ordinal).toList().indexOf(firstColor);
                    found = true;
                    break;
                }
            }
            if(!found)
                throw new IllegalArgumentException("Cube has invalid stickers");
        }

        for(int i =0; i < 6; i++){
            cubeletPermutations[20 + i] = i + 20;
            cubeletOrientations[20 + i] = 0;
            faceOrientations[i] = 0;
        }
    }

    public boolean isValid() {
        int cornerOrientationSum = 0;
        for (int i = 0; i < 8; i++)
            cornerOrientationSum += cubeletOrientations[i];
        if (cornerOrientationSum % 3 != 0)
            return false;

        int edgeOrientationSum = 0;
        for (int i = 8; i < 20; i++)
            edgeOrientationSum += cubeletOrientations[i];
        if (edgeOrientationSum % 2 != 0)
            return false;

        return !parityOdd(cubeletPermutations, 20);
    }

    private boolean parityOdd(int[] arr, int len) {
        boolean odd = false;
        for (int i = 0; i < len; i++)
            for (int j = 0; j < len; j++)
                odd ^= arr[i] < arr[j];

        return odd;
    }
}
