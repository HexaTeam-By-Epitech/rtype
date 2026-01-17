/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Argon2PasswordHasher - Argon2id password hashing implementation
*/

#pragma once

#include <cstdint>
#include "IPasswordHasher.hpp"

/**
 * @class Argon2PasswordHasher
 * @brief Argon2id password hashing wrapper
 * 
 * This class wraps the Argon2id algorithm for secure password hashing.
 * Argon2id is the recommended variant for password hashing as it provides
 * resistance against both side-channel and GPU attacks.
 * 
 * Default parameters:
 * - Time cost (iterations): 2
 * - Memory cost: 64 MiB
 * - Parallelism: 1 thread
 * - Hash length: 32 bytes
 * - Salt length: 16 bytes
 */
class Argon2PasswordHasher final : public IPasswordHasher {
   public:
    /**
     * @brief Constructor with default parameters
     */
    Argon2PasswordHasher();

    /**
     * @brief Constructor with custom parameters
     * @param timeCost Number of iterations (default: 2)
     * @param memoryCost Memory usage in KiB (default: 65536 = 64 MiB)
     * @param parallelism Number of threads (default: 1)
     * @param hashLength Length of the hash in bytes (default: 32)
     * @param saltLength Length of the salt in bytes (default: 16)
     */
    Argon2PasswordHasher(uint32_t timeCost, uint32_t memoryCost, uint32_t parallelism, size_t hashLength = 32,
                         size_t saltLength = 16);

    ~Argon2PasswordHasher() override = default;

    /**
     * @brief Hash a plaintext password using Argon2id
     * @param password The plaintext password to hash
     * @return std::string The encoded Argon2id hash
     * @throws std::runtime_error if hashing fails
     */
    std::string hash(const std::string &password) override;

    /**
     * @brief Verify a password against an Argon2id hash
     * @param password The plaintext password to verify
     * @param hash The Argon2id hash to verify against
     * @return bool True if the password matches the hash
     */
    bool verify(const std::string &password, const std::string &hash) override;

   private:
    uint32_t _timeCost;
    uint32_t _memoryCost;
    uint32_t _parallelism;
    size_t _hashLength;
    size_t _saltLength;
};
