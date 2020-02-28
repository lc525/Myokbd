/* ---------
 * Copyright 2019 Lucian Carata <lucian.carata@cl.cam.ac.uk>
 *
 * This file is part of the Myokbd open-source project: github.com/lc525/myokbd
 * Licensed under the terms of Apache license 2.0, see the LICENSE file at the
 * root of the project for details.
 * ---------
 *
 * Robust peak detection algorithm on analog data reads.
 *
 * An adaptation for incremental data processing of an algorithm by Jean-Paul,
 * https://stackoverflow.com/questions/22583391/peak-signal-detection-in-realtime-timeseries-data
 *
 */


#ifndef _PEAKDETECTION_H_
#define _PEAKDETECTION_H_

#include <stdint.h>
#include <LowPowerTimer.h>

namespace ldry { namespace signal {
  enum class PeakSignal {
    MORE_DATA_NEEDED,
    POS_PEAK,
    NEG_PEAK,
    PEAK,           // POS_PEAK | NEG_PEAK
    NO_PEAK=1<<3,   // 8_(10)=1000_(2)
    FLIP_MASK=11    // 1011_(2)
  };

  template <uint16_t LOG_2LAG=7>
  class PeakDetection {
    public:
     PeakDetection(float threshold=3, float infl=0):
      _lag(1<<LOG_2LAG),
      _threshold(threshold),
      _influence(infl),
      _n(0), _K(0),
      _Ex(0.0), _Ex2(0.0),
      _avgFilter(0.0),
      _stable_sig(PeakSignal::NO_PEAK),
      _stable_count(1000),
      _unstable_count(0),
      _bufFilled(false),
      _lagData_cBuf {},
      _stdFilter(0.0) {
     }

     PeakSignal addDataGetPeak(uint16_t data){
       uint16_t rmVal = 0;
       PeakSignal ret;

       // incremental computation of avg and variance
       if(_n == 0 && !_bufFilled) _K = data;
       if(_bufFilled){ // save oldest value from the avg/std dev calculations
         rmVal = _lagData_cBuf[_n % _lag];
       } else { // buffer not filled yet so we are just adding values

        _lagData_cBuf[_n % _lag] = data;

        // add new value to the avg/std dev calculations
        _Ex += data - _K;
        _Ex2 += (data - _K) * (data - _K);

        _n++;
       }

       if(_n < _lag && !_bufFilled) return PeakSignal::MORE_DATA_NEEDED;
       if(_n == _lag && !_bufFilled) { // we now compute statistics for a window of size _lag
         _bufFilled = true;
         _avgFilter = _K + _Ex / _lag;
         _stdFilter = sqrt( (_Ex2 - (_Ex * _Ex) / _lag) / (_lag - 1) );
         /* Serial.print("avg:");
          * Serial.println(_avgFilter);
          * Serial.print("thr:");
          * Serial.println(_threshold * _stdFilter); */
         return PeakSignal::MORE_DATA_NEEDED;
       }
       else {
         if( abs(data - _avgFilter) > _threshold * _stdFilter) { // PEAK
           if(((int)_stable_sig & (int)PeakSignal::PEAK) != 0) {
             _stable_count++;  // was previously a peak, maintained
             _unstable_count = 0;
           } else {
             _unstable_count++;// stable is NO_PEAK but we got a peak
           }
           /* if( data > _avgFilter ){
            *   ret = PeakSignal::POS_PEAK;
            * } else {
            *   ret = PeakSignal::NEG_PEAK;
            * } */
           _lagData_cBuf[_n % _lag] =
             _influence * data + (1 - _influence) * _lagData_cBuf[(_n) % _lag];
         }
         else { // NO_PEAK
           if(_stable_sig == PeakSignal::NO_PEAK) {
             _stable_count++;  // was previously a NO_PEAK, maintained
             _unstable_count = 0;
           } else {
             _unstable_count++;// stable is PEAK but we got a NO_PEAK
           }
           ret = PeakSignal::NO_PEAK;
           _lagData_cBuf[_n % _lag] = data;
         }

         // update statistics
         _Ex -= rmVal - _K;
         _Ex2 -= (rmVal - _K) * (rmVal - _K);
         _Ex += _lagData_cBuf[_n % _lag]  - _K;
         _Ex2 += (_lagData_cBuf[_n % _lag] - _K) * (_lagData_cBuf[_n % _lag] - _K);
         _avgFilter = _K + _Ex / _lag;
         _stdFilter = sqrt( (_Ex2 - (_Ex * _Ex) / _lag) / (_lag - 1) );
         /* Serial.println("sig:");
          * Serial.println(data-_avgFilter);
          * Serial.println("thr:");
          * Serial.println(_threshold * _stdFilter);  */
         _n++;
         if(_unstable_count > 5){ // flip stable/unstable
           if(_stable_sig==PeakSignal::NO_PEAK) _stable_sig=PeakSignal::PEAK;
           else _stable_sig = PeakSignal::NO_PEAK;
           _stable_count = _unstable_count;
           _unstable_count = 0;
           //Serial.println("flip!");
         }
         return _stable_sig;
       }
     }

     void setThreshold(float newthreshold){
       _threshold = newthreshold;
     }

    private:
      uint16_t _lag;          // lag of moving window (in number of samples)
      float _threshold;       // number of standard deviations from the moving
                              // mean above which we classify a new datapoint as
                              // a "peak"
      float _influence;       // number in interval [0, 1] controlling the
                              // influence of detected peaks on means/std dev
                              // 0 means that future peaks are determined based
                              // on a threshold not influenced by past signals
      double _avgFilter;
      double _stdFilter;
      uint16_t _n;
      uint16_t _K;
      uint16_t _lagData_cBuf[1<<LOG_2LAG];
      /* mbed::LowPowerTimer *_timer; */
      PeakSignal _stable_sig;
      uint32_t _stable_count;
      uint32_t _unstable_count;
      /* bool _extTimer; */
      bool _bufFilled;
      double _Ex, _Ex2;
  };
} }

#endif /*_PEAKDETECTION_H_*/
