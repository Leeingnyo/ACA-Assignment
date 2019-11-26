#include "motion.hpp"

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
