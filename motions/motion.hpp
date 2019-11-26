#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>

#include "../Eigen/Dense"

class Displacement;

class Motion {
public:
    Eigen::Vector3d position;
    std::vector<Eigen::Quaterniond> orientations;

    Motion& operator=(const Motion& rhs) {
        if (this != &rhs) {
            position = rhs.position;
            orientations = rhs.orientations;
        }
        return *this;
    }

    Motion operator+(const Displacement& d) const;
    Motion operator-(const Displacement& d) const;

    Displacement operator-(const Motion& m) const;
};

class Displacement {
public:
    std::vector<Eigen::Vector3d> vectors;

    Displacement operator+(const Displacement& d) const {
        if (!(vectors.size() == d.vectors.size()))
            throw std::runtime_error("dimensions are not same.");
        throw std::runtime_error("sum operation of displacement is not implemented.");
    }

    Displacement operator*(double alpha) const {
        Displacement d;
        // 이런 건 map 인가 쓰면 더 빠른가
        for (int i = 0; i < vectors.size(); i++) {
            d.vectors.push_back(vectors[i] * alpha);
        }
        return d;
    }

    Displacement operator-() const {
        return *this * -1;
    }

    Displacement operator-(const Displacement& d) const {
        if (!(vectors.size() == d.vectors.size()))
            throw std::runtime_error("dimensions are not same.");
        return *this + (-d);
    }
};
