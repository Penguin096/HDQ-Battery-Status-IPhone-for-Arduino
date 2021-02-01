/*
 * iPhone Battery HDQ Status
 * 
 * Copyright (c) 2021 Penguin096
 * 
 * Requires the Arduino HDQ library from
 * https://github.com/mozzwald/HDQLib
 */

#include <HDQ.h>

HDQ HDQ(HDQ_DEFAULT_PIN); // Arduino digital pin 7
// USAGE!
// The only other thing needed is to have a string pull up (4.7K did the job, I saw 10K used in the app notes).

void setup() {
  Serial.begin(9600);
}

void loop() {
  uint8_t BYT1, BYT2;
  int dev, firm, hard, major, minor;
  
/* Standard */
/* Commands */  

  /* Getdevicetype */
  BYT1 = HDQ.write(0x0, 0x01);
  BYT2 = HDQ.write(0x1, 0x00);
  BYT1 = HDQ.read(0x00);
  BYT2 = HDQ.read(0x01);
  dev = (BYT2*100)+BYT1;

/* 
   Devices (dev var):
      565 = bq27541 (3G, 3GS, 4G, 4GS)
      569 = bq27545 (5G, 5C, 5S, 6G, 6GP)
      570 = bq27546 (6S, 6SP, 7G)
      
   Firmware (firm var):
      123 = v1.17 (3GS)
      137 = v1.25 (4G)
      153 = v1.35 (4GS)
      316 = v3.10 (5G, 5C, 5S)
      502 = v5.02 (6G, 6GP)
      601 = v6.01 (6S, 6SP, 7G)

   Hardware (hard var):
      ??? = ???
*/
     
  if(BYT1 != 255 && BYT2 != 255){ // Check if HDQ capable battery is connected
    
    /* GetFirmwareVersion */
    BYT1 = HDQ.write(0x0, 0x02);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x00);
    BYT2 = HDQ.read(0x01);
    firm = (BYT2*100)+BYT1;
  
    /* GetHardwareVersion */
    BYT1 = HDQ.write(0x0, 0x03);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x00);
    BYT2 = HDQ.read(0x01);
    hard = (BYT2*100)+BYT1;

    if(dev == 565){ // bq27541 (3G, 3GS, 4G, 4GS)
      if(firm == 123){
        Serial.println("iPhone 3GS Battery Detected");
      }else if(firm == 137){
        Serial.println("iPhone 4G Battery Detected");
      }else if(firm == 153){
        Serial.println("iPhone 4GS Battery Detected");
      }else{
        Serial.println("Unknown HDQ Battery Detected");
      }
    }else if(dev == 569){ // bq27545 (5G, 5C, 5S, 6G, 6GP)
      if(hard == 167){
        Serial.println("iPhone 5S / 5C Battery Detected");
      }else{
        Serial.println("Unknown HDQ Battery Detected");
      }
    }else if(dev == 570){ // bq27546 (6S, 6SP, 7G)
      if(hard == 168){
        Serial.println("iPhone 7 Battery Detected");
      }else{
        Serial.println("Unknown HDQ Battery Detected");
      }
    }else if(dev == 167){ // Fake?
      Serial.println("Unknown (Fake iPhone 5S/5C) HDQ Battery Detected");
    }else{
      Serial.println("Unknown HDQ Battery Detected");
    }

    /* DisplayDevice */
    Serial.print("Device: ");
    switch(dev) {
      case 565:
        Serial.println("bq27541");
        break;
      case 569:
        Serial.println("bq27545");
        break;
      case 570:
        Serial.println("bq27546");
        break;
      default:
        Serial.print("Unknown (");
        Serial.print(dev);
        Serial.println(")");
        break;
    }

    /* DisplayFirmwareVersion */
    major = firm/100;
    minor = firm-(major*100);
    Serial.print("Firmware: v");
    Serial.print(major, HEX);
    Serial.print(".");
    Serial.println(minor, HEX);

    /* DisplayHardwareVersion */
    Serial.print("Hardware: 0x");
    Serial.println(hard, HEX);

    /* AtRate mA*/
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x02);
    BYT2 = HDQ.read(0x03);
    if(word(BYT2, BYT1) != -1){
      Serial.print("At Rate: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mA");
    }
    
    /* UnfilteredSOC % */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x04);
    BYT2 = HDQ.read(0x05);
    if(word(BYT2, BYT1) != -1 && word(BYT2, BYT1) >= 0 && word(BYT2, BYT1) <= 100){
      Serial.print("Unfiltered SOC: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" %");
    }

    /* Temperature(in Kelvin to C/F) */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x06);
    BYT2 = HDQ.read(0x07);
    float tempC = ((word(BYT2, BYT1))-2731)/10;
    if(word(BYT2, BYT1) != -1){
      Serial.print("Temperature: ");
      Serial.print(tempC);
      Serial.print(char(176));
      Serial.println("C");
    }

    /* Voltage mV */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(8);
    BYT2 = HDQ.read(9);
    int mvolts = word(BYT2, BYT1);
    float volts = (mvolts*0.001);
    if(mvolts != -1){
      Serial.print("Battery Voltage: ");
      Serial.print(volts);
      Serial.print("V (");
      Serial.print(mvolts);
      Serial.println("mV)");
    }

    /* Flags */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x0A);
    BYT2 = HDQ.read(0x0B);
    Serial.println("Flags: ");
    if(bitRead(BYT2, 7) == 1){
      Serial.println("  Over-Temperature in Charge condition is detected"); //OTC
    }
    if(bitRead(BYT2, 6) == 1){
      Serial.println("  Over-Temperature in Discharge condition is detected"); //OTD
    }
    if(bitRead(BYT2, 5) == 1){
      Serial.println("  High battery voltage condition"); //BATHI
    }
    if(bitRead(BYT2, 4) == 1){
      Serial.println("  Low battery voltage condition"); //BATLOW
    }
    if(bitRead(BYT2, 3) == 1){
      Serial.println("  Charge Inhibit Temp Low / Charge Inhibit Temp High"); //CHG_INH
    }
    /*if(bitRead(BYT2, 2) == 1){
      Serial.println("  Reserved"); //RSVD
    }*/
    if(bitRead(BYT2, 1) == 1){
      Serial.println("  Full-charged is detected"); //FC
    }
    if(bitRead(BYT2, 0) == 1){
      Serial.println("  (Fast) charging allowed"); //CHG
    }
    if(bitRead(BYT1, 7) == 1){
      Serial.println("  OCV measurement is performed in RELAX"); //OCVTAKEN
    }
    if(bitRead(BYT1, 6) == 1){
      Serial.println("  Internal Short is detected"); //ISD
    }
    if(bitRead(BYT1, 5) == 1){
      Serial.println("  Tab Disconnect is detected"); //TDD
    }
    if(bitRead(BYT1, 4) == 1 && bitRead(BYT1, 3 == 0)){
      Serial.println("  Device Identification: OK"); //HW[1:0]
    }
    if(bitRead(BYT1, 2) == 1){
      Serial.println("  State-of-Charge-Threshold 1 (SOC1 Set) reached"); //SOC1
    }
    if(bitRead(BYT1, 1) == 1){
      Serial.println("  State-of-Charge-Threshold Final (SOCF Set %) reached"); //SOCF
    }
    if(bitRead(BYT1, 0) == 1){
      Serial.println("  Discharging detected"); //DSG
    }
    
    /* NomAvailableCapacity mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x0C);
    BYT2 = HDQ.read(0x0D);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Nom Available Capacity: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }

    /* FullAvailableCapacity mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x0E);
    BYT2 = HDQ.read(0x0F);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Full Available Capacity: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }

    /* RemainingCapacity mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x10);
    BYT2 = HDQ.read(0x11);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Remaining Capacity: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }
    
    /* Fullchargecapacity mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x12);
    BYT2 = HDQ.read(0x13);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Full Charge Capacity: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }

    /* AverageCurrent mA */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x14);
    BYT2 = HDQ.read(0x15);
    int mamps = word(BYT2, BYT1);
    float amp = (mamps*0.001);
    if(mamps != -1){
      Serial.print("Average Current: ");
      Serial.print(amp);
      Serial.print("A (");
      Serial.print(mamps);
      Serial.println("mA)");
    }

    /* TimetoEmptyMinutes */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x16);
    BYT2 = HDQ.read(0x17);
    Serial.print("Time to empty: ");
    if(word(BYT2, BYT1) == -1){
      Serial.println("battery is not being discharged");
    }else{
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" minutes");
    }
    
    /* FullChargeCapacityFiltered mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x18);
    BYT2 = HDQ.read(0x19);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Full Charge Capacity Filtered: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }

    /* StandbyCurrent/SafetyStatus */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x1A);
    BYT2 = HDQ.read(0x1B);
    if(word(BYT2, BYT1) != -1){
    if(dev == 569 || dev == 565){
      Serial.print("Standby Current: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mA");
    }else if(dev == 570){
      Serial.print("Safety Status: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println("");
    }
    }

    /* FullChargeCapacityUnfiltered mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x1C);
    BYT2 = HDQ.read(0x1D);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Full Charge Capacity Unfiltered: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }

    /* Imax mA */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x1E);
    BYT2 = HDQ.read(0x1F);
    mamps = word(BYT2, BYT1);
    amp = (mamps*0.001);
    if(mamps != -1){
      Serial.print("I max: ");
      Serial.print(amp);
      Serial.print("A (");
      Serial.print(mamps);
      Serial.println("mA)");
    }

    /* RemainingCapacityUnfiltered mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x20);
    BYT2 = HDQ.read(0x21);
    if(mamps != -1){
      Serial.print("Remaining Capacity Unfiltered: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }

    /* RemainingCapacityFiltered mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x22);
    BYT2 = HDQ.read(0x23);
    if(mamps != -1){
      Serial.print("Remaining Capacity Filtered: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }

    /* AveragePower mW/cW/BTPSOC1Set mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x24);
    BYT2 = HDQ.read(0x25);
    if(word(BYT2, BYT1) != -1){
    if(dev == 565){
      Serial.print("Average Power: ");
      Serial.print((word(BYT2, BYT1), DEC)*10);
      Serial.println(" mW");
    }else if(dev == 569){
      Serial.print("Average Power: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mW/cW");
    }else if(dev == 570){
      Serial.print("BTPSOC1 Set: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }
    }

    /* TTEatConstantPower Minutes/BTPSOC1Clear mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x26);
    BYT2 = HDQ.read(0x27);
    if(word(BYT2, BYT1) != -1){
    if(dev == 565){
      Serial.print("TTEatConstantPower: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" minutes");
    }else if(dev == 570){
      Serial.print("BTPSOC1 Clear: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }
    }

    /* InternalTemperature (in Kelvin to C/F) */
    if(dev == 569 || dev == 570){
      BYT1 = HDQ.write(0x0, 0x01);
      BYT2 = HDQ.write(0x1, 0x00);
      BYT1 = HDQ.read(0x28);
      BYT2 = HDQ.read(0x29);
      if(word(BYT2, BYT1) != -127){
        tempC = ((word(BYT2, BYT1))-2731)/10;
        Serial.print("Internal Temperature: ");
        Serial.print(tempC);
        Serial.print(char(176));
        Serial.println("C");
    }
    }

    /* ChargeCycleCount */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x2A);
    BYT2 = HDQ.read(0x2B);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Charge Cycle Count: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" times");
    }

    /* StateofCharge % */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x2C);
    BYT2 = HDQ.read(0x2D);
    if(word(BYT2, BYT1) != -1 && word(BYT2, BYT1) >= 0 && word(BYT2, BYT1) <= 100){
      Serial.print("State of Charge: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(char(37));
    }

    /* StateofHealth % / num  */
    if(dev == 569 || dev == 570){
      BYT1 = HDQ.write(0x0, 0x01);
      BYT2 = HDQ.write(0x1, 0x00);
      BYT1 = HDQ.read(0x2E);
      BYT2 = HDQ.read(0x2F);
      if(word(BYT2, BYT1) != -1 && word(BYT2, BYT1) >= 0 && word(BYT2, BYT1) <= 100){
        Serial.print("State of Health: ");
        Serial.print(word(BYT2, BYT1), DEC);
        Serial.println(" %/num");
    }
    

    /* ChargingVoltage mV */
    if(dev == 570){
      BYT1 = HDQ.write(0x0, 0x01);
      BYT2 = HDQ.write(0x1, 0x00);
      BYT1 = HDQ.read(0x30);
      BYT2 = HDQ.read(0x31);
      mvolts = word(BYT2, BYT1);
      if(mvolts != -1){
        volts = (mvolts*0.001);
        Serial.print("Charging Voltage: ");
        Serial.print(volts);
        Serial.print("V (");
        Serial.print(mvolts);
        Serial.println("mV)");
      }

    /* ChargingCurrent mA */
      BYT1 = HDQ.write(0x0, 0x01);
      BYT2 = HDQ.write(0x1, 0x00);
      BYT1 = HDQ.read(0x32);
      BYT2 = HDQ.read(0x33);
      mamps = word(BYT2, BYT1);
      if(mamps != -1){
        amp = (mamps*0.001);
        Serial.print("Charging Current: ");
        Serial.print(amp);
        Serial.print("A (");
        Serial.print(mamps);
        Serial.println("mA)");
      }
    }

    /* PassedCharge mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x34);
    BYT2 = HDQ.read(0x35);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Passed Charge: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");   
    } 

    /* DOD0 hex */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x36);
    BYT2 = HDQ.read(0x37);
    if(word(BYT2, BYT1) != -1){
      Serial.print("DOD0: 0x");
      Serial.println(word(BYT2, BYT1), HEX);
    }
        
    /* SelfDischargeCurrent mA */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x38);
    BYT2 = HDQ.read(0x39);
    mamps = word(BYT2, BYT1);
    if(mamps != -1){
      Serial.print("Self Discharge Current: ");
      Serial.print(mamps);
      Serial.println(" mAh");
    }
    
    

/* Extended */
/* Commands */

    /* PackConfig hex */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x3A);
    BYT2 = HDQ.read(0x3B);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Pack Config: 0x");
      Serial.println(word(BYT2, BYT1), HEX);
    }
    }
    
    /* Designcapacity mAh */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x3c);
    BYT2 = HDQ.read(0x3d);
    if(word(BYT2, BYT1) != -1){
      Serial.print("Design Capacity: ");
      Serial.print(word(BYT2, BYT1), DEC);
      Serial.println(" mAh");
    }

    /* DeviceName */
    BYT1 = HDQ.write(0x0, 0x01);
    BYT2 = HDQ.write(0x1, 0x00);
    BYT1 = HDQ.read(0x62);
    if(BYT1 != 0xFF){
      Serial.print("DeviceName: ");
      for (int i=0; i <= BYT1; i++){
        BYT2 = HDQ.read(0x40+i);
        Serial.print((char)BYT2);  
      }
    }
  
   
  }else{
    // HDQ Battery isn't connected
    Serial.println("Battery not detected");
  }
  
  delay(5000);  
  Serial.println("");
}
