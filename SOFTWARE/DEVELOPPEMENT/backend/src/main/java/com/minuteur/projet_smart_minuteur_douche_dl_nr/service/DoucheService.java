package com.minuteur.projet_smart_minuteur_douche_dl_nr.service;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.Douche;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.repository.DoucheRepository;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class DoucheService {

    private final DoucheRepository doucheRepository;
    private final UserRepository userRepository;

    @Autowired
    public DoucheService(DoucheRepository doucheRepository, UserRepository userRepository) {
        this.doucheRepository = doucheRepository;
        this.userRepository = userRepository;
    }

    public Douche enregistrerDouche(Douche douche) {
        // Calcul du dépassement (si besoin)
        int duree = douche.getDuree();
        int depassement = Math.max(0, duree - 300);
        douche.setTempsDepasse(depassement);

        Douche saved = doucheRepository.save(douche);

        // Met à jour le temps total utilisateur
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

    public List<Douche> getAllDouches() {
        return doucheRepository.findAll();
    }

    public Optional<Douche> getDoucheById(Long id) {
        return doucheRepository.findById(id);
    }

    public List<Douche> getDouchesByUser(User user) {
        return doucheRepository.findByUser(user);
    }

    public Douche save(Douche douche) {
        return doucheRepository.save(douche);

    }
}
