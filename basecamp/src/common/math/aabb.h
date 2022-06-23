#pragma once

struct Aabb3 {
    Vector3 m_min, m_max;

    Aabb3() { reset(); }

    void reset()
    {
        m_min = Vector3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
        m_max = Vector3(numeric_limits<float>::lowest(), numeric_limits<float>::lowest(), numeric_limits<float>::lowest());
    }

    void expand(const Vector3& p)
    {
        m_min.x = min(m_min.x, p.x);
        m_min.y = min(m_min.y, p.y);
        m_min.z = min(m_min.z, p.z);

        m_max.x = max(m_max.x, p.x);
        m_max.y = max(m_max.y, p.y);
        m_max.z = max(m_max.z, p.z);
    }
};
