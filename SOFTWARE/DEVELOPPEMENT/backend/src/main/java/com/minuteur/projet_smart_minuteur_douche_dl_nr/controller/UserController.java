package com.minuteur.projet_smart_minuteur_douche_dl_nr.controller;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Optional;

@RestController
@RequestMapping("/users")
@CrossOrigin
public class UserController {

    @Autowired
    private UserService userService;

    @PostMapping
    public User createUser(@RequestBody User user) {
        return userService.createUser(user);
    }

    @GetMapping
    public List<User> getAllUsers() {
        return userService.getAllUsers();
    }

    @GetMapping("/{id}")
    public Optional<User> getUserById(@PathVariable Long id) {
        return userService.getUserById(id);
    }

    @PatchMapping("/{id}/add-time")
    public void addTempsToUser(@PathVariable Long id, @RequestParam int duree) {
        userService.ajouterTemps(id, duree);
    }
}
