void init_port()
{
  volatile char *ddrf = (char *)0x30;   // DDRF – data direction for rows
  volatile char *ddrk = (char *)0x107;  // DDRK – data direction for columns
  volatile char *portk = (char *)0x108; // PORTK – to enable pull-ups
volatile char *ddra=(char *)0x21;
volatile char *ddrd=(char *)0x2A;
  *ddrf = 0x0F;   // Lower 4 bits as output (rows)
  *ddrk = 0x00;   // All PORTK pins as input (columns)
  *portk = 0xFF;  // Enable pull-ups
  *ddra=0x0FF;
  *ddrd=0xFF;
}


char output(char data)
{
  volatile char *pout= (char *)0x31;
  *pout=data;
  
}

char input()
{
  volatile char *pinK = (char *)0x106;  // PIN register for PORTK
  char input_d = ~(*pinK);              // Invert since active low
  return input_d;
}


int keymap[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7','8','9', 'C'},
  {'*', '0','#','D'}
};

char getkeyr() {
  int row, col;
  char value;

  for (row = 0; row < 4; row++) {
    output(~(1 << row));  // Make one row low at a time (active low)
    delay(1);             // Small delay for signal stabilization
    value = input();
    
    if (value != 0x00) {  // A key was pressed in this row
      for (col = 0; col < 4; col++) {
        if (value & (1 << col)) {
          // Return the mapped key
          delay(200);  // Debounce delay
          return keymap[row][col];
        }
      }
    }
  }
  return 0;  // No key press detected
}


void setup() {
  init_port();
  Serial.begin(9600);
}

volatile char *portd = (char *)0x2B;
volatile char *porta = (char *)0x22;


int seg_pattern[10] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F  // 9
};


 int count;
 char digits[4] = {' ',' ',' ',' '};
void loop() {
  int d0=0;int d1=0;int d2=0;int d3=0;
  
  char key = getkeyr();
 
  // If a valid digit key is pressed
  if ((key >= '0' && key <= '9')&& count < 5 ) {
    // Shift digits left and insert new digit at end
    digits[0] = digits[1];
    digits[1] = digits[2];
    digits[2] = digits[3];
    digits[3] = key;
    count++;
    Serial.print("Key Pressed: ");
    Serial.println(key);
  }

   if (count > 4) {
    delay(1000); // Show number for 2 seconds
    digits[0] = digits[1] = digits[2] = digits[3] = '0'; // Reset digits
    count = 0;  // Reset count
  }

  for (int i = 0; i < 4; i++) {
    if (digits[i] >= '0' && digits[i] <= '9') {
      int digitVal = digits[i] - '0';
      *porta = seg_pattern[digitVal];   // Segment pattern
    } else {
      *porta = 0x00; // Blank if not a digit
    }

    // Select one digit (common cathode)
    switch (i) {
      case 0: *portd = 0x0E; break;  // 1110
      case 1: *portd = 0x0D; break;  // 1101
      case 2: *portd = 0x0B; break;  // 1011
      case 3: *portd = 0x07; break;  // 0111
    }

    delay(5);      // Short delay for multiplexing
    *portd = 0xFF; // Turn off all digits
  }
  }

  

  

  

  
 
 
