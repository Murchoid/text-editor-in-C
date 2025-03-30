#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

#define CTRL_KEY(k) ((k)& 0x1f)

struct termios orig_termios;

void die(const char *s){
	perror(s);
	exit(1);
}

void disableRawMode() {

 if( tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios)==-1) die("tcsetattr");

}
void enableRawMode() {

  if(tcgetattr(STDIN_FILENO, &orig_termios)==-1) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON );
 raw.c_cflag |= (CS8);
 raw.c_cc[VMIN] = 0;
 raw.c_cc[VTIME] = 1;
 raw.c_oflag &=~(OPOST);
  raw.c_lflag &= ~(ECHO |  ICANON | ISIG |  IEXTEN);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

}

char editorReadKey(){
	int nread;
	char c;
	while((nread = read(STDIN_FILENO, &c, 1) )!= 1){
		if(nread == -1 && errno != EAGAIN) die("read");
	}
	return c;
}


void editorProcessKeyPressed(){
	char c = editorReadKey();
	switch(c){
		case CTRL_KEY('q'):
			exit(0);
			break;
	}
}

void editorRefreshScreen(){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
}

int main() {
	
	
	enableRawMode();

 	
  	while(1){
		editorRefreshScreen();
		editorProcessKeyPressed();
	}
 	 return 0;
}
