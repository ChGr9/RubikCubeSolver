package com.chgr.cuberestapi.models;

import lombok.Getter;

public class MoveSequence {
    @Getter
    private int length;
    private final int[] moves;
    private final int[] amount;

    public MoveSequence(int len, int[] moves, int[] amount){
        this.length = len;
        this.moves = new int[len];
        this.amount = new int[len];
        for(int i = 0; i < len; i++){
            this.moves[i] = moves[len - 1 - i];
            this.amount[i] = 4 - amount[len - 1 - i];
        }
        simplify();
    }

    public int getMove(int index){
        return moves[index];
    }
    public int getAmount(int index){
        return amount[index];
    }

    private void simplify(){
        int[] axis = new int[length];
        int[] type = new int[length];
        int[] turns = new int[3];
        for (int i = 0; i < length; i++)
        {
            axis[i] = moves[i] % 3;
            type[i] = (moves[i] - axis[i]) / 3;
        }

        int j, index = 0;
        while (index < length)
        {
            turns[0] = turns[1] = turns[2] = 0;
            for (j = index; j < length && axis[index] == axis[j]; j++)
            {
                switch (type[j])
                {
                    case 0:
                        turns[0] += amount[j];
                        break;
                    case 1:
                        turns[2] += amount[j];
                        break;
                    case 2:
                        turns[1] += amount[j];
                        break;
                    case 3:
                        turns[0] += amount[j];
                        turns[1] += amount[j];
                        turns[2] -= amount[j];
                        break;
                    case 4:
                        turns[0] += amount[j];
                        turns[2] -= amount[j];
                        break;
                    case 5:
                        turns[0] += amount[j];
                        turns[2] += amount[j];
                        break;
                }
            }

            if (j <= index + 1)
            {
                index++;
                continue;
            }

            turns[0] &= 3;
            turns[1] &= 3;
            turns[2] &= 3;
            index = processTurns(index, j, turns, axis, type);
        }
    }

    private int processTurns(int i, int j, int[] turns, int[] axis, int[] type) {
        if (turns[0] != 0 || turns[1] != 0 || turns[2] != 0) {
            if (turns[0] == turns[1] && turns[0] + turns[2] == 4)
            {
                amount[i] = turns[0];
                type[i] = 3;
                moves[i] = 9 + axis[i];
                i++;
            }
            else if (turns[1] == 0 && turns[2] == 0)
            {
                amount[i] = turns[0];
                type[i] = 0;
                moves[i] = axis[i];
                i++;
            }
            else if (turns[0] == 0 && turns[1] == 0)
            {
                amount[i] = turns[2];
                type[i] = 1;
                moves[i] = 3 + axis[i];
                i++;
            }
            else if (turns[0] == 0 && turns[2] == 0)
            {
                amount[i] = turns[1];
                type[i] = 2;
                moves[i] = 6 + axis[i];
                i++;
            }
            else if (turns[1] == 0 && turns[0] + turns[2] == 4)
            {
                amount[i] = turns[0];
                type[i] = 4;
                moves[i] = 12 + axis[i];
                i++;
            }
            else if (turns[1] == 0 && turns[0] == turns[2])
            {
                amount[i] = turns[0];
                type[i] = 5;
                moves[i] = 15 + axis[i];
                i++;
            }
            else if (turns[0] == turns[1])
            {
                amount[i] = turns[0];
                type[i] = 3;
                moves[i] = 9 + axis[i];
                i++;
                amount[i] = (turns[2] + turns[0]) & 3;
                type[i] = 1;
                moves[i] = 3 + axis[i];
                i++;
            }
            else if (turns[2] + turns[1] == 4)
            {
                amount[i] = turns[1];
                type[i] = 3;
                moves[i] = 9 + axis[i];
                i++;
                amount[i] = (turns[0] + turns[2]) & 3;
                type[i] = 0;
                moves[i] = axis[i];
                i++;
            }
            else if (turns[2] + turns[0] == 4)
            {
                amount[i] = turns[1];
                type[i] = 3;
                moves[i] = 9 + axis[i];
                i++;
                amount[i] = (turns[0] - turns[1]) & 3;
                type[i] = 4;
                moves[i] = 12 + axis[i];
                i++;
            }
            else if (((turns[0] - 2 * turns[1] - turns[2]) & 3) == 0)
            {
                amount[i] = turns[1];
                type[i] = 3;
                moves[i] = 9 + axis[i];
                i++;
                amount[i] = (turns[0] - turns[1]) & 3;
                type[i] = 5;
                moves[i] = 15 + axis[i];
                i++;
            }
            else if (turns[0] == 0 || turns[1] == 0 || turns[2] == 0)
            {
                if (turns[0] != 0)
                {
                    amount[i] = turns[0];
                    type[i] = 0;
                    moves[i] = axis[i];
                    i++;
                }
                if (turns[1] != 0)
                {
                    amount[i] = turns[1];
                    type[i] = 2;
                    moves[i] = 6 + axis[i];
                    i++;
                }
                if (turns[2] != 0)
                {
                    amount[i] = turns[2];
                    type[i] = 1;
                    moves[i] = 3 + axis[i];
                    i++;
                }
            }
            else
            {
                amount[i] = (turns[0] - turns[1]) & 3;
                type[i] = 0;
                moves[i] = axis[i];
                i++;
                amount[i] = turns[1];
                type[i] = 3;
                moves[i] = 9 + axis[i];
                i++;
                amount[i] = (turns[1] + turns[2]) & 3;
                type[i] = 1;
                moves[i] = 3 + axis[i];
                i++;
            }
        }

        length -= j - i;
        for (int k = i; k < length; k++)
        {
            moves[k] = moves[k + j - i];
            amount[k] = amount[k + j - i];
            axis[k] = axis[k + j - i];
            type[k] = type[k + j - i];
        }
        return i;
    }
}
