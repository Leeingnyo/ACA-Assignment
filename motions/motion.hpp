#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>

#include "../Eigen/Dense"

#define SAME_DIMENSION(x, y) ((x).orientations.size() == (y).orientations.size())

#define SAME_DIMENSION_PRINT(x, y) (std::cout << (x).orientations.size() << " " << (y).orientations.size() << \
        " | " << ((x).orientations.size() == (y).orientations.size()) << std::endl)

class Displacement;

class Motion {
public:
    Eigen::Vector3d position;
    std::vector<Eigen::Quaterniond> orientations;

    Motion& operator=(const Motion& rhs) {
        if (this != &rhs) {
            if (!SAME_DIMENSION(*this, rhs))
                throw std::runtime_error("dimensions are not same.");
            position = rhs.position;
            orientations = rhs.orientations;
        }
    }

    Motion& operator+(const Displacement& d) const;
    Motion operator-(const Displacement& d) const;

    Displacement operator-(const Motion& m) const;
};

class Displacement {
public:
    Eigen::Vector3d position;
    std::vector<Eigen::Quaterniond> orientations;

    Displacement operator+(const Displacement& d) const {
        if (!SAME_DIMENSION(*this, d))
            throw std::runtime_error("dimensions are not same.");
    }

    Displacement operator-() const {
    }

    Displacement operator-(const Displacement& d) const {
        if (!SAME_DIMENSION(*this, d))
            throw std::runtime_error("dimensions are not same.");
    }
};

Motion& Motion::operator+(const Displacement& d) const {
    if (!SAME_DIMENSION(*this, d))
        throw std::runtime_error("dimensions are not same.");
}

Motion Motion::operator-(const Displacement& d) const {
    if (!SAME_DIMENSION(*this, d))
        throw std::runtime_error("dimensions are not same.");
}

Displacement Motion::operator-(const Motion& m) const {
    if (!SAME_DIMENSION(*this, m)) {
        throw std::runtime_error("dimensions are not same.");
    }

    Displacement d;
    d.position = this->position - m.position;
    for (int i = 0; i < m.orientations.size(); i++) {
        d.orientations.push_back(Eigen::Quaterniond(this->orientations[i] * Eigen::AngleAxisd(m.orientations[i])));
    }

    return d;
}
