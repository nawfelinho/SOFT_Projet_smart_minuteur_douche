package com.minuteur.projet_smart_minuteur_douche_dl_nr.model;

import jakarta.persistence.*;
import lombok.*;
import java.time.LocalDateTime;

@Entity
@Table(name = "douches")
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class Douche {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "id", columnDefinition = "INTEGER") // Ajout explicite
    private Long id;

    @ManyToOne
    @JoinColumn(name = "id_user")
    private User user;

    private LocalDateTime dateDebut;
    private LocalDateTime dateFin;
    private int duree;

    @Column(name = "temps_depasse")
    private int tempsDepasse;
}
