package com.minuteur.projet_smart_minuteur_douche_dl_nr.service;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class UserService {

    private final UserRepository userRepository;

    @Autowired
    public UserService(UserRepository userRepository) {
        this.userRepository = userRepository;
    }

    public User createUser(User user) {
        return userRepository.save(user);
    }

    public List<User> getAllUsers() {
        return userRepository.findAll();
    }

    public Optional<User> getUserById(Long id) {
        return userRepository.findById(id);
    }

    public void ajouterTemps(Long id, int temps) {
        userRepository.findById(id).ifPresent(user -> {
            user.setTempsTotal(user.getTempsTotal() + temps);
            userRepository.save(user);
        });
    }

    // Méthode utilisée pour retrouver un utilisateur par son nom (nom = champ dans User.java)
    public Optional<User> getUserByNom(String nom) {
        return userRepository.findByNom(nom);
    }

    public Optional<User> getUserByName(String name) {

        return userRepository.findByNom(name);

    }
}