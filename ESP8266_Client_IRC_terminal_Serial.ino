#include <ESP8266WiFi.h>


const char* ssid     = "1503523";
const char* password = "D2E7D32DBC883";

const char* host = "chat.freenode.net";

WiFiClient client;
   
void setup() {
  
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
}



void loop()
{
 
  if (!client.connected()) {
    Serial.println("connecting ...");
    if (client.connect(host, 6667)) {
      Serial.println("connected");
      delay(1000);
      client.println("USER  uk 8 *  : Uwe Kamper\r");
      delay(500);
      client.println("NICK Esp8266IRC\r");
      delay(500);
      client.println("JOIN #tawano\r");
      delay(500);
      handle_irc_connection();
    } 
    else {
      // if you didn't get a connection to the server:
      Serial.println("connection failed");
      delay(2000);
    }
  }





  
}
  
#define IRC_BUFSIZE  32
char from[IRC_BUFSIZE];
char type[IRC_BUFSIZE];
char to[IRC_BUFSIZE];

void handle_irc_connection() {
  char c;
  // if there are incoming bytes available 
  // from the server, read them and print them:
  while(true) {
    if (!client.connected()) {
      return;
    }
    if(client.available()) {
      c = client.read();
    }
    else {
      continue;
    }
    
    if(c == ':') {
      memset(from, 0, sizeof(from));
      memset(type, 0, sizeof(type));
      memset(to, 0, sizeof(to));
      
      read_until(' ', from);
      read_until(' ', type);
      read_until(' ', to);
     
      if(strcmp(type, "PRIVMSG") == 0) {
        print_nick(from);
        ignore_until(':');
        print_until('\r');
      }
      else {
        
        ignore_until('\r');
      } 
    }
    // could be a PING request by the server.
    else if (c == 'P') {
      char buf[5];
      memset(buf, 0, sizeof(buf));
      buf[0] = c;
      for(int i = 1; i < 4; i++) {
        c = client.read();
        buf[i] = c;
      }
      ignore_until('\r');
      if(strcmp(buf, "PING") == 0) {
        client.println("PONG\r");
        //Serial.println("PING->PONG");
      }
    }
  } // end while
   
}

void print_nick(char buffer[]) {
  Serial.print("<");
  for(int i = 0; i < IRC_BUFSIZE - 1; i++) {
    if(buffer[i] == '!') { 
      break; 
    }    
    Serial.print(buffer[i]);
  }
  Serial.print(">");
}

int read_until(char abort_c, char buffer[]) {
  int bytes_read = 0;
  memset(buffer, 0, sizeof(buffer));
  for(int i = 0; i < IRC_BUFSIZE - 1; i++) {
    if (client.available()) {
      char c = client.read();  
      bytes_read++;
      if(c == abort_c) {
        return bytes_read;
      }
      else if (c == '\n') {
        return bytes_read;
      }
      buffer[i] = c;
    }
  }
  ignore_until(abort_c);
  return bytes_read;
}

// reads characters from the connection until
// it hits the given character.
void ignore_until(char c) {
  while(true){
    if (client.available()) {
      char curr_c = client.read();
      if (curr_c == c) {
        return;  
      }
    }
  }
}

// reads characters from the connection until
// it hits the given character.
void print_until(char c) {
  while(true){
    if (client.available()) {
      char curr_c = client.read();
      if (curr_c == c) {
        Serial.println("");
        return;
      }
      Serial.print(curr_c);
    }
  }
}



// reads characters from the connection until
// it hits the given character.
void print_until_endline() {
  while(true){
    if (client.available()) {
      char curr_c = client.read();
      if (curr_c == '\r') {
        curr_c = client.read();
        if (curr_c == '\n') { return; }
      }
      Serial.print(curr_c);
    }
  }
}

