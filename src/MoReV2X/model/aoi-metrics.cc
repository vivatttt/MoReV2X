#include "aoi-metrics.h"
#include "ns3/log.h"
#include <algorithm>
#include <numeric>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AoIMetrics");

AoIMetrics::AoIMetrics(std::string outputPath) : m_outputPath(outputPath) {
    m_aoiFile.open(m_outputPath + "aoi_values.csv");
    m_aoiFile << "Time,NodeID,CurrentAoI,PeakAoI" << std::endl;
    
    m_statsFile.open(m_outputPath + "aoi_statistics.csv");
    m_statsFile << "NodeID,AverageAoI,PeakAoI,UpdateCount" << std::endl;
}

AoIMetrics::~AoIMetrics() {
    CalculateStatistics();
    m_aoiFile.close();
    m_statsFile.close();
}

void AoIMetrics::UpdateAoI(uint32_t nodeId, double currentTime, double generationTime) {
    NodeAoIStats& stats = m_nodeStats[nodeId];
    
    // Calculate current AoI
    double currentAoI = currentTime - generationTime;
    stats.currentAoI = currentAoI;
    
    // Update peak AoI if necessary
    stats.peakAoI = std::max(stats.peakAoI, currentAoI);
    
    // Update sum for average calculation
    stats.sumAoI += currentAoI;
    stats.updateCount++;
    
    // Store AoI value for later analysis
    stats.aoiValues.push_back(currentAoI);
    
    // Update last update time
    stats.lastUpdateTime = currentTime;
    
    // Log current values
    m_aoiFile << currentTime << "," << nodeId << "," 
              << currentAoI << "," << stats.peakAoI << std::endl;
}

void AoIMetrics::LogCurrentAoI(double simTime) {
    for (auto& pair : m_nodeStats) {
        uint32_t nodeId = pair.first;
        NodeAoIStats& stats = pair.second;
        
        // Calculate current AoI based on time since last update
        double currentAoI = simTime - stats.lastUpdateTime;
        stats.currentAoI = currentAoI;
        
        // Update peak AoI if necessary
        stats.peakAoI = std::max(stats.peakAoI, currentAoI);
        
        // Log values
        m_aoiFile << simTime << "," << nodeId << "," 
                  << currentAoI << "," << stats.peakAoI << std::endl;
    }
}

void AoIMetrics::CalculateStatistics() {
    for (const auto& pair : m_nodeStats) {
        uint32_t nodeId = pair.first;
        const NodeAoIStats& stats = pair.second;
        
        // Calculate average AoI
        double averageAoI = stats.updateCount > 0 ? 
            stats.sumAoI / stats.updateCount : 0;
        
        // Write statistics to file
        m_statsFile << nodeId << "," 
                   << averageAoI << "," 
                   << stats.peakAoI << "," 
                   << stats.updateCount << std::endl;
    }
}

} // namespace ns3 