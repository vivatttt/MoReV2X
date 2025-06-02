#ifndef AOI_AWARE_CONGESTION_CONTROL_H
#define AOI_AWARE_CONGESTION_CONTROL_H

#include <vector>
#include <map>
#include <cstdint>
#include "nist-sl-pool.h"

namespace ns3 {

class AdaptiveResourceReservation {
public:
    AdaptiveResourceReservation(uint16_t initialRRI, double initialPersistenceProbability);
    
    uint16_t UpdateRRI(uint16_t rri, double pi0, const std::vector<uint16_t>& neighborRRI);
    uint16_t GetRRI() const;
    double GetPersistenceProbability() const;
    
private:
    uint16_t m_rri;
    double m_persistenceProbability;

    static constexpr double P_MAX = 0.8;
    static constexpr double BETA = 0.25;
    static constexpr int DELTA_R = 40;
    static constexpr double P0_INF = 0.3;
    static constexpr double P0_SUP = 0.5;
    static constexpr int RRI_MIN = 20;
    static constexpr int RRI_MAX = 100;
};

} // namespace ns3

#endif // AOI_AWARE_CONGESTION_CONTROL_H