#include <SPI.h>
#include <RF24.h>

#define CE 9
#define CS 10
#define RED_LED 2
#define GREEN_LED 5
#define YELLOW_LED 4

RF24 radio(CE, CS);

int rounds = 1;

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

  randomSeed(analogRead(6));
  Serial.begin(9600);
}

void blink(int pin) {
  delay(500);
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

//int TERMINATOR = 1;
char buf = 'a';
int arraysize = 1;

int *seqToSend = (int*) malloc((rounds)*sizeof(int));

//void expand_array (int *intarray) {
//  // allocate twice the current size
//  arraysize = rounds*2;
//  intarray = (int*)realloc(seqToSend, arraysize*sizeof(int));
//}

void loop() {
  seqToSend = (int*)realloc(seqToSend, rounds*sizeof(int));
  Serial.println("Round ");

  // generate random number from 0-2
  int randNum = random(1, 4);

  seqToSend[rounds-1] = randNum;

  for (int i = 0; i < rounds; i++) {
    light(seqToSend[i]);
  }

  // allocate more space for sequence
//  if (rounds == arraysize) {
//    expand_array(seqToSend);
//  }
 
  
  // send data to transmitter
  radio.write((void*) &randNum, sizeof(int), false);
  Serial.println("wrote data");
  
  radio.startListening();

    // true if transmitter pressed right button
    int roundSucceeded = 0;   
    while (true) {
        // get response from transmitter
        radio.read((void *)&roundSucceeded, sizeof(int));

        if (roundSucceeded) {
          Serial.println("ROUND SUCCEEDED");
          Serial.println(roundSucceeded);
          break;
        }
    }

    // succeeded
    if (roundSucceeded == 1) {
      Serial.println("round " + String(rounds) + " succeeded");
      digitalWrite(GREEN_LED, HIGH);
      delay(2000);
      digitalWrite(GREEN_LED, LOW);
      rounds++;
    } else if (roundSucceeded == 2) {
      Serial.println("round " + String(rounds) + " not succeeded");
      digitalWrite(RED_LED, HIGH);
      delay(2000);
      digitalWrite(RED_LED, LOW);
      free(seqToSend);
      // free when exiting
      Serial.println("program ended");
      delay(100000000);
    }
  
   radio.stopListening();
}
