boolean musica=true;
int length = 18; // número de notas
char notes[] = "gggebgdddebgebgggg"; // espaços representam pausas
int beats[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,};
int tempo = 300;
boolean PlayingMusic=false;
boolean alarmBuzState=false;
#define BUZZER 13

void setup(){
  pinMode(BUZZER,OUTPUT);
  digitalWrite(BUZZER,LOW);
}
void loop(){
  TurnOnMusic();
}
void beep(){
  digitalWrite(BUZZER,HIGH);
  desligarBuzzer();
}

void desligarBuzzer(){
  delay(200);
  digitalWrite(BUZZER,LOW);
}

void TurnOnMusic(){
  if(PlayingMusic) return;
    PlayingMusic=true;
    if(!musica){
      if(!alarmBuzState){
	beep();
	alarmBuzState=false;
      }else{
	digitalWrite(BUZZER,HIGH);
	void desligarBuzzer();
	alarmBuzState=true;
      }
      PlayingMusic=false;
	return;
    }
  for (int ithread = 0; ithread < length; ithread++) {
    if (notes[ithread] == ' ') {
      delay(beats[ithread] * tempo); // rest
    }
    else {
      playNote(notes[ithread], beats[ithread] * tempo);
    }

    // pausa entre notas
    delay(tempo / 2);
  }
  PlayingMusic=false;
}
void playNote(char note, int duration) {
  char names[] = {
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'   };
  int tones[] = {
    1915, 1700, 1519, 1432, 1275, 1136, 1014, 956   };

  // toque o tom correspondente ao nome da nota
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
      break;
    }
  }
}
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(tone);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(tone);
  }
}
