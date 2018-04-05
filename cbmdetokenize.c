#include <stdio.h>
#include <unistd.h>

// special chars < 0x20
char *special1[] = {
  "0x00", "0x01", "0x02", "RUN/STOP",
  "0x04", "WHITE", "0x06", "0x07",
  "SHIFT DISABLE", "SHIFT ENABLE", "0x0a", "0x0b",
  "0x0c", "CR", "TEXT MODE", "0x0f",
  "0x10", "CSR DN", "REV ON", "HOME",
  "DEL", "0x15", "0x16", "0x17",
  "0x18", "0x19", "0x1a", "0x1b",
  "RED", "CSR RT", "GREEN", "BLUE",
};

// special chars >= 0x80
// TODO: add CBM graphic characters (unicode?) at 0xa4 ff.
char *special2[] = {
  "0x80", "ORANGE", "0x82", "0x83",
  "0x84", "F1", "F3", "F5",
  "F7", "F2", "F4", "F6",
  "F8", "LF", "GRAPHICS", "0x8f",
  "BLACK", "CSR UP", "REV OFF", "CLR",
  "INSERT", "BROWN", "LIGHT RED", "DARK GRAY",
  "MIDDLE GRAY", "LIGHT GREEN", "LIGHT BLUE", "LIGHT GRAY", 
  "PURPLE", "CSR LF", "YELLOW", "CYAN",
  "SH-SPACE", "0xa1", "0xa2", "0xa3",
  "0xa4", "0xa5", "0xa6", "0xa7",
  "0xa8", "0xa9", "0xaa", "0xab",
  "0xac", "0xad", "0xae", "0xaf",
  "0xb0", "0xb1", "0xb2", "0xb3",
  "0xb4", "0xb5", "0xb6", "0xb7",
  "0xb8", "0xb9", "0xba", "0xbb",
  "0xbc", "0xbd", "0xbe", "0xbf",
  "0xc0", "0xc1", "0xc2", "0xc3",
  "0xc4", "0xc5", "0xc6", "0xc7",
  "0xc8", "0xc9", "0xca", "0xcb",
  "0xcc", "0xcd", "0xce", "0xcf",
  "0xd0", "0xd1", "0xd2", "0xd3",
  "0xd4", "0xd5", "0xd6", "0xd7",
  "0xd8", "0xd9", "0xda", "0xdb",
  "0xdc", "0xdd", "0xde", "0xdf",
  "0xe0", "0xe1", "0xe2", "0xe3",
  "0xe4", "0xe5", "0xe6", "0xe7",
  "0xe8", "0xe9", "0xea", "0xeb",
  "0xec", "0xed", "0xee", "0xef",
  "0xf0", "0xf1", "0xf2", "0xf3",
  "0xf4", "0xf5", "0xf6", "0xf7",
  "0xf8", "0xf9", "0xfa", "0xfb",
  "0xfc", "0xfd", "0xfe", "0xff",
};

// token encoding starts at 0x80
char *tokens[] = {
  "END", "FOR", "NEXT", "DATA", "INPUT#", "INPUT", "DIM",
  "READ", "LET", "GOTO", "RUN", "IF", "RESTORE", "GOSUB",
  "RETURN", "REM", "STOP", "ON", "WAIT", "LOAD", "SAVE",
  "VERIFY", "DEF", "POKE", "PRINT#", "PRINT", "CONT", "LIST",
  "CLR", "CMD", "SYS", "OPEN", "CLOSE", "GET", "NEW",
  "TAB(", "TO", "FN", "SPC(", "THEN", "NOT", "STEP",
  "+", "-", "*", "/", "^", "AND", "OR",
  ">", "=", "<", "SGN", "INT", "ABS", "USR",
  "FRE", "POS", "SQR", "RND", "LOG", "EXP", "COS",
  "SIN", "TAN", "ATN", "PEEK", "LEN", "STR$", "VAL",
  "ASC", "CHR$", "LEFT$", "RIGHT$", "MID$", "GO",
};

int main(void) {
  unsigned char c;
  unsigned char addr_low;
  unsigned char addr_high;
  unsigned char line_low;
  unsigned char line_high;
  int inquotes = 0;

  read(0, &addr_low, 1);
  read(0, &addr_high, 1);
  read(0, &addr_low, 1);
  read(0, &addr_high, 1);
  read(0, &line_low, 1);
  read(0, &line_high, 1);

  printf("%d ", line_low+256*line_high);

  while (read(0, &c, 1)) {
    if (!inquotes && (c>=128 && c<=255)) {
      if (c <=203)
        printf("%s", tokens[c-128]);
      else
        printf("** unknown token 0x%02x **", c);
    } else if (inquotes && c && (c<' ')) {
        printf("[%s]", special1[c]);
    } else if (inquotes && (c>=0x80)) {
        printf("[%s]", special2[c-0x80]);
    } else if (c=='"') {
      putchar(c);
      inquotes = 1-inquotes; 
    } else if (c==0) {
      // found tokenized file with missing quote at line 
      // end, this is a fix to correctly decode subsequent
      // lines
      inquotes = 0; 

      putchar('\n');
      read(0, &addr_low, 1);
      read(0, &addr_high, 1);
      if ((addr_low == 0) && (addr_high) == 0)
        break;
      read(0, &line_low, 1);
      read(0, &line_high, 1);
      printf("%d ", line_low+256*line_high);
    } else
      putchar(c);
  }
}
