/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** IPasswordHasher - Interface for password hashing implementations
*/

#pragma once

#include <string>

/**
 * @interface IPasswordHasher
 * @brief Abstract interface for password hashing algorithms
 * 
 * This interface provides a uniform API for different password hashing
 * implementations (Argon2, bcrypt, scrypt, etc.)
 */
class IPasswordHasher {
   public:
    virtual ~IPasswordHasher() = default;

    /**
     * @brief Hash a plaintext password
     * @param password The plaintext password to hash
     * @return std::string The hashed password (encoded format)
     * @throws std::runtime_error if hashing fails
     */
    virtual std::string hash(const std::string &password) = 0;

    /**
     * @brief Verify a password against a hash
     * @param password The plaintext password to verify
     * @param hash The hash to verify against
     * @return bool True if the password matches the hash
     */
    virtual bool verify(const std::string &password, const std::string &hash) = 0;
};
