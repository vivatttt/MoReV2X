#include "adaptive-rri-algorithm.h"
#include "ns3/log.h"
#include <numeric>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AdaptiveResourceReservation");

const double AdaptiveResourceReservation::P_MAX = 0.8;
const double AdaptiveResourceReservation::BETA = 0.25;
const int AdaptiveResourceReservation::DELTA_R = 40;
const double AdaptiveResourceReservation::P0_INF = 0.1;
const double AdaptiveResourceReservation::P0_SUP = 0.7;
const double AdaptiveResourceReservation::PI0_STAR = 0.412;
const uint8_t AdaptiveResourceReservation::RRI_MIN = 1;
const uint8_t AdaptiveResourceReservation::RRI_MAX = 100;

AdaptiveResourceReservation::AdaptiveResourceReservation(
    uint8_t initialRRI, 
    double initialPersistenceProbability
) : m_rri(initialRRI), m_persistenceProbability(initialPersistenceProbability) {
    NS_LOG_FUNCTION(this << static_cast<int>(initialRRI) << initialPersistenceProbability);
}

void AdaptiveResourceReservation::Update(double pi0, std::vector<uint8_t> neighborRRI) {
    NS_LOG_FUNCTION(this << pi0);

    if (pi0 < P0_INF) {
        m_rri = std::min(static_cast<int>(m_rri) + DELTA_R, static_cast<int>(RRI_MAX));
    } else if (pi0 > P0_SUP) {
        m_rri = std::max(static_cast<int>(m_rri) - DELTA_R, static_cast<int>(RRI_MIN));
    }

    int rTilde = m_rri + ((pi0 > P0_SUP) ? -DELTA_R : DELTA_R);
    double rneiAvg = std::accumulate(neighborRRI.begin(), neighborRRI.end(), 0.0) / neighborRRI.size();
    m_rri = static_cast<uint8_t>(BETA * rTilde + (1 - BETA) * rneiAvg);
    m_rri = std::clamp(m_rri, RRI_MIN, RRI_MAX);

    if (pi0 < P0_INF) {
        m_persistenceProbability = std::max(m_persistenceProbability - BETA, 0.0);
    } else if (pi0 > P0_SUP) {
        m_persistenceProbability = std::min(m_persistenceProbability + BETA, P_MAX);
    }

    NS_LOG_INFO("Updated RRI: " << static_cast<int>(m_rri) << ", Persistence Probability: " << m_persistenceProbability);
}

uint8_t AdaptiveResourceReservation::GetRRI() const {
    NS_LOG_FUNCTION(this);
    return m_rri;
}

double AdaptiveResourceReservation::GetPersistenceProbability() const {
    NS_LOG_FUNCTION(this);
    return m_persistenceProbability;
}

std::vector<uint8_t> AdaptiveResourceReservation::GetNeighborRRI(
    const std::map<uint16_t, std::map<SubframeInfo, std::vector<ReservedCSR>>>& sensedReservedCSRMap
) {
    std::vector<uint8_t> neighborRRI;
    std::set<uint32_t> processedNodes;

    for (const auto& csrEntry : sensedReservedCSRMap) {
        const auto& frameMap = csrEntry.second;

        for (const auto& frameEntry : frameMap) {
            const auto& reservedList = frameEntry.second;

            for (const ReservedCSR& reservation : reservedList) {
                if (processedNodes.find(reservation.nodeId) == processedNodes.end()) {
                    neighborRRI.push_back(reservation.RRI);
                    processedNodes.insert(reservation.nodeId);
                }
            }
        }
    }

    return neighborRRI;
}

double AdaptiveResourceReservation::CalculateFreeSubchannelRatio(
    const std::map<uint16_t, std::map<SubframeInfo, std::vector<ReservedCSR>>>& sensedReservedCSRMap,
    uint16_t totalRBs,
    uint16_t subchannelSize,
    uint32_t currentFrameNo,
    uint32_t currentSubframeNo,
    uint8_t RRI_slots,
    double slotDuration
) {
    uint32_t totalSubchannels = totalRBs / subchannelSize;
    std::set<uint16_t> occupiedSubchannels;

    currentFrameNo--;
    currentSubframeNo--;

    for (const auto& csrEntry : sensedReservedCSRMap) {
        uint16_t csrIndex = csrEntry.first;
        const auto& frameMap = csrEntry.second;

        for (const auto& frameEntry : frameMap) {
            const SubframeInfo& sensedSF = frameEntry.first;

            uint32_t slotsDifference = SubtractFrames(
                sensedSF.frameNo,
                currentFrameNo,
                sensedSF.subframeNo,
                currentSubframeNo
            );

            if (slotsDifference < RRI_slots * slotDuration) {
                occupiedSubchannels.insert(csrIndex);
            }
        }
    }

    uint32_t freeSubchannels = totalSubchannels - occupiedSubchannels.size();
    return static_cast<double>(freeSubchannels) / totalSubchannels;
}

uint32_t AdaptiveResourceReservation::SubtractFrames(uint32_t frameNo1, uint32_t frameNo2, uint32_t subframeNo1, uint32_t subframeNo2) {
    int totalSubframes1 = frameNo1 * 10 + subframeNo1;
    int totalSubframes2 = frameNo2 * 10 + subframeNo2;

    return static_cast<uint32_t>(totalSubframes1 - totalSubframes2);
}

} // namespace ns3
