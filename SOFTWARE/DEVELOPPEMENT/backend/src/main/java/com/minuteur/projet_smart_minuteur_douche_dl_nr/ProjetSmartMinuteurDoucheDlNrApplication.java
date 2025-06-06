package com.minuteur.projet_smart_minuteur_douche_dl_nr;

import java.io.*;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class ProjetSmartMinuteurDoucheDlNrApplication {

    public static void main(String[] args) {
        SpringApplication.run(ProjetSmartMinuteurDoucheDlNrApplication.class, args);

        // === Lancer l'app Python ===
        try {
            String cheminApp = "/Users/damienlorigeon/Documents/Polytech/4A/Projet Smart/SOFT_Projet_smart_minuteur_douche/HARDWARE/DEVELOPPEMENT/Programme TEST python BLE/TEST_Projet_SMART_BLE/dist/PontPython_ProjetSMART.app/Contents/MacOS/PontPython_ProjetSMART";

            ProcessBuilder pb = new ProcessBuilder(cheminApp);
            pb.redirectErrorStream(true);
            Process process = pb.start();

            // Lire les logs du script Python
            new Thread(() -> {
                try (BufferedReader reader = new BufferedReader(
                        new InputStreamReader(process.getInputStream()))) {
                    String line;
                    while ((line = reader.readLine()) != null) {
                        System.out.println("[PYTHON] " + line);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }).start();

            System.out.println("✅ Script Python lancé automatiquement");
        } catch (IOException e) {
            System.err.println("❌ Impossible de lancer l'app Python : " + e.getMessage());
        }
    }

}
