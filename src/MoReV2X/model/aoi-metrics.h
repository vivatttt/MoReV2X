#ifndef AOI_METRICS_H
#define AOI_METRICS_H

#include <vector>
#include <map>
#include <fstream>
#include "ns3/nstime.h"
#include "ns3/node.h"

namespace ns3 {

class AoIMetrics {
public:
    AoIMetrics(std::string outputPath);
    ~AoIMetrics();

    void UpdateAoI(uint32_t nodeId, double currentTime, double generationTime);
    void LogCurrentAoI(double simTime);
    void CalculateStatistics();

private:
    struct NodeAoIStats {
        double lastUpdateTime;
        double currentAoI;
        double peakAoI;
        double sumAoI;
        uint32_t updateCount;
        std::vector<double> aoiValues;
        
        NodeAoIStats() : lastUpdateTime(0), currentAoI(0), 
                        peakAoI(0), sumAoI(0), updateCount(0) {}
    };

    std::map<uint32_t, NodeAoIStats> m_nodeStats;
    std::string m_outputPath;
    std::ofstream m_aoiFile;
    std::ofstream m_statsFile;
};

} // namespace ns3

#endif // AOI_METRICS_H 