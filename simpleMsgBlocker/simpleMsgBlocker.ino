#include <IRremote.h>

const int RECV_PIN = 12;
const int STATUS_PIN = 13;
// I discovered the raw code for LG back looks something like this:
const unsigned int backRaw[]={63234,184,83,18,4,18,5,17,28,16,6,17,5,17,6,17,5,18,5,16,28,17,28,16,6,17,27,17,28,17,27,17,28,17,27,17,28,17,27,17,6,16,28,16,29,16,6,17,28,15,7,17,5,17,6,17,27,17,6,16,6,14,31,13,9,13,32,13};
const int rawLen= 68;


IRrecv irrecv(RECV_PIN);
IRsend irsend;
decode_results results;

void setup() {
  Serial.begin(9600);
  //Manually build the results object, using prior knowledge of what the back button looks like
  results.rawbuf=backRaw;
  results.rawlen=rawLen;
  results.decode_type =-1;
  storeCode(&results);
  //irrecv.enableIRIn(); // Start the receiver
  pinMode(STATUS_PIN, OUTPUT);
}
// Storage for the recorded code
int codeType = -1; // The type of code
unsigned int rawCodes[RAWBUF]; // The durations if raw, 101 is the max size of this buffer
int codeLen; // The length of the code

// Stores the code for later playback
// Most of this code is just logging
void storeCode(decode_results *results) 
{
  codeType = results->decode_type;
  int count = results->rawlen;
    if (codeType == UNKNOWN) {
    codeLen = results->rawlen - 1;
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
    for (int i = 1; i <= codeLen; i++) {
      if (i % 2) {
        // Mark
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
        Serial.print(" m");
      } 
      else {
        // Space
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
        Serial.print(" s");
      }
      Serial.print(rawCodes[i - 1], DEC);
    }
    Serial.println("");
  }
}

void sendCode(int repeat)
{
 if (codeType == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    irsend.sendRaw(rawCodes, codeLen, 38);
  }
}

void displayRaw(const unsigned int buf[],int len)
{
  //I used this to reverse engineer what was received by the IR from the remote
   for (unsigned int i = 0;  i < len;  i++) 
   {
       Serial.print(buf[i]);
       Serial.print(",");
   }
    Serial.println();
}
void attemptToReceive()
{
  if (irrecv.decode(&results)) {
    digitalWrite(STATUS_PIN, HIGH);
    storeCode(&results);
    Serial.print("raw buff: ");
    displayRaw(results.rawbuf,results.rawlen);
    Serial.print("raw len: ");
    Serial.println(results.rawlen);
    irrecv.resume(); // resume receiver
    digitalWrite(STATUS_PIN, LOW);
  }
}

bool enableIR = false;

void loop() {
  if (enableIR) {
    //Serial.println("listening for IR");
    //irrecv.enableIRIn(); // Re-enable receiver
  }
  //delay(1000);
  enableIR=true;
  if (true) {
    Serial.println("Sending");
    //digitalWrite(STATUS_PIN, HIGH);
    sendCode(1);
    digitalWrite(STATUS_PIN, LOW);
    delay(50); // Wait a bit between retransmissions
    //Serial.print("raw buff: ");
    //displayRaw(results.rawbuf,results.rawlen);
  } 

  attemptToReceive();
}
