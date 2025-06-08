package com.minuteur.projet_smart_minuteur_douche_dl_nr.repository;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.Douche;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface DoucheRepository extends JpaRepository<Douche, Long> {
    List<Douche> findByUser(User user); // Toutes les douches d’un utilisateur

    List<Douche> findByUserId(Long userId);
}
