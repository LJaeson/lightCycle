#pragma once

#include <Player/Player.hpp>
#include <Player/Bot.hpp>
#include <string>

std::unique_ptr<Player> PlayerFactory(std::string playerType) {
    if (playerType == "Client") {
        return std::make_unique<Client>();
    }
    return nullptr;
}

std::unique_ptr<Player> PlayerFactory(std::string playerType, int w, int h) {
    if (playerType == "Bot") {
        return std::make_unique<Bot>(w, h);
    }
    return nullptr;
}