#pragma once

#include <algorithm>
#include <random>

namespace common
{
class Random
{
    std::mt19937 m_rng;

public:
    Random() :
    m_rng(std::mt19937(std::random_device{}())) {
    }

    bool chance(double c) {
        if (c <= 0) {
            return false;
        }

        if (c >= 100) {
            return true;
        }

        static std::uniform_real_distribution<double> dist(1, 100);
        return c > dist(m_rng);
    }

    std::mt19937& get_rng() {
        return m_rng;
    }

    template <typename T>
    T& random_element(const std::vector<T>& in) {
        std::uniform_int_distribution<int> dist(std::distance(in.begin(), in.end()));
        return in.at(dist(m_rng));
    }

    template <typename T>
    void random_elements(
        const std::vector<T>& in,
        size_t count,
        std::vector<T>& out
    ) {
        count = std::min(count, in.size());

        std::sample(
            in.begin(),
            in.end(),
            std::back_inserter(out),
            count,
            m_rng
        );
    }
};
}
