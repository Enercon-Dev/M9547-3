#include "Thermistor.h"

constexpr uint16_t Thermistor::MeasIn[];
constexpr int16_t Thermistor::MeasOut[];

int16_t Thermistor::Format(uint16_t raw){
        int pStart=0;
        int pStop = 150;
        int mid;
        while (pStop - pStart > 1)
        {
          mid = (pStop + pStart)/2;
          if (MeasIn[mid] > raw)
            pStop = mid;
          else
            pStart = mid;
        }
        if (pStop == pStart)
          return MeasOut[pStop];
        return (int16_t)((MeasOut[pStop] - MeasOut[pStart])/(double)((MeasIn[pStop] - MeasIn[pStart])) * (raw - MeasIn[pStart]) + MeasOut[pStart]);
}
