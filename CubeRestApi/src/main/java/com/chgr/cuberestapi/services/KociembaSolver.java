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
    private final int[][] transformationFace = new int[4096][6];

    private final byte[] pruneEdgeOrient = new byte[2048];
    private final byte[] pruneCornerOrient = new byte[2187];
    private final byte[] pruneEdgePerm = new byte[40320];
    private final byte[] pruneCornerPerm = new byte[40320];
    private final byte[] pruneChoice = new byte[2048];
    private final byte[] pruneSlicePerm = new byte[24];
    private final byte[] pruneFace1 = new byte[4096];
    private final byte[] pruneFace2 = new byte[4096];

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

        for (int i = 0; i < 4096; i++)
        {
            pruneFace1[i] = 1;
            if ((i & 1) != 0) pruneFace1[i]++;
            if ((i & 16) != 0) pruneFace1[i]++;
            if ((i & 64) != 0) pruneFace1[i]++;
            if ((i & 1024) != 0) pruneFace1[i]++;
            pruneFace2[i] = 1;
            if ((i & 3) != 0) pruneFace2[i]++;
            if ((i & 12) != 0) pruneFace2[i]++;
            if ((i & 48) != 0) pruneFace2[i]++;
            if ((i & 192) != 0) pruneFace2[i]++;
            if ((i & 768) != 0) pruneFace2[i]++;
            if ((i & 3072) != 0) pruneFace2[i]++;
        }
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

        for(int i = 0; i < 4096; i++){
            transformationFace[i][0] = ((i + (3 << 10)) & (1 << 12) - 1);
            transformationFace[i][1] = ((i + (3 << 8)) & ((1 << 10) - 1)) + (i & ((1 << 12) - (1 << 10)));
            transformationFace[i][2] = ((i + (3 << 6)) & ((1 << 8) - 1)) + (i & ((1 << 12) - (1 << 8)));
            transformationFace[i][3] = ((i + (3 << 4)) & ((1 << 6) - 1)) + (i & ((1 << 12) - (1 << 6)));
            transformationFace[i][4] = ((i + (3 << 2)) & ((1 << 4) - 1)) + (i & ((1 << 12) - (1 << 4)));
            transformationFace[i][5] = ((i + (3)) & ((1 << 2) - 1)) + (i & ((1 << 12) - (1 << 2)));
        }
    }

    private int getTransformationEdgeOrient(int pos, int m)
    {
        int[] edges = new int[12];
        Num2Ori(edges, 0, 12, 2, pos);

        if (m == 3)
        {
            Cycle(edges, 0, 5, 8, 4);
            edges[0] ^= 1; edges[5] ^= 1; edges[8] ^= 1; edges[4] ^= 1;
        }
        else if (m == 2)
        {
            Cycle(edges, 1, 6, 9, 5);
        }
        else if (m == 0)
        {
            Cycle(edges, 2, 7, 10, 6);
            edges[2] ^= 1; edges[7] ^= 1; edges[10] ^= 1; edges[6] ^= 1;
        }
        else if (m == 5)
        {
            Cycle(edges, 3, 4, 11, 7);
        }
        else if (m == 1)
        {
            Cycle(edges, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            Cycle(edges, 8, 9, 10, 11);
        }
        return Ori2Num(edges, 0, 12, 2);
    }

    private int getTransformationCornerOrient(int pos, int m)
    {
        int[] corners = new int[8];
        Num2Ori(corners, 0, 8, 3, pos);

        if (m == 3)
        {
            Cycle(corners, 0, 1, 5, 4);
            corners[0] += 2; corners[1]++; corners[5] += 2; corners[4]++;
        }
        else if (m == 2)
        {
            Cycle(corners, 1, 2, 6, 5);
            corners[1] += 2; corners[2]++; corners[6] += 2; corners[5]++;
        }
        else if (m == 0)
        {
            Cycle(corners, 2, 3, 7, 6);
            corners[2] += 2; corners[3]++; corners[7] += 2; corners[6]++;
        }
        else if (m == 5)
        {
            Cycle(corners, 3, 0, 4, 7);
            corners[3] += 2; corners[0]++; corners[4] += 2; corners[7]++;
        }
        else if (m == 1)
        {
            Cycle(corners, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            Cycle(corners, 4, 5, 6, 7);
        }
        return Ori2Num(corners, 0, 8, 3);
    }

    private int getTransformationCornerPerm(int pos, int m)
    {
        int[] corners = new int[8];
        Num2Perm(corners, 0, 8, pos);
        if (m == 3)
        {
            Cycle(corners, 0, 1, 5, 4);
        }
        else if (m == 2)
        {
            Cycle(corners, 1, 2, 6, 5);
        }
        else if (m == 0)
        {
            Cycle(corners, 2, 3, 7, 6);
        }
        else if (m == 5)
        {
            Cycle(corners, 3, 0, 4, 7);
        }
        else if (m == 1)
        {
            Cycle(corners, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            Cycle(corners, 4, 5, 6, 7);
        }
        return Perm2Num(corners, 0, 8);
    }

    private int getTransformationEdgePerm(int pos, int m)
    {
        int[] edges = new int[8];
        Num2Perm(edges, 0, 8, pos);
        if (m == 3)
        {
            Swap(edges, 0, 4);
        }
        else if (m == 2)
        {
            Swap(edges, 1, 5);
        }
        else if (m == 0)
        {
            Swap(edges, 2, 6);
        }
        else if (m == 5)
        {
            Swap(edges, 3, 7);
        }
        else if (m == 1)
        {
            Cycle(edges, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            Cycle(edges, 4, 5, 6, 7);
        }
        return Perm2Num(edges, 0, 8);
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
            Cycle(edges, 0, 5, 8, 4);
        }
        else if (m == 2)
        {
            Cycle(edges, 1, 6, 9, 5);
        }
        else if (m == 0)
        {
            Cycle(edges, 2, 7, 10, 6);
        }
        else if (m == 5)
        {
            Cycle(edges, 3, 4, 11, 7);
        }
        else if (m == 1)
        {
            Cycle(edges, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            Cycle(edges, 8, 9, 10, 11);
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
        Num2Perm(edges, 0, 4, pos);
        if (m == 3)
        {
            Swap(edges, 0, 1);
        }
        else if (m == 2)
        {
            Swap(edges, 1, 2);
        }
        else if (m == 0)
        {
            Swap(edges, 2, 3);
        }
        else if (m == 5)
        {
            Swap(edges, 3, 0);
        }
        return Perm2Num(edges, 0, 4);
    }

    private int getTransformationChoice(int pos, int m)
    {
        int[] edges = new int[12];
        Num2Ori(edges, 0, 12, 2, pos);

        if (m == 3)
        {
            Cycle(edges, 0, 5, 8, 4);
        }
        else if (m == 2)
        {
            Cycle(edges, 1, 6, 9, 5);
        }
        else if (m == 0)
        {
            Cycle(edges, 2, 7, 10, 6);
        }
        else if (m == 5)
        {
            Cycle(edges, 3, 4, 11, 7);
        }
        else if (m == 1)
        {
            Cycle(edges, 3, 2, 1, 0);
        }
        else if (m == 4)
        {
            Cycle(edges, 8, 9, 10, 11);
        }
        return (Ori2Num(edges, 0, 12, 2));
    }

    private void Swap(int[] pr, int i, int j)
    {
        int c = pr[i];
        pr[i] = pr[j];
        pr[j] = c;
    }

    private void Cycle(int[] pr, int i, int j, int k, int l)
    {
        int c = pr[i];
        pr[i] = pr[j];
        pr[j] = pr[k];
        pr[k] = pr[l];
        pr[l] = c;
    }

    private static boolean ParityOdd(int[] pieces, int start, int len)
    {
        int i, j;
        boolean p = false;
        for (i = 0; i < len; i++)
        {
            for (j = 0; j < i; j++)
            {
                p ^= pieces[start + i] < pieces[start + j];
            }
        }
        return p;
    }

    private static void Num2Perm(int[] pieces, int start, int len, int pos)
    {
        int i, r;
        int[] w = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
        for (i = 0; i < len; i++)
        {
            r = pos % (len - i);
            pos = (pos - r) / (len - i);
            pieces[start + i] = w[r];
            while (++r < len) w[r - 1] = w[r];
        }
    }

    private void Num2PartPerm(int[] pieces, int start, int len, int np, int p0, int pos)
    {
        int i, j, r;
        for (i = 0; i < np; i++)
        {
            r = pos % (len - i);
            pos = (pos - r) / (len - i);
            for (j = start; j < start + len && ((pieces[j] >= p0 && pieces[j] < p0 + np) || r > 0); j++)
            {
                if (pieces[j] < p0 || pieces[j] >= p0 + np) r--;
            }
            pieces[j] = p0 + i;
        }
    }

    private static void Num2Ori(int[] pieces, int start, int len, int val, int pos)
    {
        int i, j = 0, k;
        for (i = 0; i < len - 1; i++)
        {
            k = pos % val;
            j += val - k;
            pos = (pos - k) / val;
            pieces[start + i] = k;
        }
        pieces[start + len - 1] = j % val;
    }

    private int Ori2Num(int[] pieces, int start, int len, int val)
    {
        int i, j = 0;
        for (i = len - 2; i >= 0; i--) j = j * val + (pieces[start + i] % val);
        return (j);
    }

    private int Perm2Num(int[] pieces, int start, int len)
    {
        int i, j, r;
        int p = 0;
        for (i = len - 1; i >= 0; i--)
        {
            r = 0;
            for (j = i + 1; j < len; j++)
            {
                if (pieces[start + j] < pieces[start + i]) { r++; }
            }
            p = p * (len - i) + r;
        }
        return p;
    }
    private int PartPerm2Num(int[] perm, int start, int len, int permCount, int permOffset)
    {
        int i, j, r, pos = 0;
        for (i = permCount - 1; i >= 0; i--)
        {
            r = 0;
            for (j = 0; j < len && perm[start + j] != permOffset + i; j++)
            {
                if (perm[start + j] < permOffset || perm[start + j] > permOffset + i) r++;
            }
            pos = pos * (len - i) + r;
        }
        return (pos);
    }

    public void setCubeState(CubeState cubeState){
        if(!cubeState.isValid())
            throw new IllegalArgumentException("Cube is not valid");
        for (int[] position : positions)
            Arrays.fill(position, 0);

        positions[0][0] = calculateEdgeOrientation(cubeState);
        positions[0][1] = calculateCornerOrientation(cubeState);
        positions[0][2] = Perm2Num(cubeState.getCubeletPermutations(), 0, 8);
        positions[0][3] = PartPerm2Num(cubeState.getCubeletPermutations(), 8, 12, 4, 8);
        positions[0][4] = PartPerm2Num(cubeState.getCubeletPermutations(), 8, 12, 4, 12);
        positions[0][5] = PartPerm2Num(cubeState.getCubeletPermutations(), 8, 12, 4, 16);
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
            int nxt = solutionLength + 1;
            int m = solutionMoves[solutionLength];
            if (m >= 0)
            {
                positions[nxt][0] = transformationEdgeOrient[positions[nxt][0]][m];
                positions[nxt][1] = transformationCornerOrient[positions[nxt][1]][m];
                positions[nxt][2] = transformationCornerPerm[positions[nxt][2]][m];
                positions[nxt][3] = transformationSlice[positions[nxt][3]][m];
                positions[nxt][4] = transformationSlice[positions[nxt][4]][m];
                positions[nxt][5] = transformationSlice[positions[nxt][5]][m];
                positions[nxt][6] = transformationChoice[positions[nxt][6]][m];
            }
            else
            {
                positions[nxt][0] = positions[solutionLength][0];
                positions[nxt][1] = positions[solutionLength][1];
                positions[nxt][2] = positions[solutionLength][2];
                positions[nxt][3] = positions[solutionLength][3];
                positions[nxt][4] = positions[solutionLength][4];
                positions[nxt][5] = positions[solutionLength][5];
                positions[nxt][6] = positions[solutionLength][6];
                positions[nxt][9] = positions[solutionLength][9];
            }
            solutionAmounts[solutionLength]++;
            if (solutionAmounts[solutionLength] > 3) {
                solutionAmounts[solutionLength] = 0;
                do {
                    solutionMoves[solutionLength]++;
                } while (solutionLength != 0 && (solutionMoves[solutionLength] == solutionMoves[solutionLength - 1] || solutionMoves[solutionLength] == solutionMoves[solutionLength - 1] + 3));
                if (solutionMoves[solutionLength] >= 6) {
                    solutionLength--;
                    continue;
                }
                continue;
            }

            if (solutionLength + pruneEdgeOrient[positions[nxt][0]] < phase1Len + 1
                && solutionLength + pruneCornerOrient[positions[nxt][1]] < phase1Len + 1
                && solutionLength + pruneChoice[positions[nxt][6]] < phase1Len + 1) {
                solutionMoves[nxt] = -1;
                solutionAmounts[nxt] = 3;
                solutionLength = nxt;
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
            Num2PartPerm(edges, 0, 12, 4, 0, positions[phase1Len][3]);
            Num2PartPerm(edges, 0, 12, 4, 4, positions[phase1Len][4]);
            Num2PartPerm(edges, 0, 12, 4, 8, positions[phase1Len][5]);

            positions[phase1Len][8] = Perm2Num(edges, 4, 4);
            edges[4] = edges[8]; edges[5] = edges[9];
            edges[6] = edges[10]; edges[7] = edges[11];
            positions[phase1Len][7] = Perm2Num(edges, 0, 8);

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
            int nxt = solutionLength + 1;
            int m = solutionMoves[solutionLength];
            if (m == 1 || m == 4)
            {
                positions[nxt][2] = transformationCornerPerm[positions[nxt][2]][m];
                positions[nxt][7] = transformationEdgePerm[positions[nxt][7]][m];
            }
            else if (m >= 0)
            {
                positions[nxt][2] = transformationCornerPerm[transformationCornerPerm[positions[nxt][2]][m]][m];
                positions[nxt][7] = transformationEdgePerm[positions[nxt][7]][m];
                positions[nxt][8] = transformationSlicePerm[positions[nxt][8]][m];
            }
            else
            {
                positions[nxt][2] = positions[solutionLength][2];
                positions[nxt][7] = positions[solutionLength][7];
                positions[nxt][8] = positions[solutionLength][8];
                positions[nxt][9] = positions[solutionLength][9];
            }
            solutionAmounts[solutionLength] += (m == 1 || m == 4) ? 1 : 2;
            if (solutionAmounts[solutionLength] > 3)
            {
                solutionAmounts[solutionLength] = 0;
                do
                {
                    solutionMoves[solutionLength]++;
                } while (solutionLength != 0 && (solutionMoves[solutionLength] == solutionMoves[solutionLength - 1] || solutionMoves[solutionLength] == solutionMoves[solutionLength - 1] + 3));
                if (solutionMoves[solutionLength] >= 6)
                {
                    solutionLength--;
                    continue;
                }
                continue;
            }

            if (solutionLength + pruneEdgePerm[positions[nxt][7]] < phase1Len + phase2Len + 1 &&
                solutionLength + pruneCornerPerm[positions[nxt][2]] < phase1Len + phase2Len + 1 &&
                solutionLength + pruneSlicePerm[positions[nxt][8]] < phase1Len + phase2Len + 1)
            {
                solutionMoves[nxt] = -1;
                solutionAmounts[nxt] = 3;
                solutionLength = nxt;
                if (solutionLength >= phase1Len + phase2Len)
                    return true;
            }
        }
        solutionMoves[phase1Len] = -1;
        solutionAmounts[phase1Len] = 3;
        solutionLength = phase1Len;
        return false;
    }

    public MoveSequence getGenerator() {
        return new MoveSequence(solutionLength, solutionMoves, solutionAmounts);
    }

    public String print() {
        StringBuilder sb = new StringBuilder();
        sb.append("Edge orientation: ").append(array2d(transformationEdgeOrient)).append("\r\n");
        sb.append("Corner orientation: ").append(array2d(transformationCornerOrient)).append("\r\n");
        sb.append("Edge permutation: ").append(array2d(transformationEdgePerm)).append("\r\n");
        sb.append("Corner permutation: ").append(array2d(transformationCornerPerm)).append("\r\n");
        sb.append("Slice: ").append(array2d(transformationSlice)).append("\r\n");
        sb.append("Choice: ").append(array2d(transformationChoice)).append("\r\n");
        sb.append("Slice permutation: ").append(array2d(transformationSlicePerm)).append("\r\n");
        sb.append("Face: ").append(array2d(transformationFace)).append("\r\n");

        sb.append("Edge orientation prune: ").append(array1d(pruneEdgeOrient)).append("\r\n");
        sb.append("Corner orientation prune: ").append(array1d(pruneCornerOrient)).append("\r\n");
        sb.append("Edge permutation prune: ").append(array1d(pruneEdgePerm)).append("\r\n");
        sb.append("Corner permutation prune: ").append(array1d(pruneCornerPerm)).append("\r\n");
        sb.append("Choice prune: ").append(array1d(pruneChoice)).append("\r\n");
        sb.append("Slice permutation prune: ").append(array1d(pruneSlicePerm)).append("\r\n");
        sb.append("Face 1 prune: ").append(array1d(pruneFace1)).append("\r\n");
        sb.append("Face 2 prune: ").append(array1d(pruneFace2)).append("\r\n");
        return sb.toString();
    }

    private String array2d(int[][] arr) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < arr.length; i++) {
            for (int elem : arr[i]) {
                sb.append(elem).append(",");
            }
            if(i < arr.length - 1)
                sb.append(" | ");
        }
        return sb.toString();
    }

    private String array1d(byte[] arr) {
        StringBuilder sb = new StringBuilder();
        for (byte elem : arr) {
            sb.append(elem).append(",");
        }
        return sb.toString();
    }
}
