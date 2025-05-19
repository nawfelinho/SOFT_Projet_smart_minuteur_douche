package com.minuteur.projet_smart_minuteur_douche_dl_nr.model;

import jakarta.persistence.*;
import lombok.*;

@Entity
@Table(name = "users")
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class User {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "id", columnDefinition = "INTEGER") // Ajout explicite
    private Long id;

    private String nom;
    private String prenom;

    @Column(name = "temps_total")
    private int tempsTotal;
}
