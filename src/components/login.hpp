#pragma once

#include "../api.hpp";

namespace component_login {
    void init() {
        api::login([](const std::string &msg) {
            
        });
    }
}