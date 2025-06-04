package com.minuteur.projet_smart_minuteur_douche_dl_nr.controller;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.DTO.UserToBleDTO;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.*;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.client.RestTemplate;

import java.util.HashMap;
import java.util.Map;

@RestController
@RequestMapping("/ble")
@CrossOrigin
public class BleController {

    @Autowired
    private UserService userService;

    @PostMapping("/sendUserToEsp32")
    public ResponseEntity<?> sendUserToEsp32(@RequestBody UserToBleDTO dto) {
        // Vérification que l'utilisateur existe
        User user = userService.getUserByNom(dto.getUsername())
                .orElseThrow(() -> new RuntimeException("Utilisateur non trouvé: " + dto.getUsername()));

        // Appel du pont Python pour envoyer le nom à l’ESP32
        RestTemplate restTemplate = new RestTemplate();
        String url = "http://localhost:5001/sendToEsp32";
        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_JSON);
        Map<String, String> body = new HashMap<>();
        body.put("message", user.getNom());
        HttpEntity<Map<String, String>> req = new HttpEntity<>(body, headers);

        try {
            restTemplate.postForEntity(url, req, String.class);
            return ResponseEntity.ok(Map.of("status", "envoyé à l’ESP32", "user", user.getNom()));
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR)
                    .body(Map.of("error", "Erreur lors de l’envoi vers l’ESP32", "details", e.getMessage()));
        }
    }
}