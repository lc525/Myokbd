#ifndef _PRESENTATION_CONTROLLER_H_
#define _PRESENTATION_CONTROLLER_H_

#include <mbed.h>
#include "LowPowerTimer.h"

#include "PresentationRemote.h"
#include "PeakDetection.h"

namespace myokbd {
  class PresentationController {
    public:
     PresentationController(PresentationRemote* pr,
                            PinName data_src_pin,
                            uint8_t evt_squeue_size = 20,
                            uint16_t next_cmd_time = 550,
                            uint16_t prev_min_cmd_time = 125) :
       _presenter(pr),
       _sensor_queue(evt_squeue_size * EVENTS_EVENT_SIZE),
       _dproc(),
       _data_src(data_src_pin),
       _sensor_data(0),
       _threshold(32667),
       _next_cmd_time(next_cmd_time),
       _prev_min_cmd_time(prev_min_cmd_time),
       _last_signal_time(0),
       _last_nosignal_time(0)
    {
      setupDataProcessing();
      _sensor_queue.call_every(25, this, &PresentationController::sensorLoop);
      _sensor_queue.dispatch_forever();
    }

    ~PresentationController() {
      _timer.stop();
    }

    private:
     void setupDataProcessing() {
       _timer.start();
       delay(1000);
     }

     void sensorLoop(void) {
       using namespace ldry::signal;

       _sensor_data = _data_src.read_u16();
       PeakSignal sig = _dproc.addDataGetPeak(_sensor_data);
       if(sig == PeakSignal::MORE_DATA_NEEDED){
         return;
       }
       if(sig == PeakSignal::PEAK) {
         if(_last_signal == PeakSignal::NO_PEAK){
            _last_signal_time = _timer.read_ms();
         }
         _last_signal = PeakSignal::POS_PEAK;
       }
       if(sig == PeakSignal::NO_PEAK) {
         _last_nosignal_time = _timer.read_ms();
         if (_last_signal_time == 0) {
           _last_signal_time = _last_nosignal_time;
         }
         int delta = _last_nosignal_time - _last_signal_time;
         //Serial.println(delta);
         if ( delta >= _next_cmd_time)
           _presenter->nextSlide();
         else if( delta >= _prev_min_cmd_time)
           _presenter->previousSlide();
         _last_signal_time = 0;
         _last_signal=PeakSignal::NO_PEAK;
       }
     }

    private:
      events::EventQueue _sensor_queue;
      ldry::signal::PeakDetection<> _dproc;
      ldry::signal::PeakSignal _last_signal;
      mbed::AnalogIn _data_src;
      uint16_t _sensor_data;
      uint16_t _threshold;
      uint16_t _window_ms;
      PresentationRemote* _presenter;
      mbed::LowPowerTimer _timer;
      int _cmd_time;
      int _last_signal_time;
      int _last_nosignal_time;
      int _next_cmd_time;
      int _prev_min_cmd_time;
      bool _cmd_is_active;
  };

}

#endif
