package com.minuteur.projet_smart_minuteur_douche_dl_nr.service;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.Douche;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.repository.DoucheRepository;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class DoucheService {

    @Autowired
    private DoucheRepository doucheRepository;

    @Autowired
    private UserRepository userRepository;

    public Douche saveDouche(Douche douche) {
        int duree = douche.getDuree();
        int depassement = Math.max(0, duree - 300);
        douche.setTempsDepasse(depassement);

        Douche saved = doucheRepository.save(douche);

        if (saved.getUser() != null) {
            Long userId = saved.getUser().getId();
            int total = doucheRepository.findByUserId(userId)
                    .stream()
                    .mapToInt(Douche::getDuree)
                    .sum();

            User user = saved.getUser();
            user.setTempsTotal(total);
            userRepository.save(user);
        }

        return saved;
    }

    public List<Douche> getAll() {
        return doucheRepository.findAll();
    }
}
