#include <SPI.h>
#include <RF24.h>

#define CE 9
#define CS 10
#define RED_LED 2
#define GREEN_LED 3
#define YELLOW_LED 4

RF24 radio(CE, CS);

int roundCount = 1;

void setup() {
  radio.begin();
  radio.setChannel(2); 
  radio.setPALevel(RF24_PA_MIN);
  
  radio.stopListening();
  
  uint8_t readAddress[] = { 0xC2, 0xC2, 0xC2, 0xC2, 0xC2 };
  radio.openReadingPipe(1, readAddress);

  uint8_t writeAddress[] = { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 };
  radio.openWritingPipe(writeAddress);
  
  radio.setCRCLength(RF24_CRC_16);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);

  Serial.begin(9600);
}

void blink(int pin) {
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
}

void light(int type) {
  switch(type) {
    // RED
    case 1:      blink(RED_LED);   break;
    // GREEN
    case 2:      blink(GREEN_LED);  break;
    // YELLOW
    case 3:      blink(YELLOW_LED);     break;
  }
}

//typedef struct Sequence{
//  // send a dynamically allocated list
//  int *sequence;
//  int a = 0;
//};

int TERMINATOR = 1;
char buf = 'a';

//void loop() {
//  Serial.println("sent a");
//
//  radio.write((void*)&buf, 1, false);
//
//  delay(1000);
//
//}

int *seqToSend = (int*) malloc((roundCount+TERMINATOR)*sizeof(int));

void (int *intarray) {
  // get size of array
  int sizeOfSequence = sizeof(intarray)/sizeof(int);

  // allocate more space for sequence
  if (roundCount == sizeOfSequence-TERMINATOR) {
    // point to sequence, store temporarily
    int *history = intarray;

    // clean memory
    free(intarray);
    
    // allocate twice the current size
    intarray = (int*)malloc((roundCount+TERMINATOR)*2*sizeof(int));

    // copy old data to new array
    int i;
    for (i = 0; history[i] != 4; i++) {
      intarray[i] = history[i];
    }
    intarray[i] = 4;
  }
}

void loop() {
  Serial.println("Round ");

  // generate random number from 0-2
  int randNum = random(1, 4);

  seqToSend[roundCount-TERMINATOR] = randNum;
  seqToSend[roundCount] = 4;

  for (int i = 0; i < sizeof(seqToSend)/sizeof(int); i++) {
    light(seqToSend[i]);
  }

  expand_array(seqToSend);

  // send data to transmitter
  radio.write((void*) &randNum, sizeof(int), false);
  
  radio.startListening();
//  
//
////  while(true) {
////    
////    Serial.println("Sending..");
////    Serial.print(sizeof(seqToSend));
////    seqToSend[0] = random(0, 3);
////    seqToSend[1] = 4;
////    radio.write((void*) &buf, sizeof(buf), 1);
////    radio.write((void*) seqToSend, sizeof(seqToSend), false);
////    for (int i = 0; i < sizeof(seqToSend); i++)
////      Serial.println("Sent " + String(seqToSend[0]) + String(seqToSend[1])); 
////    delay(1000);
////    radio.stopListening();
////  }
//
//
// 
    // true if transmitter pressed right button
    int roundSucceeded = 0;   
//
    // 0 until succeed
    while (true) {
//        Serial.println("polling");
        // get response from transmitter
        radio.read((void *)&roundSucceeded, sizeof(int));

        if (roundSucceeded) {
          Serial.println("ROUND SUCCEEDED");
          roundCount++;
          break;
        }
    }

//    // succeeded
//    if (roundSucceeded == 1) {
//      Serial.println("round " + String(roundCount) + " succeeded");
//      light(2);
//      roundCount++;
//    } else if (roundSucceeded == 2) {
//      Serial.println("round " + String(roundCount) + " not succeeded");
//      light(2);
//      free(seqToSend);
//      Serial.println("program ended");
//    }
//  
   radio.stopListening();

  // free when exiting
}
