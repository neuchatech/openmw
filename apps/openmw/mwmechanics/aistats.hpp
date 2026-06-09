#ifndef OPENMW_MWMECHANICS_AISTATS_H
#define OPENMW_MWMECHANICS_AISTATS_H

#include <algorithm>
#include <atomic>
#include <cstddef>

#include <osg/Stats>

namespace MWMechanics::AiStats
{
    inline std::atomic<unsigned int> sEngageCombatChecks{ 0 };
    inline std::atomic<unsigned int> sSidingCacheMisses{ 0 };
    inline std::atomic<unsigned int> sStartCombatCalls{ 0 };
    inline std::atomic<unsigned int> sCombatPlayerChecks{ 0 };
    inline std::atomic<unsigned int> sCombatForceFlee{ 0 };
    inline std::atomic<unsigned int> sFleePathgridScans{ 0 };
    inline std::atomic<unsigned int> sPathBuilds{ 0 };
    inline std::atomic<unsigned int> sPathgridFallbacks{ 0 };
    inline std::atomic<unsigned int> sNavigatorFailures{ 0 };
    inline std::atomic<unsigned long long> sPathBuildMicroseconds{ 0 };

    inline void recordEngageCombatCheck()
    {
        sEngageCombatChecks.fetch_add(1, std::memory_order_relaxed);
    }

    inline void recordSidingCacheMiss()
    {
        sSidingCacheMisses.fetch_add(1, std::memory_order_relaxed);
    }

    inline void recordStartCombat()
    {
        sStartCombatCalls.fetch_add(1, std::memory_order_relaxed);
    }

    inline void recordCombatPlayerCheck()
    {
        sCombatPlayerChecks.fetch_add(1, std::memory_order_relaxed);
    }

    inline void recordCombatForceFlee()
    {
        sCombatForceFlee.fetch_add(1, std::memory_order_relaxed);
    }

    inline void recordFleePathgridScan(std::size_t points)
    {
        sFleePathgridScans.fetch_add(static_cast<unsigned int>(points), std::memory_order_relaxed);
    }

    inline void recordPathBuild(double milliseconds, bool usedPathgridFallback)
    {
        sPathBuilds.fetch_add(1, std::memory_order_relaxed);
        if (usedPathgridFallback)
            sPathgridFallbacks.fetch_add(1, std::memory_order_relaxed);
        sPathBuildMicroseconds.fetch_add(
            static_cast<unsigned long long>(std::max(0.0, milliseconds) * 1000.0), std::memory_order_relaxed);
    }

    inline void recordNavigatorFailure()
    {
        sNavigatorFailures.fetch_add(1, std::memory_order_relaxed);
    }

    inline unsigned int consume(std::atomic<unsigned int>& value)
    {
        return value.exchange(0, std::memory_order_relaxed);
    }

    inline unsigned long long consume(std::atomic<unsigned long long>& value)
    {
        return value.exchange(0, std::memory_order_relaxed);
    }

    inline void reportStats(unsigned int frameNumber, osg::Stats& stats)
    {
        stats.setAttribute(frameNumber, "AI Engage Checks", static_cast<double>(consume(sEngageCombatChecks)));
        stats.setAttribute(frameNumber, "AI Siding Cache Misses", static_cast<double>(consume(sSidingCacheMisses)));
        stats.setAttribute(frameNumber, "AI StartCombat", static_cast<double>(consume(sStartCombatCalls)));
        stats.setAttribute(frameNumber, "AI Combat Player Checks", static_cast<double>(consume(sCombatPlayerChecks)));
        stats.setAttribute(frameNumber, "AI Combat ForceFlee", static_cast<double>(consume(sCombatForceFlee)));
        stats.setAttribute(frameNumber, "AI Flee Pathgrid Scans", static_cast<double>(consume(sFleePathgridScans)));
        stats.setAttribute(frameNumber, "AI Path Builds", static_cast<double>(consume(sPathBuilds)));
        stats.setAttribute(frameNumber, "AI Pathgrid Fallbacks", static_cast<double>(consume(sPathgridFallbacks)));
        stats.setAttribute(frameNumber, "AI Navigator Failures", static_cast<double>(consume(sNavigatorFailures)));
        stats.setAttribute(
            frameNumber, "AI Path Build ms", static_cast<double>(consume(sPathBuildMicroseconds)) / 1000.0);
    }
}

#endif
