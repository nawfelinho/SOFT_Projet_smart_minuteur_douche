package com.minuteur.projet_smart_minuteur_douche_dl_nr.service;

import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.Douche;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.model.User;
import com.minuteur.projet_smart_minuteur_douche_dl_nr.repository.DoucheRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class DoucheService {

    private final DoucheRepository doucheRepository;

    @Autowired
    public DoucheService(DoucheRepository doucheRepository) {
        this.doucheRepository = doucheRepository;
    }

    public Douche enregistrerDouche(Douche douche) {
        return doucheRepository.save(douche);
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
}
