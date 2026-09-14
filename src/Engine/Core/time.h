#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>

enum class TimeDomain
{
    Scaled,
    Unscaled,
};

struct TimeState
{
    using clock = std::chrono::steady_clock;

    std::uint64_t frameIndex = 0;

    std::chrono::nanoseconds unscaledDelta{};
    std::chrono::nanoseconds delta{};

    std::chrono::nanoseconds unscaledTime{};
    std::chrono::nanoseconds time{};

    double timeScale = 1.0;
    bool paused = false;

    double unscaledDeltaSeconds() const { return std::chrono::duration<double>(unscaledDelta).count(); }
    double deltaSeconds() const { return std::chrono::duration<double>(delta).count(); }
    double unscaledTimeSeconds() const { return std::chrono::duration<double>(unscaledTime).count(); }
    double timeSeconds() const { return std::chrono::duration<double>(time).count(); }
};

class TimeSystem
{
public:
    using clock = TimeState::clock;

    void setPaused(bool paused) { m_state.paused = paused; }
    void setTimeScale(double timeScale) { m_state.timeScale = std::clamp(timeScale, 0.0, 100.0); }

    void setMaxDelta(std::chrono::nanoseconds maxDelta) { m_maxDelta = std::max(std::chrono::nanoseconds(0), maxDelta); }

    void setFixedDelta(std::chrono::nanoseconds fixedDelta)
    {
        m_fixedDelta = std::max(std::chrono::nanoseconds(0), fixedDelta);
        if (m_fixedDelta == std::chrono::nanoseconds(0)) {
            m_fixedDelta = std::chrono::milliseconds(16);
        }
    }

    void setMaxFixedSubSteps(int maxSubSteps) { m_maxFixedSubSteps = std::clamp(maxSubSteps, 1, 128); }

    const TimeState& state() const { return m_state; }

    void reset()
    {
        m_state = {};
        m_last = clock::time_point{};
        m_hasLast = false;
        m_fixedAccumulator = std::chrono::nanoseconds(0);
    }

    void beginFrame()
    {
        const auto now = clock::now();
        if (!m_hasLast) {
            m_last = now;
            m_hasLast = true;

            m_state.frameIndex = 0;
            m_state.unscaledDelta = std::chrono::nanoseconds(0);
            m_state.delta = std::chrono::nanoseconds(0);
            return;
        }

        auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_last);
        m_last = now;

        if (m_maxDelta.count() > 0) {
            dt = std::min(dt, m_maxDelta);
        }
        if (dt.count() < 0) {
            dt = std::chrono::nanoseconds(0);
        }

        m_state.frameIndex++;

        m_state.unscaledDelta = dt;
        m_state.unscaledTime += dt;

        if (m_state.paused) {
            m_state.delta = std::chrono::nanoseconds(0);
            return;
        }

        const double scaledSeconds = std::chrono::duration<double>(dt).count() * m_state.timeScale;
        const auto scaledDt = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(scaledSeconds));

        m_state.delta = std::max(std::chrono::nanoseconds(0), scaledDt);
        m_state.time += m_state.delta;
    }

    template <class FixedUpdateFn>
    int stepFixed(FixedUpdateFn&& fixedUpdate)
    {
        if (m_state.paused) {
            return 0;
        }

        m_fixedAccumulator += m_state.delta;

        int steps = 0;
        while (m_fixedAccumulator >= m_fixedDelta && steps < m_maxFixedSubSteps) {
            fixedUpdate(m_fixedDelta);
            m_fixedAccumulator -= m_fixedDelta;
            ++steps;
        }

        if (steps == m_maxFixedSubSteps && m_fixedAccumulator >= m_fixedDelta) {
            m_fixedAccumulator = std::chrono::nanoseconds(0);
        }

        return steps;
    }

    double fixedAlpha() const
    {
        if (m_fixedDelta.count() <= 0) {
            return 0.0;
        }
        return std::clamp(std::chrono::duration<double>(m_fixedAccumulator).count() / std::chrono::duration<double>(m_fixedDelta).count(), 0.0, 1.0);
    }

    std::chrono::nanoseconds fixedDelta() const { return m_fixedDelta; }

private:
    TimeState m_state{};

    clock::time_point m_last{};
    bool m_hasLast = false;

    std::chrono::nanoseconds m_maxDelta = std::chrono::milliseconds(250);

    std::chrono::nanoseconds m_fixedDelta = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(1.0 / 60.0));
    std::chrono::nanoseconds m_fixedAccumulator{};
    int m_maxFixedSubSteps = 8;
};
