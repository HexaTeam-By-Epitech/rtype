/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** LoginResponse.hpp
*/

#pragma once

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/array.h>
#include <schemas/s2c_messages.capnp.h>
#include <string>
#include <vector>

namespace RType {
    namespace Messages {
        namespace S2C {

            /**
 * @class LoginResponse
 * @brief Server response to login request
 */
            class LoginResponse {
               public:
                bool success;
                std::string message;
                std::string sessionToken;

                LoginResponse() = default;
                LoginResponse(bool s, const std::string &msg, const std::string &token = "")
                    : success(s), message(msg), sessionToken(token) {}

                /**
     * @brief Serialize to Cap'n Proto binary format
     */
                std::vector<uint8_t> serialize() const {
                    ::capnp::MallocMessageBuilder msgBuilder;
                    auto response = msgBuilder.initRoot<::LoginResponse>();

                    response.setSuccess(success);
                    response.setMessage(message);
                    response.setSessionToken(sessionToken);

                    kj::Array<capnp::word> words = messageToFlatArray(msgBuilder);
                    auto bytes = words.asBytes();
                    return std::vector<uint8_t>(bytes.begin(), bytes.end());
                }

                /**
     * @brief Deserialize from Cap'n Proto binary format
     */
                static LoginResponse deserialize(const std::vector<uint8_t> &data) {
                    kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(data.data()),
                                                          data.size() / sizeof(capnp::word));

                    ::capnp::FlatArrayMessageReader msgReader(words);
                    auto response = msgReader.getRoot<::LoginResponse>();

                    LoginResponse result;
                    result.success = response.getSuccess();
                    result.message = response.getMessage().cStr();
                    result.sessionToken = response.getSessionToken().cStr();

                    return result;
                }
            };

        }  // namespace S2C
    }  // namespace Messages
}  // namespace RType
