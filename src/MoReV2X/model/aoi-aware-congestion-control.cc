#include "aoi-aware-congestion-control.h"
#include "ns3/log.h"
#include <set>
#include <cstdint>
#include <numeric>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AdaptiveResourceReservation");

AdaptiveResourceReservation::AdaptiveResourceReservation(
    uint16_t initialRRI, 
    double initialPersistenceProbability
) : m_rri(initialRRI), m_persistenceProbability(initialPersistenceProbability) {
    NS_LOG_FUNCTION(this << static_cast<int>(initialRRI) << initialPersistenceProbability);
}

uint16_t AdaptiveResourceReservation::UpdateRRI(uint16_t rri, double pi0, const std::vector<uint16_t>& neighborRRI) {
    std::cout << "pi0: " << pi0 << std::endl;
    std::cout << "Neighbor RRIs: ";
    for (uint16_t r : neighborRRI) {
        std::cout << r << " ";
    }
    std::cout << std::endl;

    int R_tilde = static_cast<int>(m_rri);
    if (R_tilde == 0) {
        R_tilde = rri;
    }

    if (pi0 < P0_INF) {
        int oldRRI = R_tilde;
        R_tilde = std::min(R_tilde + DELTA_R, RRI_MAX);
   
        m_persistenceProbability = std::max(m_persistenceProbability - BETA, 0.0);

    } else if (pi0 > P0_SUP) {

        int oldRRI = R_tilde;
        R_tilde = std::max(R_tilde - DELTA_R, RRI_MIN);

        m_persistenceProbability = std::min(m_persistenceProbability + BETA, P_MAX);
    } 

    if (!neighborRRI.empty()) {
        double avgNeighborRRI = 0;
        for (uint16_t r : neighborRRI) {
            avgNeighborRRI += r;
        }
        avgNeighborRRI /= neighborRRI.size();
        
        int oldRRI = R_tilde;
        R_tilde = static_cast<int>(BETA * R_tilde + (1.0 - BETA) * avgNeighborRRI);
        
        R_tilde = std::max(RRI_MIN, std::min(R_tilde, RRI_MAX));
    }

    std::cout << "\nFinal values:" << std::endl;
    std::cout << "Final RRI: " << R_tilde << std::endl;
    std::cout << "Final persistence probability: " << m_persistenceProbability << std::endl;

    m_rri = static_cast<uint16_t>(R_tilde);
    return m_rri;
}

uint16_t AdaptiveResourceReservation::GetRRI() const {
    NS_LOG_FUNCTION(this);
    return m_rri;
}

double AdaptiveResourceReservation::GetPersistenceProbability() const {
    NS_LOG_FUNCTION(this);
    return m_persistenceProbability;
}

} // namespace ns3