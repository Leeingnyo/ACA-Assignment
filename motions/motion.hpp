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
            if (!(orientations.size() == rhs.orientations.size()))
                throw std::runtime_error("dimensions are not same.");
            position = rhs.position;
            orientations = rhs.orientations;
        }
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
        for (int i = 0; i < vectors.size(); i++) {
            d.vectors.push_back(vectors[i] * alpha);
        }
        return d;
    }

    Displacement operator-() const {
        // 이런 건 map 인가 쓰면 더 빠른가
        return *this * -1;
    }

    Displacement operator-(const Displacement& d) const {
        if (!(vectors.size() == d.vectors.size()))
            throw std::runtime_error("dimensions are not same.");
        return *this + (-d);
    }
};

Motion Motion::operator+(const Displacement& d) const {
    if (!((orientations.size() + 1) == d.vectors.size()))
        throw std::runtime_error("dimensions are not same.");
    Motion m;
    m.position = position + (orientations[0] * d.vectors[0]);
    for (int i = 1; i < d.vectors.size(); i++) {
        const auto& angleAxis = d.vectors[i];
        m.orientations.push_back(orientations[i - 1] * Eigen::Quaterniond(Eigen::AngleAxisd(angleAxis.norm(), angleAxis.normalized())));
    }
    return m;
}

Motion Motion::operator-(const Displacement& d) const {
    return (*this) + (-d);
}

Displacement Motion::operator-(const Motion& m) const {
    if (!(orientations.size() == m.orientations.size()))
        throw std::runtime_error("dimensions are not same.");

    Displacement d;
    d.vectors.push_back(m.orientations[0].inverse() * (this->position - m.position));
    for (int i = 0; i < m.orientations.size(); i++) {
        auto&& angleAxis = Eigen::AngleAxisd(m.orientations[i].inverse() * this->orientations[i]);
        d.vectors.push_back(angleAxis.angle() * angleAxis.axis());
    }

    return d;
}
