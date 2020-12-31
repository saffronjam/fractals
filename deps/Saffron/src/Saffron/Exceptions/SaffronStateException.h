#pragma once

#include <exception>

#include "Saffron/Base.h"

namespace Se {
    class SaffronStateException : public std::exception {
    public:
        explicit SaffronStateException(const String &msg) : _msg() {
        }

    private:
        std::string _msg;
    };
}
