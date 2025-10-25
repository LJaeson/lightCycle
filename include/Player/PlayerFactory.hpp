#pragma once

#include <Player/Player.hpp>
#include <string>

std::unique_ptr<Player> PlayerFactory(std::string playerType) {
    if (playerType == "Client") {
        return std::make_unique<Client>();
    }
    return nullptr;
}