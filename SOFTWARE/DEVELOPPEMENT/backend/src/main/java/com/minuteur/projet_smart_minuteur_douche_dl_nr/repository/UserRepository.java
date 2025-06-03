package com.minuteur.projet_smart_minuteur_douche_dl_nr.repository;
import java.util.Optional;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import org.springframework.data.jpa.repository.JpaRepository;

public interface UserRepository extends JpaRepository<User, Long> {
    Optional<User> findByNom(String nom);
}
