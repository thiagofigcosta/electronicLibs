short int musica=2;
char music_names[] = {
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'   };
int music_tones[] = {
    1915, 1700, 1519, 1432, 1275, 1136, 1014, 956   };

const int maiormusica=18;//tamanho da maior musica    
int music_length=18;
char music_notes[maiormusica+1];
int music_beats[maiormusica+1];
int music_tempo;
//musica 1
const int music1_length = 18; // número de notas
char music1_notes[music1_length+1] ={"gggebgdddebgebgggg"}; // espaços representam pausas
int music1_beats[music1_length+1] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,};
int music1_tempo = 300;
//musica 2
const int music2_length = 18; // número de notas
char music2_notes[music2_length+1] ={"gggebgdddebgebgggg"}; // espaços representam pausas
int music2_beats[music2_length+1] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,};
int music2_tempo = 300;
//---------------------
boolean music_inBigestLoop=false;
short int music_bigestValue=-1;
boolean music_inLoop=false;
long music_loopValue=-1;
int music_tone;
int music_duration;
boolean music_inDelay=false;
boolean music_delaySucess=false;
boolean music_needSucess=false;
unsigned long music_timer;
float music_microdelay;
#define BUZZER 13

void setup(){
  pinMode(BUZZER,OUTPUT);
  TurnOnMusic();
}

void loop(){
  if(music_inBigestLoop) bigestForAlarm();
  if(music_inDelay) music_count();
}

void TurnOnMusic(){
    switch(musica){
      default:
      music_length=music1_length;
      for(int m=0; m<music_length; m++){
      music_notes[m]=music1_notes[m];
      music_beats[m]=music1_beats[m];
      }
      music_tempo= music1_tempo;
      break;
      
      case 2:
      music_length=music2_length;
      for(int m=0; m<music_length; m++){
      music_notes[m]=music2_notes[m];
      music_beats[m]=music2_beats[m];
      break;
      
      //case -1:
       
      //break;
      }
      music_tempo= music2_tempo;
      break;
    }
    bigestForAlarm();
}

//função do loop principal
void bigestForAlarm(){
  if(music_inBigestLoop==false&&music_bigestValue!=-1)
    return;
  if(music_bigestValue>=music_length){
    music_inBigestLoop=false;
    music_bigestValue=-1;
    return;
  }
  if(music_inDelay==false)
    music_bigestValue++;
  music_inBigestLoop=true;
  if(music_delaySucess&&music_inLoop==false){
    music_needSucess=false;
    music_uDelay(music_tempo / 2*1000);
    music_delaySucess=false;
    return;
  }
    
  if (music_notes[music_bigestValue] == ' ') {
      if(music_inDelay==false&&music_inLoop==false){
	music_needSucess=true;
	music_uDelay(music_beats[music_bigestValue] * music_tempo*1000); // rest
	return;
      }
    }else {
      //toca a nota correspondente
    if(music_inLoop==false)
      for (int i = 0; i < 8; i++) {
        if (music_names[i] == music_notes[music_bigestValue]) {
  	//faz o tom da nota
          music_tone=music_tones[i];
          music_duration=music_beats[music_bigestValue] * music_tempo;
  	  forAlarm();
  	break;
        }
      }
    else
      forAlarm();
  }
}

//função do loop secundario
void forAlarm(){
  if(music_inLoop==false&&music_loopValue!=-1)
    return;
  if(music_loopValue>=music_duration * 1000L){
    digitalWrite(BUZZER, LOW);
    music_inLoop=false;
    music_loopValue=-1;
    return;
  }
  if(music_inDelay==false)
    music_loopValue++;
  music_inLoop=true;
  if(music_loopValue==0&&music_inDelay==false){
    digitalWrite(BUZZER, HIGH);
    music_needSucess=false;
    music_uDelay(music_tone);
  }
  if(music_loopValue>0)
  if(music_delaySucess){
    digitalWrite(BUZZER, HIGH);
    music_needSucess=false;
    music_uDelay(music_tone);
    music_delaySucess=false;
    return;
  }else if(music_inDelay==false){
    digitalWrite(BUZZER, LOW);
    music_needSucess=true;
    music_uDelay(music_tone);
  }
    
}

//função que ativa o delay
void music_uDelay(float alarmdelay){
  music_inDelay=true;
  music_timer=micros();
  music_microdelay=alarmdelay;
}

//função que conta o delay
void music_count(){
  if(music_timer<=micros()-music_microdelay){
      music_timer=micros();
      if(music_needSucess==true)
	music_delaySucess=true;
      music_needSucess=false;
      music_inDelay=false;
  }
}

