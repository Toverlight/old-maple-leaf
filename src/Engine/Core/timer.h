#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <queue>
#include <unordered_set>
#include <vector>

#include "time.h"

struct TimerHandle
{
    std::uint64_t id = 0;
    explicit operator bool() const { return id != 0; }
};

class TimerSystem
{
public:
    using Callback = std::function<void()>;

    void reset()
    {
        m_nextId = 1;
        m_cancelled.clear();
        m_queue = {};
    }

    TimerHandle setTimeout(const TimeState& time, std::chrono::nanoseconds delay, Callback cb, TimeDomain domain = TimeDomain::Scaled)
    {
        return enqueue(time, delay, std::chrono::nanoseconds(0), false, std::move(cb), domain);
    }

    TimerHandle setInterval(const TimeState& time, std::chrono::nanoseconds period, Callback cb, TimeDomain domain = TimeDomain::Scaled)
    {
        return enqueue(time, period, period, true, std::move(cb), domain);
    }

    void cancel(TimerHandle handle)
    {
        if (!handle) {
            return;
        }
        m_cancelled.insert(handle.id);
    }

    // Call once per frame, after TimeSystem::beginFrame().
    void update(const TimeState& time)
    {
        while (!m_queue.empty()) {
            const Entry& e = m_queue.top();
            const auto t = currentTimeFor(time, e.domain);
            if (e.due > t) {
                break;
            }

            const std::uint64_t id = e.id;
            const bool repeating = e.repeating;
            const auto period = e.period;
            const auto domain = e.domain;
            Callback cb = e.cb;

            m_queue.pop();

            if (isCancelled(id)) {
                continue;
            }

            if (cb) {
                cb();
            }

            if (repeating && !isCancelled(id)) {
                Entry next = {};
                next.id = id;
                next.domain = domain;
                next.repeating = true;
                next.period = period;
                next.cb = std::move(cb);

                // Keep a stable cadence by advancing due time by whole periods.
                auto due = e.due;
                do {
                    due += period;
                } while (due <= t);
                next.due = due;

                m_queue.push(std::move(next));
            } else {
                // One-shot finished.
                m_cancelled.erase(id);
            }
        }
    }

private:
    struct Entry
    {
        std::chrono::nanoseconds due{};
        std::chrono::nanoseconds period{};
        std::uint64_t id = 0;
        TimeDomain domain = TimeDomain::Scaled;
        bool repeating = false;
        Callback cb;
    };

    struct EntryGreater
    {
        bool operator()(const Entry& a, const Entry& b) const
        {
            return a.due > b.due;
        }
    };

    TimerHandle enqueue(const TimeState& time, std::chrono::nanoseconds delay, std::chrono::nanoseconds period, bool repeating, Callback cb, TimeDomain domain)
    {
        const std::uint64_t id = m_nextId++;

        Entry e;
        e.id = id;
        e.domain = domain;
        e.repeating = repeating;
        e.period = period;
        e.cb = std::move(cb);

        const auto now = currentTimeFor(time, domain);
        e.due = now + std::max(std::chrono::nanoseconds(0), delay);

        m_queue.push(std::move(e));
        return TimerHandle{id};
    }

    bool isCancelled(std::uint64_t id)
    {
        if (id == 0) return true;
        const auto it = m_cancelled.find(id);
        if (it != m_cancelled.end()) {
            return true;
        }
        return false;
    }

    std::chrono::nanoseconds currentTimeFor(const TimeState& t, TimeDomain domain) const
    {
        return (domain == TimeDomain::Unscaled) ? t.unscaledTime : t.time;
    }

private:
    std::uint64_t m_nextId = 1;
    std::priority_queue<Entry, std::vector<Entry>, EntryGreater> m_queue;
    std::unordered_set<std::uint64_t> m_cancelled;
};
