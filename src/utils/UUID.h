#pragma once
#include <random>
#include <cstdint>

namespace Lengine {

    class UUID {
    public:
        UUID() {
            static std::random_device rd;
            static std::mt19937_64 gen(rd());
            m_UUID = gen();
        }

        explicit UUID(uint64_t uuid) : m_UUID(uuid) {}

        operator uint64_t() const { return m_UUID; }
        uint64_t value() const { return m_UUID; }

        static const UUID Null;

        bool isNull() const { return value() == 0; }

        bool operator==(const UUID& other) const {
            return m_UUID == other.m_UUID;
        }
        uint64_t toUint64() const { return m_UUID; }


    private:
        uint64_t m_UUID;
    };

}

namespace std {
    template<>
    struct hash<Lengine::UUID> {
        std::size_t operator()(const Lengine::UUID& uuid) const noexcept {
            return std::hash<uint64_t>()(uuid.value());
        }
    };
}
