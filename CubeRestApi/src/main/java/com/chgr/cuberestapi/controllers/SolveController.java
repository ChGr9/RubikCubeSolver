package com.chgr.cuberestapi.controllers;

import com.chgr.cuberestapi.models.CubeState;
import com.chgr.cuberestapi.models.MoveSequence;
import com.chgr.cuberestapi.services.KociembaSolver;
import org.apache.commons.lang3.time.StopWatch;
import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.server.ResponseStatusException;

import java.util.ArrayList;
import java.util.List;

@RequestMapping("/solve")
@RestController
public class SolveController {

    @GetMapping("/{cube}")
    public String solve(@PathVariable String cube){
        if(cube.length() != 54)
            throw new IllegalArgumentException("Cube must have 54 stickers");

        CubeState cubeState = new CubeState(cube);

        StopWatch stopWatch = StopWatch.createStarted();
        String solution = solve(cubeState);
        if(solution.isBlank())
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "No solution found");
        stopWatch.stop();
        System.out.println("Solver took " + stopWatch.getTime() + " ms");
        return solution;
    }

    private String solve(CubeState cubeState) {
        KociembaSolver solver = new KociembaSolver();
        solver.setCubeState(cubeState);
        //return solver.print();
        if(solver.solve()){
            MoveSequence generator = solver.getGenerator();
            List<String> result = new ArrayList<>();
            for(int i= generator.getLength() - 1; i >= 0; i--){
                StringBuilder sb = new StringBuilder();
                int move = generator.getMove(i);
                int amount = 4 - generator.getAmount(i);
                if(move < 6)
                    sb.append("LUFRDB".charAt(move));
                else {
                    sb.append("LUF".charAt(move % 3));
                    sb.append("xxmcsa".charAt(move / 3));
                }
                if(amount > 1)
                    sb.append("2i".charAt(amount - 2));
                result.add(sb.toString());
            }
            return String.join(", ", result);
        }
        return "";
    }
}
