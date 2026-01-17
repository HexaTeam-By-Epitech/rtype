/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Argon2PasswordHasher - Argon2id password hashing implementation
*/

#include "Argon2PasswordHasher.hpp"
#include <argon2.h>
#include <random>
#include <stdexcept>

Argon2PasswordHasher::Argon2PasswordHasher()
    : _timeCost(2), _memoryCost(65536), _parallelism(1), _hashLength(32), _saltLength(16) {}

Argon2PasswordHasher::Argon2PasswordHasher(uint32_t timeCost, uint32_t memoryCost, uint32_t parallelism,
                                           size_t hashLength, size_t saltLength)
    : _timeCost(timeCost),
      _memoryCost(memoryCost),
      _parallelism(parallelism),
      _hashLength(hashLength),
      _saltLength(saltLength) {}

std::string Argon2PasswordHasher::hash(const std::string &password) {
    if (password.empty()) {
        throw std::invalid_argument("Password cannot be empty");
    }

    // Generate random salt
    std::vector<unsigned char> salt(_saltLength);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (size_t i = 0; i < _saltLength; ++i) {
        salt[i] = static_cast<unsigned char>(dis(gen));
    }

    // Allocate buffer for encoded hash
    const size_t encodedLength = 128;
    std::vector<char> encoded(encodedLength);

    // Hash password using Argon2id
    int result =
        argon2id_hash_encoded(_timeCost, _memoryCost, _parallelism, password.c_str(), password.length(),
                              salt.data(), _saltLength, _hashLength, encoded.data(), encodedLength);

    if (result != ARGON2_OK) {
        throw std::runtime_error(std::string("Argon2 hashing failed: ") + argon2_error_message(result));
    }

    return std::string(encoded.data());
}

bool Argon2PasswordHasher::verify(const std::string &password, const std::string &hash) {
    if (password.empty() || hash.empty()) {
        return false;
    }

    // Verify password against Argon2id hash
    int result = argon2id_verify(hash.c_str(), password.c_str(), password.length());

    return result == ARGON2_OK;
}
