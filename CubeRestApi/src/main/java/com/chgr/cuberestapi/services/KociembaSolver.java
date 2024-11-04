package com.chgr.cuberestapi.services;

import com.chgr.cuberestapi.models.CubeState;
import com.chgr.cuberestapi.models.MoveSequence;

import java.util.Arrays;

public class KociembaSolver {
    private int phase1Len;
    private int phase2Len;
    private int maxDepth;
    private int solutionLength;
    private final int[] solutionMoves = new int[40];
    private final int[] solutionAmounts = new int[40];
    private final int[][] positions = new int[40][10];

    private final int[][] transformationEdgeOrient = new int[2048][6];
    private final int[][] transformationCornerOrient = new int[2187][6];
    private final int[][] transformationEdgePerm = new int[40320][6];
    private final int[][] transformationCornerPerm = new int[40320][6];
    private final int[][] transformationSlice = new int[11880][6];
    private final int[][] transformationChoice = new int[2048][6];
    private final int[][] transformationSlicePerm = new int[24][6];

    private final byte[] pruneEdgeOrient = new byte[2048];
    private final byte[] pruneCornerOrient = new byte[2187];
    private final byte[] pruneEdgePerm = new byte[40320];
    private final byte[] pruneCornerPerm = new byte[40320];
    private final byte[] pruneChoice = new byte[2048];
    private final byte[] pruneSlicePerm = new byte[24];

    public KociembaSolver(){
        phase1Len = 0;
        phase2Len = 0;
        maxDepth = 25;
        solutionLength = 0;
        initializeParameters();
    }

    private void initializeParameters() {
        initializeTransformations();
        initializePrunings();
    }

    private void initializePrunings() {
        pruneEdgeOrient[0] = 1;
        initializePruneArray(pruneEdgeOrient, transformationEdgeOrient);
        pruneCornerOrient[0] = 1;
        initializePruneArray(pruneCornerOrient, transformationCornerOrient);
        pruneChoice[240] = 1;
        initializePruneArray(pruneChoice, transformationChoice);
        pruneCornerPerm[0] = 1;
        initializePruneCornerPerm(pruneCornerPerm, transformationCornerPerm);
        pruneEdgePerm[0] = 1;
        initializePruneArray(pruneEdgePerm, transformationEdgePerm);
        pruneSlicePerm[0] = 1;
        initializePruneArray(pruneSlicePerm, transformationSlicePerm);
    }

    private void initializePruneArray(byte[] pruneArray, int[][] transformationArray) {
        int updateCount, l = 1;
        do {
            updateCount = 0;
            for (int i = 0; i < pruneArray.length; i++)
                if (pruneArray[i] == l)
                    for (int j = 0; j < 6; j++) {
                        int p = i;
                        for(int k = 0; k < 3; k++) {
                            p = transformationArray[p][j];
                            if (pruneArray[p] == 0) {
                                pruneArray[p] = (byte) (l + 1);
                                updateCount++;
                            }
                        }
                    }
            l++;
        } while (updateCount > 0);
    }

    private void initializePruneCornerPerm(byte[] pruneArray, int[][] transformationArray) {
        int updateCount, l = 1;
        do {
            updateCount = 0;
            for (int i = 0; i < pruneArray.length; i++)
                if (pruneArray[i] == l)
                    for (int j = 0; j < 6; j++) {
                        int p = i;
                        for (int k = 0; k < 3; k++) {
                            p = transformationArray[p][j];
                            if ((j == 1 || j == 4 || k == 1) && pruneArray[p] == 0) {
                                pruneArray[p] = (byte)(l + 1);
                                updateCount++;
                            }
                        }
                    }
            l++;
        } while (updateCount != 0);
    }

    private void initializeTransformations() {
        for (int i = 0; i < 2048; i++)
            for (int m = 0; m < 6; m++)
                transformationEdgeOrient[i][m] = getTransformationEdgeOrient(i, m);
        for (int i = 0; i < 2187; i++)
            for (int m = 0; m < 6; m++)
                transformationCornerOrient[i][m] = getTransformationCornerOrient(i, m);
        for (int i = 0; i < 40320; i++)
            for (int m = 0; m < 6; m++)
                transformationCornerPerm[i][m] = getTransformationCornerPerm(i, m);
        for (int i = 0; i < 40320; i++)
            for (int m = 0; m < 6; m++)
                transformationEdgePerm[i][m] = getTransformationEdgePerm(i, m);
        for (int i = 0; i < 11880; i++)
            for (int m = 0; m < 6; m++)
                transformationSlice[i][m] = getTransformationSlice(i, m);
        for (int i = 0; i < 2048; i++)
            for (int m = 0; m < 6; m++)
                transformationChoice[i][m] = getTransformationChoice(i, m);
        for (int i = 0; i < 24; i++)
            for (int m = 0; m < 6; m++)
                transformationSlicePerm[i][m] = getTransformationSlicePerm(i, m);
    }

    private int getTransformationEdgeOrient(int pos, int m)
    {
        int[] edges = new int[12];
        generateNthCombination(edges, 12, 2, pos);

        if (m == 3)
        {
            cycle(edges, 0, 5, 8, 4);
            edges[0] ^= 1; edges[5] ^= 1; edges[8] ^= 1; edges[4] ^= 1;
        }
        else if (m == 2)
        {
            cycle(edges, 1, 6, 9, 5);
        }
        else if (m == 0)
        {
            cycle(edges, 2, 7, 10, 6);
            edges[2] ^= 1; edges[7] ^= 1; edges[10] ^= 1; edges[6] ^= 1;
        }
        else if (m == 5)
        {
            cycle(edges, 3, 4, 11, 7);
        }
        else if (m == 1)
        {
            cycle(edges, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            cycle(edges, 8, 9, 10, 11);
        }
        return getNFromCombination(edges, 12, 2);
    }

    private int getTransformationCornerOrient(int pos, int m)
    {
        int[] corners = new int[8];
        generateNthCombination(corners, 8, 3, pos);

        if (m == 3)
        {
            cycle(corners, 0, 1, 5, 4);
            corners[0] += 2; corners[1]++; corners[5] += 2; corners[4]++;
        }
        else if (m == 2)
        {
            cycle(corners, 1, 2, 6, 5);
            corners[1] += 2; corners[2]++; corners[6] += 2; corners[5]++;
        }
        else if (m == 0)
        {
            cycle(corners, 2, 3, 7, 6);
            corners[2] += 2; corners[3]++; corners[7] += 2; corners[6]++;
        }
        else if (m == 5)
        {
            cycle(corners, 3, 0, 4, 7);
            corners[3] += 2; corners[0]++; corners[4] += 2; corners[7]++;
        }
        else if (m == 1)
        {
            cycle(corners, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            cycle(corners, 4, 5, 6, 7);
        }
        return getNFromCombination(corners, 8, 3);
    }

    private int getTransformationCornerPerm(int pos, int m)
    {
        int[] corners = new int[8];
        generateNthPermutation(corners, 8, pos);
        if (m == 3)
        {
            cycle(corners, 0, 1, 5, 4);
        }
        else if (m == 2)
        {
            cycle(corners, 1, 2, 6, 5);
        }
        else if (m == 0)
        {
            cycle(corners, 2, 3, 7, 6);
        }
        else if (m == 5)
        {
            cycle(corners, 3, 0, 4, 7);
        }
        else if (m == 1)
        {
            cycle(corners, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            cycle(corners, 4, 5, 6, 7);
        }
        return getNFromPermutation(corners, 8);
    }

    private int getTransformationEdgePerm(int pos, int m)
    {
        int[] edges = new int[8];
        generateNthPermutation(edges, 8, pos);
        if (m == 3)
        {
            swap(edges, 0, 4);
        }
        else if (m == 2)
        {
            swap(edges, 1, 5);
        }
        else if (m == 0)
        {
            swap(edges, 2, 6);
        }
        else if (m == 5)
        {
            swap(edges, 3, 7);
        }
        else if (m == 1)
        {
            cycle(edges, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            cycle(edges, 4, 5, 6, 7);
        }
        return getNFromPermutation(edges, 8);
    }

    private int getTransformationSlice(int pos, int m)
    {
        int i, j, r;
        int[] edges = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        for (i = 0; i < 4; i++)
        {
            r = pos % (12 - i);
            pos = (pos - r) / (12 - i);
            for (j = 0; j < 12 && (edges[j] != 0 || r > 0); j++)
            {
                if (edges[j] == 0) r--;
            }
            edges[j] = i + 1;
        }

        if (m == 3)
        {
            cycle(edges, 0, 5, 8, 4);
        }
        else if (m == 2)
        {
            cycle(edges, 1, 6, 9, 5);
        }
        else if (m == 0)
        {
            cycle(edges, 2, 7, 10, 6);
        }
        else if (m == 5)
        {
            cycle(edges, 3, 4, 11, 7);
        }
        else if (m == 1)
        {
            cycle(edges, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            cycle(edges, 8, 9, 10, 11);
        }
        pos = 0;
        for (i = 3; i >= 0; i--)
        {
            r = 0;
            for (j = 0; j < 12 && edges[j] != i + 1; j++)
            {
                if (edges[j] == 0 || edges[j] > i + 1) r++;
            }
            pos = pos * (12 - i) + r;
        }
        return pos;
    }

    private int getTransformationSlicePerm(int pos, int m)
    {
        int[] edges = new int[4];
        generateNthPermutation(edges, 4, pos);
        if (m == 3)
        {
            swap(edges, 0, 1);
        }
        else if (m == 2)
        {
            swap(edges, 1, 2);
        }
        else if (m == 0)
        {
            swap(edges, 2, 3);
        }
        else if (m == 5)
        {
            swap(edges, 3, 0);
        }
        return getNFromPermutation(edges, 4);
    }

    private int getTransformationChoice(int pos, int m)
    {
        int[] edges = new int[12];
        generateNthCombination(edges, 12, 2, pos);

        if (m == 3)
        {
            cycle(edges, 0, 5, 8, 4);
        }
        else if (m == 2)
        {
            cycle(edges, 1, 6, 9, 5);
        }
        else if (m == 0)
        {
            cycle(edges, 2, 7, 10, 6);
        }
        else if (m == 5)
        {
            cycle(edges, 3, 4, 11, 7);
        }
        else if (m == 1)
        {
            cycle(edges, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            cycle(edges, 8, 9, 10, 11);
        }
        return (getNFromCombination(edges, 12, 2));
    }

    private void swap(int[] pr, int i, int j)
    {
        int temp = pr[i];
        pr[i] = pr[j];
        pr[j] = temp;
    }

    private void cycle(int[] pr, int i, int j, int k, int l)
    {
        int temp = pr[i];
        pr[i] = pr[j];
        pr[j] = pr[k];
        pr[k] = pr[l];
        pr[l] = temp;
    }

    private static void generateNthPermutation(int[] arr, int len, int n)
    {
        int[] numbers = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
        for (int i = 0; i < len; i++)
        {
            int value = n % (len - i);
            n = (n - value) / (len - i);
            arr[i] = numbers[value];
            while (++value < len)
                numbers[value - 1] = numbers[value];
        }
    }

    private void generateNthPartialPermutation(int[] arr, int permOffset, int n)
    {
        for (int i = 0; i < 4; i++)
        {
            int value = n % (arr.length - i);
            n = (n - value) / (arr.length - i);
            int j;
            for (j = 0; j < arr.length && ((arr[j] >= permOffset && arr[j] < permOffset + 4) || value > 0); j++)
            {
                if (arr[j] < permOffset || arr[j] >= permOffset + 4)
                    value--;
            }
            arr[j] = permOffset + i;
        }
    }

    private static void generateNthCombination(int[] arr, int len, int valueLimit, int n)
    {
        int j = 0;
        for (int i = 0; i < len - 1; i++)
        {
            int value = n % valueLimit;
            j += valueLimit - value;
            n = (n - value) / valueLimit;
            arr[i] = value;
        }
        arr[len - 1] = j % valueLimit;
    }

    private int getNFromCombination(int[] arr, int len, int valueLimit)
    {
        int n = 0;
        for (int i = len - 2; i >= 0; i--)
            n = n * valueLimit + (arr[i] % valueLimit);
        return n;
    }

    private int getNFromPermutation(int[] arr, int len) {
        return getNFromPermutation(arr, 0, len);
    }

    private int getNFromPermutation(int[] arr, int start, int len)
    {
        int n = 0;
        for (int i = len - 1; i >= 0; i--)
        {
            int value = 0;
            for (int j = i + 1; j < len; j++)
                if (arr[start + j] < arr[start + i])
                    value++;

            n = n * (len - i) + value;
        }
        return n;
    }
    private int getNFromPartialPermutation(int[] perm, int start, int len, int permCount, int permOffset)
    {
        int n = 0;
        for (int i = permCount - 1; i >= 0; i--)
        {
            int value = 0;
            for (int j = 0; j < len && perm[start + j] != permOffset + i; j++)
                if (perm[start + j] < permOffset || perm[start + j] > permOffset + i)
                    value++;
            n = n * (len - i) + value;
        }
        return n;
    }

    public void setCubeState(CubeState cubeState){
        if(!cubeState.isValid())
            throw new IllegalArgumentException("Cube is not valid");
        for (int[] position : positions)
            Arrays.fill(position, 0);

        positions[0][0] = calculateEdgeOrientation(cubeState);
        positions[0][1] = calculateCornerOrientation(cubeState);
        positions[0][2] = getNFromPermutation(cubeState.getCubeletPermutations(), 8);
        positions[0][3] = getNFromPartialPermutation(cubeState.getCubeletPermutations(), 8, 12, 4, 8);
        positions[0][4] = getNFromPartialPermutation(cubeState.getCubeletPermutations(), 8, 12, 4, 12);
        positions[0][5] = getNFromPartialPermutation(cubeState.getCubeletPermutations(), 8, 12, 4, 16);
        positions[0][6] = calculateMiddleSlice(cubeState);
        positions[0][9] = -1;
        
        resetConfiguration();
    }

    private void resetConfiguration() {
        solutionLength = phase1Len = phase2Len = 0;
        maxDepth = 25;
        Arrays.fill(solutionMoves, 0);
        Arrays.fill(solutionAmounts, 0);
        solutionMoves[0] = -1;
        solutionAmounts[0] = 3;
    }

    private int calculateEdgeOrientation(CubeState cubeState) {
        int edgeOrientation = 0;
        for (int i = 10; i >= 0; i--) {
            edgeOrientation = edgeOrientation * 2 + cubeState.getCubeletOrientations()[8 + i];
        }
        return edgeOrientation;
    }

    private int calculateCornerOrientation(CubeState cubeState) {
        int cornerOrientation = 0;
        for (int i = 6; i >= 0; i--) {
            cornerOrientation = cornerOrientation * 3 + cubeState.getCubeletOrientations()[i];
        }
        return cornerOrientation;
    }

    private int calculateMiddleSlice(CubeState cubeState) {
        int middleSlice = 0;
        for (int i = 10; i >= 0; i--) {
            middleSlice = middleSlice * 2 + ((cubeState.getCubeletPermutations()[8 + i] >= 12 && cubeState.getCubeletPermutations()[8 + i] < 16) ? 1 : 0);
        }
        return middleSlice;
    }

    public boolean solve() {

        if (phase1Len >= maxDepth) return (false);

        while (!search1())
        {
            phase1Len++;
            if (phase1Len >= maxDepth) return (false);
        }
        maxDepth = solutionLength - 1;
        return true;
    }

    private boolean search1()
    {
        if (solutionLength >= phase1Len) {
            if (positions[phase1Len][0] == 0
                    && positions[phase1Len][1] == 0
                    && positions[phase1Len][6] == 240)
                if (solve2())
                    return true;
            solutionLength = phase1Len;
        }

        while (solutionLength >= 0)
        {
            int next = solutionLength + 1;
            int move = solutionMoves[solutionLength];
            if (move >= 0)
            {
                positions[next][0] = transformationEdgeOrient[positions[next][0]][move];
                positions[next][1] = transformationCornerOrient[positions[next][1]][move];
                positions[next][2] = transformationCornerPerm[positions[next][2]][move];
                positions[next][3] = transformationSlice[positions[next][3]][move];
                positions[next][4] = transformationSlice[positions[next][4]][move];
                positions[next][5] = transformationSlice[positions[next][5]][move];
                positions[next][6] = transformationChoice[positions[next][6]][move];
            }
            else
            {
                positions[next][0] = positions[solutionLength][0];
                positions[next][1] = positions[solutionLength][1];
                positions[next][2] = positions[solutionLength][2];
                positions[next][3] = positions[solutionLength][3];
                positions[next][4] = positions[solutionLength][4];
                positions[next][5] = positions[solutionLength][5];
                positions[next][6] = positions[solutionLength][6];
                positions[next][9] = positions[solutionLength][9];
            }
            solutionAmounts[solutionLength]++;
            if (solutionAmounts[solutionLength] > 3) {
                advanceSolutionMove();
                continue;
            }

            if (solutionLength + pruneEdgeOrient[positions[next][0]] < phase1Len + 1
                && solutionLength + pruneCornerOrient[positions[next][1]] < phase1Len + 1
                && solutionLength + pruneChoice[positions[next][6]] < phase1Len + 1) {
                solutionMoves[next] = -1;
                solutionAmounts[next] = 3;
                solutionLength = next;
                if (solutionLength >= phase1Len)
                    if (solve2())
                        return true;
            }
        }
        solutionMoves[0] = -1;
        solutionAmounts[0] = 3;
        solutionLength = 0;
        return false;
    }

    private boolean solve2()
    {
        if (solutionLength > 0)
            if (solutionAmounts[solutionLength - 1] == 2 || solutionMoves[solutionLength - 1] == 1 || solutionMoves[solutionLength - 1] == 4)
                return false;

        if (phase1Len + pruneCornerPerm[positions[phase1Len][2]] > maxDepth + 1)
            return false;

        phase2Len = solutionLength - phase1Len;
        if (solutionLength > maxDepth)
        {
            phase2Len = maxDepth - phase1Len;
        }

        if (phase2Len == 0)
        {
            if (positions[phase1Len][2] == 0 &&
                positions[phase1Len][3] == 0 &&
                positions[phase1Len][4] == 5860 &&
                positions[phase1Len][5] == 11720 &&
                positions[phase1Len][9] <= 0)
                return true;

            int[] edges = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
            generateNthPartialPermutation(edges, 0, positions[phase1Len][3]);
            generateNthPartialPermutation(edges, 4, positions[phase1Len][4]);
            generateNthPartialPermutation(edges, 8, positions[phase1Len][5]);

            positions[phase1Len][8] = getNFromPermutation(edges, 4, 4);
            edges[4] = edges[8]; edges[5] = edges[9];
            edges[6] = edges[10]; edges[7] = edges[11];
            positions[phase1Len][7] = getNFromPermutation(edges, 8);

        }

        while (!search2())
        {
            phase2Len++;
            if (phase1Len + phase2Len > maxDepth)
                return false;
        }
        return true;
    }

    private boolean search2()
    {
        while (solutionLength >= phase1Len)
        {
            int next = solutionLength + 1;
            int move = solutionMoves[solutionLength];
            if (move == 1 || move == 4)
            {
                positions[next][2] = transformationCornerPerm[positions[next][2]][move];
                positions[next][7] = transformationEdgePerm[positions[next][7]][move];
            }
            else if (move >= 0)
            {
                positions[next][2] = transformationCornerPerm[transformationCornerPerm[positions[next][2]][move]][move];
                positions[next][7] = transformationEdgePerm[positions[next][7]][move];
                positions[next][8] = transformationSlicePerm[positions[next][8]][move];
            }
            else
            {
                positions[next][2] = positions[solutionLength][2];
                positions[next][7] = positions[solutionLength][7];
                positions[next][8] = positions[solutionLength][8];
                positions[next][9] = positions[solutionLength][9];
            }
            solutionAmounts[solutionLength] += (move == 1 || move == 4) ? 1 : 2;
            if (solutionAmounts[solutionLength] > 3)
            {
                advanceSolutionMove();
                continue;
            }

            if (solutionLength + pruneEdgePerm[positions[next][7]] < phase1Len + phase2Len + 1 &&
                solutionLength + pruneCornerPerm[positions[next][2]] < phase1Len + phase2Len + 1 &&
                solutionLength + pruneSlicePerm[positions[next][8]] < phase1Len + phase2Len + 1)
            {
                solutionMoves[next] = -1;
                solutionAmounts[next] = 3;
                solutionLength = next;
                if (solutionLength >= phase1Len + phase2Len)
                    return true;
            }
        }
        solutionMoves[phase1Len] = -1;
        solutionAmounts[phase1Len] = 3;
        solutionLength = phase1Len;
        return false;
    }

    private void advanceSolutionMove() {
        solutionAmounts[solutionLength] = 0;
        do
        {
            solutionMoves[solutionLength]++;
        } while (solutionLength != 0 && (solutionMoves[solutionLength] == solutionMoves[solutionLength - 1] || solutionMoves[solutionLength] == solutionMoves[solutionLength - 1] + 3));
        if (solutionMoves[solutionLength] >= 6)
        {
            solutionLength--;
        }
    }

    public MoveSequence getGenerator() {
        return new MoveSequence(solutionLength, solutionMoves, solutionAmounts);
    }
}
