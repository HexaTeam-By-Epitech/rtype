/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** AccessibilitySettings - Struct for storing accessibility settings with binary save/load
*/

#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace Settings {

    /**
     * @brief Magic number to identify valid settings files
     */
    constexpr uint32_t SETTINGS_MAGIC = 0x52545950;  // "RTYP"

    /**
     * @brief Current settings version for forward compatibility
     */
    constexpr uint32_t SETTINGS_VERSION = 1;

    /**
     * @brief Default settings file path
     */
    constexpr const char *SETTINGS_FILE_PATH = ".rtype_settings";

    /**
     * @brief Colorblind filter types (matches AccessibilityMenu::ColorblindFilter)
     */
    enum class ColorblindFilterType : uint8_t {
        NONE = 0,
        PROTANOPIA = 1,
        DEUTERANOPIA = 2,
        TRITANOPIA = 3,
        MONOCHROMACY = 4
    };

/**
     * @brief Binary struct for storing accessibility settings
     * 
     * This struct is designed for direct binary read/write (C-style).
     * All fields use fixed-size types for cross-platform compatibility.
     */
#pragma pack(push, 1)
    struct AccessibilitySettings {
        // Header for validation
        uint32_t magic;    ///< Magic number for file validation
        uint32_t version;  ///< Settings version for compatibility

        // Accessibility settings
        ColorblindFilterType colorblindFilter;  ///< Colorblind filter type
        uint8_t visualSoundIndicators;          ///< Visual sound indicators (0 = off, 1 = on)
        uint8_t padding[2];                     ///< Padding for alignment

        /**
         * @brief Initialize with default values
         */
        void SetDefaults() {
            magic = SETTINGS_MAGIC;
            version = SETTINGS_VERSION;
            colorblindFilter = ColorblindFilterType::NONE;
            visualSoundIndicators = 0;
            padding[0] = 0;
            padding[1] = 0;
        }

        /**
         * @brief Check if the settings struct has valid header
         */
        [[nodiscard]] bool IsValid() const { return magic == SETTINGS_MAGIC && version <= SETTINGS_VERSION; }
    };
#pragma pack(pop)

    /**
     * @brief Save accessibility settings to file (C-style binary write)
     * @param settings The settings struct to save
     * @param filePath Path to the settings file (default: .rtype_settings)
     * @return true if save was successful
     */
    inline bool SaveSettings(const AccessibilitySettings &settings,
                             const char *filePath = SETTINGS_FILE_PATH) {
        FILE *file = fopen(filePath, "wb");
        if (!file) {
            return false;
        }

        size_t written = fwrite(&settings, sizeof(AccessibilitySettings), 1, file);
        fclose(file);

        return written == 1;
    }

    /**
     * @brief Load accessibility settings from file (C-style binary read)
     * @param settings The settings struct to load into
     * @param filePath Path to the settings file (default: .rtype_settings)
     * @return true if load was successful and settings are valid
     */
    inline bool LoadSettings(AccessibilitySettings &settings, const char *filePath = SETTINGS_FILE_PATH) {
        FILE *file = fopen(filePath, "rb");
        if (!file) {
            // File doesn't exist - use defaults
            settings.SetDefaults();
            return false;
        }

        size_t read = fread(&settings, sizeof(AccessibilitySettings), 1, file);
        fclose(file);

        if (read != 1 || !settings.IsValid()) {
            // Invalid file - use defaults
            settings.SetDefaults();
            return false;
        }

        return true;
    }

    /**
     * @brief Check if settings file exists
     * @param filePath Path to the settings file (default: .rtype_settings)
     * @return true if file exists
     */
    inline bool SettingsFileExists(const char *filePath = SETTINGS_FILE_PATH) {
        FILE *file = fopen(filePath, "rb");
        if (file) {
            fclose(file);
            return true;
        }
        return false;
    }

}  // namespace Settings
