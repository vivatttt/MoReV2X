#ifndef AIO_CONGESTION_CONTROL_UE_MAC_H
#define AIO_CONGESTION_CONTROL_UE_MAC_H

#include <algorithm>
#include <vector>
#include <map>
#include <cstdint>

namespace ns3 {

struct SubframeInfo {
    uint32_t frameNo;
    uint32_t subframeNo;
};

struct ReservedCSR {
    double psschRsrpDb;
    double reservationTime;
    uint32_t nodeId;
    uint8_t RRI;
    SubframeInfo reservedSF;
    bool isReTx;
    bool isSameTB;
    uint16_t rbStart;
    uint16_t rbLen;
    uint32_t CreselRx;
};

class AdaptiveResourceReservation {
public:
    AdaptiveResourceReservation(uint8_t initialRRI, double initialPersistenceProbability);

    void Update(double pi0, std::vector<uint8_t> neighborRRI);

    uint8_t GetRRI() const;
    double GetPersistenceProbability() const;

    static std::vector<uint8_t> GetNeighborRRI(
        const std::map<uint16_t, std::map<SubframeInfo, std::vector<ReservedCSR>>>& sensedReservedCSRMap
    );
    static double CalculateFreeSubchannelRatio(
        const std::map<uint16_t, std::map<SubframeInfo, std::vector<ReservedCSR>>>& sensedReservedCSRMap,
        uint16_t totalRBs,
        uint16_t subchannelSize,
        uint32_t currentFrameNo,
        uint32_t currentSubframeNo,
        uint8_t RRI_slots,
        double slotDuration
    );
    static uint32_t SubtractFrames(uint32_t frameNo1, uint32_t frameNo2, uint32_t subframeNo1, uint32_t subframeNo2);

private:
    uint8_t m_rri;
    double m_persistenceProbability;

    static const double P_MAX;
    static const double BETA;
    static const int DELTA_R;
    static const double P0_INF;
    static const double P0_SUP;
    static const double PI0_STAR;
    static const uint8_t RRI_MIN;
    static const uint8_t RRI_MAX;
};

} // namespace ns3

#endif // AIO_CONGESTION_CONTROL_UE_MAC_H
