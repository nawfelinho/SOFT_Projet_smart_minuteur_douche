package com.minuteur.projet_smart_minuteur_douche_dl_nr.controller;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.DTO.DoucheDTO;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.Douche;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.service.DoucheService;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
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
    public ResponseEntity<Douche> enregistrerDouche(@RequestBody DoucheDTO dto) {
        Long userId = dto.userId;
        if (userId == null) {
            return ResponseEntity.badRequest().build();
        }

        Optional<User> userOpt = userService.getUserById(userId);
        if (userOpt.isEmpty()) {
            return ResponseEntity.badRequest().body(null);
        }

        User user = userOpt.get();

        Douche douche = Douche.builder()
                .user(user)
                .dateDebut(LocalDateTime.now())
                .dateFin(LocalDateTime.now().plusSeconds(dto.timeSeconds))
                .duree(dto.timeSeconds)
                .tempsDepasse(0)
                .build();

        return ResponseEntity.ok(doucheService.enregistrerDouche(douche));
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