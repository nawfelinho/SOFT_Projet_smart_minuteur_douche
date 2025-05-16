package com.minuteur.projet_smart_minuteur_douche_dl_nr.repository;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import org.springframework.data.jpa.repository.JpaRepository;

public interface UserRepository extends JpaRepository<User, Long> {
    // Ajoute ici des méthodes personnalisées si besoin plus tard
}
