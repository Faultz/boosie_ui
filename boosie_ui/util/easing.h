#pragma once

namespace easing
{
    inline float get_delta_min(float scale)
    {
        return (1.f / 0.2f) * (1.f / 60.f) * scale;
    }

    template<typename T>
    inline float lerp(T start, T end, float t)
    {
        return start + ((end - start) * t);
    }

    inline float lerp(float start, float end, float t, float(*ease)(float)) {
        /* Calculate the interpolated value */
        float value = start + (end - start) * t;

        /* Apply the easing factor */
        value = start + (value - start) * ease(t);

        return value;
    }

    inline float linear(const float t)
    {
        return t;
    }
    inline float in_sine(const float t)
    {
        return sinf(1.5707963 * t);
    }
    inline float out_sine(float t)
    {
        return 1 + sinf(1.5707963 * (--t));
    }
    inline float in_out_sine(const float t)
    {
        return 0.5 * (1 + sinf(3.1415926 * (t - 0.5)));
    }
    inline float inquad(const float t)
    {
        return t * t;
    }
    inline float out_quad(const float t)
    {
        return t * (2 - t);
    }
    inline float in_out_quad(const float t)
    {
        return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
    }
    inline float in_cubic(const float t)
    {
        return t * t * t;
    }
    inline float out_cubic(float t)
    {
        return 1 + (--t) * t * t;
    }
    inline float in_out_cubic(float t)
    {
        return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
    }
    inline float in_quart(float t)
    {
        t *= t;
        return t * t;
    }
    inline float out_quart(float t)
    {
        t = (--t) * t;
        return 1 - t * t;
    }
    inline float in_out_quart(float t)
    {
        if (t < 0.5)
        {
            t *= t;
            return 8 * t * t;
        }
        else
        {
            t = (--t) * t;
            return 1 - 8 * t * t;
        }
    }
    inline float in_quint(const float t)
    {
        const float t2 = t * t;
        return t * t2 * t2;
    }
    inline float out_quint(float t)
    {
        const float t2 = (--t) * t;
        return 1 + t * t2 * t2;
    }
    inline float in_out_quint(float t)
    {
        float t2;
        if (t < 0.5)
        {
            t2 = t * t;
            return 16 * t * t2 * t2;
        }
        else
        {
            t2 = (--t) * t;
            return 1 + 16 * t * t2 * t2;
        }
    }
    inline float in_expo(const float t)
    {
        return (powf(2, 8 * t) - 1) / 255;
    }
    inline float out_expo(const float t)
    {
        return 1 - powf(2, -8 * t);
    }
    inline float in_out_expo(const float t)
    {
        if (t < 0.5)
        {
            return (powf(2, 16 * t) - 1) / 510;
        }
        else
        {
            return 1 - 0.5 * powf(2, -16 * (t - 0.5));
        }
    }
    inline float in_circ(const float t)
    {
        return 1 - sqrtf(1 - t);
    }
    inline float out_circ(const float t)
    {
        return sqrtf(t);
    }
    inline float in_out_circ(const float t)
    {
        if (t < 0.5)
        {
            return (1 - sqrtf(1 - 2 * t)) * 0.5;
        }
        else
        {
            return (1 + sqrtf(2 * t - 1)) * 0.5;
        }
    }
    inline float in_back(const float t)
    {
        return t * t * (2.70158 * t - 1.70158);
    }
    inline float out_back(float t)
    {
        return 1 + (--t) * t * (2.70158 * t + 1.70158);
    }
    inline  float in_out_back(float t)
    {
        if (t < 0.5)
        {
            return t * t * (7 * t - 2.5) * 2;
        }
        else
        {
            return 1 + (--t) * t * 2 * (7 * t + 2.5);
        }
    }
    inline float in_elastic(const float t)
    {
        const float t2 = t * t;
        return t2 * t2 * sinf(t * M_PI * 4.5);
    }
    inline float out_elastic(const float t)
    {
        const float t2 = (t - 1) * (t - 1);
        return 1 - t2 * t2 * cosf(t * M_PI * 4.5);
    }
    inline float in_out_elastic(const float t)
    {
        float t2;
        if (t < 0.45)
        {
            t2 = t * t;
            return 8 * t2 * t2 * sinf(t * M_PI * 9);
        }
        else if (t < 0.55)
        {
            return 0.5 + 0.75 * sinf(t * M_PI * 4);
        }
        else
        {
            t2 = (t - 1) * (t - 1);
            return 1 - 8 * t2 * t2 * sinf(t * M_PI * 9);
        }
    }
    inline float in_bounce(const float t)
    {
        return powf(2, 6 * (t - 1)) * fabsf(sinf(t * M_PI * 3.5));
    }
    inline float out_bounce(const float t)
    {
        return 1 - powf(2, -6 * t) * fabsf(cosf(t * M_PI * 3.5));
    }
    inline float in_out_bounce(const float t)
    {
        if (t < 0.5)
        {
            return 8 * powf(2, 8 * (t - 1)) * fabsf(sinf(t * M_PI * 7));
        }
        else
        {
            return 1 - 8 * powf(2, -8 * t) * fabsf(sinf(t * M_PI * 7));
        }
    }
}