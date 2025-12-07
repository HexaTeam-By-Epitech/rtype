/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** CapnpGameCodec.cpp
*/

#include "CapnpGameCodec.hpp"

#include "GameMessages.hpp"
#include <kj/io.h>

// NOTE: In a real project, you would include the generated Cap'n Proto
// schema header here, e.g.:
// #include "game_messages.capnp.h"
// For this example, we only document the expected pattern in comments.

CapnpGameCodec::CapnpGameCodec() = default;

std::vector<uint8_t> CapnpGameCodec::encodeSpawn(const SpawnEntityMessage &message) {
    // Example encoding flow using a builder-based IMessage wrapper. In a real
    // implementation, the filler lambda would construct a Cap'n Proto
    // SpawnEntity root from the gameplay message fields:
    //
    //   auto root = builder.initRoot<game::SpawnEntity>();
    //   root.setId(message.id);
    //   root.setX(message.x);
    //   root.setY(message.y);
    //
    auto iMsg = makeCapnpBuilderMessage([
            id = message.id,
            x = message.x,
            y = message.y
        ](capnp::MallocMessageBuilder &builder) {
            // TODO: Replace this pseudo-code with concrete Cap'n Proto schema code.
            // auto root = builder.initRoot<game::SpawnEntity>();
            // root.setId(id);
            // root.setX(x);
            // root.setY(y);
            (void)builder;
            (void)id;
            (void)x;
            (void)y;
        });

    return _serializer.encode(*iMsg);
}

SpawnEntityMessage CapnpGameCodec::decodeSpawn(const std::vector<uint8_t> &data) {
    // Example decoding flow using a Cap'n Proto reader:
    //
    //   kj::ArrayPtr<const capnp::word> words(
    //       reinterpret_cast<const capnp::word*>(data.data()),
    //       data.size() / sizeof(capnp::word));
    //   capnp::FlatArrayMessageReader reader(words);
    //   auto root = reader.getRoot<game::SpawnEntity>();
    //
    //   SpawnEntityMessage msg;
    //   msg.id = root.getId();
    //   msg.x  = root.getX();
    //   msg.y  = root.getY();
    //   return msg;
    //
    // For now, we simply construct an empty message to illustrate the API.
    SpawnEntityMessage msg;
    (void)data; // suppress unused warning for the example implementation
    return msg;
}