package com.minuteur.projet_smart_minuteur_douche_dl_nr.controller;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.Douche;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.service.DoucheService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/douches")
@CrossOrigin(origins = "http://localhost:3000")
public class DoucheController {

    @Autowired
    private DoucheService doucheService;

    @GetMapping
    public List<Douche> getAllDouches() {
        return doucheService.getAll();
    }

    @PostMapping
    public Douche addDouche(@RequestBody Douche douche) {
        return doucheService.saveDouche(douche);
    }
}
