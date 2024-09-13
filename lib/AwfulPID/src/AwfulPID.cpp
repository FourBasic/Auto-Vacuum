#include "AwfulPID.h"
#include <Arduino.h>
#include "Debounce.h"
#include "GeneralFunctions.h"

#define INIT        0x00
#define ENABLE      0x01
#define TIEBACK     0x02
#define MANUAL      0x03

AwfulPID::AwfulPID() {

}

void AwfulPID::setConfig(PIDConfiguration c) {
    cfg = c;
}

void AwfulPID::setParam(PIDParameters p) {
    param = p;
}

void AwfulPID::setManual(int val) {
    MV = val;
}

int AwfulPID::update(byte ctrl, int _PV, int _SP) {    
    if (ctrl == INIT) {
        // Clear and init
        CV = cfg.outMn;
        acc_ki = 0;      
        err_last = 0;

        // Prime timer to start on new cycle
        cycleTimer.update(cycleTimer.getState(),cfg.period_ms,cfg.period_ms);

    } else if (ctrl == ENABLE) {
        // Use timer to create a continous clock frequency while enabled
        cycleTimer.update(!cycleTimer.getState(),cfg.period_ms,cfg.period_ms);
        
        // Recalc on every timer transition
        if (cycleTimer.getTransitionFlag()) {
            cycleTimer.resetTransitionFlag();

            // Push request to current
            PV = _PV;
            SP = _SP;
            
            // Calculations
            int err = calculateError();
            if (err != 0) {
                int term_kp;
                int term_kd = 0;
                term_kp = (int) (param.kp * err); 
                if (param.ki > 0.00001) { acc_ki = acc_ki + (param.ki * err); }
                if (param.kd > 0.00001) { term_kd = param.kd * (err - err_last); }
                CV = term_kp + acc_ki + term_kd;
            }        
        }    
    } else if (ctrl == TIEBACK) {

    } else if (ctrl == MANUAL) {
        CV = MV;
    }    
    CV = limit(cfg.outMn, CV, cfg.outMx);
    return CV;
}

int AwfulPID::calculateError() {
    int e;
    if (cfg.reverseActing) { e = PV - SP; }
    else { e = SP - PV; }
    return e;
}

int AwfulPID::getError() {
    return err_last;
}

int AwfulPID::getCV() {
    return CV;
}