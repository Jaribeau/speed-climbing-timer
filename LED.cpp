class LED{

    public:

    LED(int pin){
    }

    void on(){

    }

    void off(){

    }

    void pulse(){

    }

    void blink(int blinks, bool stayOn){

    }

    private:
    //Constants
    const int ON      = 0;
    const int OFF     = 1;
    const int BLINK   = 2;
    const int PULSE   = 3;
    int brightness = 255;
    int pulseBrightness = 0;

    // Update async animations
    void update(){
        long now = millis();
        switch (activeAnimation){
            //
            // BLINK
            //
            case BLINK:
                if(now < nextBlinkTime){ 
                    // Do nothng
                    break;
                }
                else{
                    if(blinksLeft % 2) /* if odd */
                            analogWrite(pin, brightness);
                        else
                            analogWrite(pin, 0);
                        
                    nextBlinkTime += blinkInterval;

                    blinksLeft--;
                    if(blinksLeft == 0){ 
                        if(stayOn){
                            activeAnimation = ON;
                            analogWrite(pin, brightness);
                        }
                        else{
                            activeAnimation = OFF;
                            analogWrite(pin, 0);
                        }
                    }
                }
                break; 

            //
            // PULSE
            //
            case PULSE:
                // Brightness proportional to distance from pulse end
                if(now >= nextPulseTime){ 
                    pulseDirection *= -1;
                    nextPulseTime += pulseInterval;
                    break;
                }
                if(pulseDirection)
                    this->pulseBrightness = brightness - (255 * (now - nextPulseTime) / pulseInterval);
                else
                    this->pulseBrightness =     0      + (255 * (now - nextPulseTime) / pulseInterval);
                
                analogWrite(pin, this->pulseBrightness);
                break;   
        }
    }
}