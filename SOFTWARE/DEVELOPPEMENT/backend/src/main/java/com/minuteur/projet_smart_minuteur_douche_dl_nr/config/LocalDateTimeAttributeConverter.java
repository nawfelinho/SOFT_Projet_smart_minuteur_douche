package com.minuteur.projet_smart_minuteur_douche_dl_nr.config;

import jakarta.persistence.AttributeConverter;
import jakarta.persistence.Converter;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.time.format.DateTimeParseException;

@Converter(autoApply = true)
public class LocalDateTimeAttributeConverter implements AttributeConverter<LocalDateTime, String> {

    private static final DateTimeFormatter FORMATTER_ISO = DateTimeFormatter.ISO_LOCAL_DATE_TIME;
    private static final DateTimeFormatter FORMATTER_SQL = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");

    @Override
    public String convertToDatabaseColumn(LocalDateTime attribute) {
        // Sauvegarde toujours en ISO
        return attribute != null ? attribute.format(FORMATTER_ISO) : null;
    }

    @Override
    public LocalDateTime convertToEntityAttribute(String dbData) {
        if (dbData == null) return null;
        try {
            // Essaye ISO d'abord
            return LocalDateTime.parse(dbData, FORMATTER_ISO);
        } catch (DateTimeParseException e) {
            try {
                // Puis tente le format SQL (avec espace)
                return LocalDateTime.parse(dbData, FORMATTER_SQL);
            } catch (DateTimeParseException ex) {
                throw new RuntimeException("DateTime non reconnue: " + dbData);
            }
        }
    }
}