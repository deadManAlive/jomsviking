#include "SIMDMath.h"

namespace SIMDMath{
    inline float sum(const float* source, int num){
        if(juce::SystemStats::hasSSE3()){
            __m128 mmSum = _mm_setzero_ps();

            int i = 0;
            int rd = num - (num % 16);

            for(; i < rd; i += 16){
                __m128 v0 = _mm_loadu_ps(source + i + 0);
                __m128 v1 = _mm_loadu_ps(source + i + 4);
                __m128 s01 = _mm_add_ps(v0, v1);

                __m128 v2 = _mm_loadu_ps(source + i + 8);
                __m128 v3 = _mm_loadu_ps(source + i + 12);
                __m128 s23 = _mm_add_ps(v2, v3);

                mmSum = _mm_add_ps(mmSum, s01);
                mmSum = _mm_add_ps(mmSum, s23);
            }

            for(; i < num; i++){
                mmSum = _mm_add_ss(mmSum, mmSum);
            }

            mmSum = _mm_hadd_ps(mmSum, mmSum);
            mmSum = _mm_hadd_ps(mmSum, mmSum);
            return _mm_cvtss_f32(mmSum);
        }

        float s = 0.0f;
        const float* e = source + num;

        while(source != e){
            s += *source;
            source++;
        }

        return s;
    }
}