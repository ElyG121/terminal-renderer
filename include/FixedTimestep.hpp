#pragma once
#include <algorithm>

// Turns variable frame times into a whole number of equal physics steps.
// Leftover time carries over to the next frame, so no time is lost.
class FixedTimestep
{
public:
    // max_frame_seconds caps a single frame so a long stall can't demand hundreds of
    // catch-up steps at once (the "spiral of death")
    explicit FixedTimestep(float step_seconds, float max_frame_seconds = 0.25f)
        : dt(step_seconds), max_frame(max_frame_seconds)
    {
    }

    // Adds frame_seconds of elapsed time and calls step(dt) once per full dt.
    // Returns how many steps ran.
    template <class StepFn>
    int advance(float frame_seconds, StepFn &&step)
    {
        // A non-positive step would never drain the accumulator
        if (dt <= 0.0f)
            return 0;

        frame_seconds = std::min(frame_seconds, max_frame);
        accumulator += frame_seconds;
        int count = 0;
        while (accumulator >= dt)
        {
            step(dt);
            accumulator -= dt;
            count++;
        }
        return count;
    }

private:
    float dt;
    float max_frame;
    float accumulator = 0.0f;
};
