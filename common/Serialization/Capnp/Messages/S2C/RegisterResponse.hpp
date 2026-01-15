/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** RegisterResponse.hpp
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
 * @class RegisterResponse
 * @brief Server response to registration request
 */
            class RegisterResponse {
               public:
                bool success;
                std::string message;

                RegisterResponse() = default;
                RegisterResponse(bool s, const std::string &msg) : success(s), message(msg) {}

                /**
     * @brief Serialize to Cap'n Proto binary format
     */
                std::vector<uint8_t> serialize() const {
                    ::capnp::MallocMessageBuilder msgBuilder;
                    auto response = msgBuilder.initRoot<::RegisterResponse>();

                    response.setSuccess(success);
                    response.setMessage(message);

                    kj::Array<capnp::word> words = messageToFlatArray(msgBuilder);
                    auto bytes = words.asBytes();
                    return std::vector<uint8_t>(bytes.begin(), bytes.end());
                }

                /**
     * @brief Deserialize from Cap'n Proto binary format
     */
                static RegisterResponse deserialize(const std::vector<uint8_t> &data) {
                    kj::ArrayPtr<const capnp::word> words(reinterpret_cast<const capnp::word *>(data.data()),
                                                          data.size() / sizeof(capnp::word));

                    ::capnp::FlatArrayMessageReader msgReader(words);
                    auto response = msgReader.getRoot<::RegisterResponse>();

                    RegisterResponse result;
                    result.success = response.getSuccess();
                    result.message = response.getMessage().cStr();

                    return result;
                }
            };

        }  // namespace S2C
    }  // namespace Messages
}  // namespace RType
