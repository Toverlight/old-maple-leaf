#pragma once

#include <algorithm>
#include <chrono>
#include <thread>

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#include <immintrin.h>
#endif

class FramePacer
{
public:
    using clock = std::chrono::steady_clock;

    FramePacer() = default;

    // maxFps <= 0 disables pacing.
    explicit FramePacer(int maxFps, int spinWaitUs = 500)
    {
        configure(maxFps, spinWaitUs);
    }

    void configure(int maxFps, int spinWaitUs = 500)
    {
        m_enabled = (maxFps > 0);
        m_maxFps = std::max(0, maxFps);
        m_spinWaitUs = std::clamp(spinWaitUs, 0, 5000);

        if (m_enabled) {
            m_targetDt = std::chrono::duration_cast<clock::duration>(std::chrono::duration<double>(1.0 / static_cast<double>(m_maxFps)));
            if (m_targetDt <= clock::duration::zero()) {
                m_enabled = false;
            }
        }

        reset();
    }

    void reset()
    {
        m_next = clock::time_point{};
        m_hasNext = false;
    }

    // Call once per frame, typically at the end of the frame, after swap.
    // Uses sleep (coarse) + spin/yield (fine) to hit a stable cadence.
    void pace()
    {
        if (!m_enabled) {
            return;
        }

        const auto now = clock::now();
        if (!m_hasNext) {
            m_next = now + m_targetDt;
            m_hasNext = true;
        } else {
            // Keep a stable cadence without drift.
            m_next += m_targetDt;
            if (now > m_next) {
                // If we fell behind (e.g., breakpoint, hitch), resync to avoid a long catch-up loop.
                m_next = now + m_targetDt;
            }
        }

        auto remaining = m_next - now;
        if (remaining <= clock::duration::zero()) {
            return;
        }

        const auto spinThreshold = std::chrono::microseconds(m_spinWaitUs);
        if (remaining > spinThreshold) {
            std::this_thread::sleep_for(remaining - spinThreshold);
        }

        // Fine wait: busy loop/yield to reduce jitter.
        while (clock::now() < m_next) {
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
            _mm_pause();
#else
            std::this_thread::yield();
#endif
        }
    }

private:
    bool m_enabled = false;
    int m_maxFps = 0;
    int m_spinWaitUs = 500;

    clock::duration m_targetDt{};
    clock::time_point m_next{};
    bool m_hasNext = false;
};
