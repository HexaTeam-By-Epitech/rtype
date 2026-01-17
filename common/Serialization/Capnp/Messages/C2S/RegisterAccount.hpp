/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** RegisterAccount.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/array.h>
#include <schemas/c2s_messages.capnp.h>
#include <string>
#include <vector>

namespace RType {
    namespace Messages {
        namespace C2S {

            /**
 * @class RegisterAccount
 * @brief Request to register a new user account
 */
            class RegisterAccount {
               public:
                std::string username;
                std::string password;

                RegisterAccount() = default;
                RegisterAccount(const std::string &user, const std::string &pass)
                    : username(user), password(pass) {}

                /**
     * @brief Serialize to Cap'n Proto binary format
     */
                std::vector<uint8_t> serialize() const {
                    ::capnp::MallocMessageBuilder message;
                    auto request = message.initRoot<::RegisterAccount>();

                    request.setUsername(username);
                    request.setPassword(password);

                    kj::Array<capnp::word> words = messageToFlatArray(message);
                    auto bytes = words.asBytes();
                    return std::vector<uint8_t>(bytes.begin(), bytes.end());
                }

                /**
     * @brief Deserialize from Cap'n Proto binary format
     */
                static RegisterAccount deserialize(const std::vector<uint8_t> &data) {
                    kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(data.data()),
                                                          data.size() / sizeof(capnp::word));

                    ::capnp::FlatArrayMessageReader message(words);
                    auto request = message.getRoot<::RegisterAccount>();

                    RegisterAccount result;
                    result.username = request.getUsername().cStr();
                    result.password = request.getPassword().cStr();

                    return result;
                }
            };

        }  // namespace C2S
    }  // namespace Messages
}  // namespace RType
