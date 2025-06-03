package com.minuteur.projet_smart_minuteur_douche_dl_nr.controller;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.DTO.DoucheDTO;
import java.time.LocalDateTime;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.Douche;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.service.DoucheService;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Optional;

@RestController
@RequestMapping("/douches")
@CrossOrigin
public class DoucheController {

    @Autowired
    private DoucheService doucheService;

    @Autowired
    private UserService userService;

    @PostMapping
    public Douche enregistrerDouche(@RequestBody DoucheDTO dto) {
        // 1. Cherche l'utilisateur par son nom (ou adapte selon ton champ)
        User user = userService.getUserByNom(dto.user)
                .orElseThrow(() -> new RuntimeException("Utilisateur non trouvé: " + dto.user));

        // 2. Calcule les dates (ici on fait simple : date début = maintenant - durée)
        LocalDateTime now = LocalDateTime.now();

        Douche douche = new Douche();
        douche.setUser(user);
        douche.setDateDebut(now.minusSeconds(dto.timeSeconds));
        douche.setDateFin(now);
        douche.setDuree(dto.timeSeconds);
        douche.setTempsDepasse(0); // à calculer si besoin

        // 3. Enregistre la douche
        return doucheService.enregistrerDouche(douche);
    }
    @GetMapping
    public List<Douche> getAllDouches() {
        return doucheService.getAllDouches();
    }

    @GetMapping("/{id}")
    public Optional<Douche> getDoucheById(@PathVariable Long id) {
        return doucheService.getDoucheById(id);
    }

    @GetMapping("/user/{userId}")
    public List<Douche> getDouchesByUser(@PathVariable Long userId) {
        Optional<User> user = userService.getUserById(userId);
        return user.map(doucheService::getDouchesByUser).orElse(List.of());
    }
}
