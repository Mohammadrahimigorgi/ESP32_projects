#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>



String hexToBinaryString(String hexString) {
    String binaryString = "";
   
    // Map each hex digit to its corresponding 4-bit binary representation
    for (int i = 0; i < hexString.length(); i++) {
        char hexDigit = hexString[i];
       
        switch (hexDigit) {
            case '0': binaryString += "0000"; break;
            case '1': binaryString += "0001"; break;
            case '2': binaryString += "0010"; break;
            case '3': binaryString += "0011"; break;
            case '4': binaryString += "0100"; break;
            case '5': binaryString += "0101"; break;
            case '6': binaryString += "0110"; break;
            case '7': binaryString += "0111"; break;
            case '8': binaryString += "1000"; break;
            case '9': binaryString += "1001"; break;
            case 'A': binaryString += "1010"; break;
            case 'B': binaryString += "1011"; break;
            case 'C': binaryString += "1100"; break;
            case 'D': binaryString += "1101"; break;
            case 'E': binaryString += "1110"; break;
            case 'F': binaryString += "1111"; break;
            default:  // Handle invalid characters
                return "Invalid Hex String";
        }
    }
   
    return binaryString;
}

String uint64ToHexString(uint64_t num) {
    char buffer[33];  // 33 hex digits + 1 for the null terminator
    sprintf(buffer, "%016llX", num);  // Convert to hex string with leading zeros

    String hexString = String(buffer);

    // Remove leading zeros
    int firstNonZero = 0;
    while (firstNonZero < 33 && hexString[firstNonZero] == '0') {
        firstNonZero++;
    }

    // Special case for zero
    if (firstNonZero == 33) {
        return "0";
    }

    return hexString.substring(firstNonZero);
}


const int irreceiver_pin = 19;

IRrecv irrecv(irreceiver_pin,2048,50);

decode_results results;
decode_type_t type_decode;




void setup() {

Serial.begin(115200);

Serial.println("Starting IR-receiver... ");

irrecv.enableIRIn();


}

void loop() {
  
if(irrecv.decode(&results)){

Serial.print("code is : ");

String binFormat=hexToBinaryString(uint64ToHexString(results.value));

Serial.println(binFormat);

Serial.println(results.value,HEX);

Serial.print(" type is : ");

Serial.println(results.decode_type);
if (results.decode_type == NEC) {
    Serial.println("NEC");
} else if (results.decode_type == SONY) {
    Serial.println("SONY");
} else if (results.decode_type == RC5) {
    Serial.println("RC5");
} else if (results.decode_type == RC6) {
    Serial.println("RC6");
} else if (results.decode_type == DISH) {
    Serial.println("DISH");
} else if (results.decode_type == SHARP) {
    Serial.println("SHARP");
} else if (results.decode_type == JVC) {
    Serial.println("JVC");
} else if (results.decode_type == SANYO) {
    Serial.println("SANYO");
} else if (results.decode_type == MITSUBISHI) {
    Serial.println("MITSUBISHI");
} else if (results.decode_type == SAMSUNG) {
    Serial.println("SAMSUNG");
} else if (results.decode_type == LG) {
    Serial.println("LG");
} else if (results.decode_type == WHYNTER) {
    Serial.println("WHYNTER");
} else if (results.decode_type == AIWA_RC_T501) {
    Serial.println("AIWA_RC_T501");
} else if (results.decode_type == PANASONIC) {
    Serial.println("PANASONIC");
} else if (results.decode_type == DENON) {
    Serial.println("DENON");
} else if (results.decode_type == MAGIQUEST) {
    Serial.println("MAGIQUEST");
} else if (results.decode_type == XMP) {
    Serial.println("XMP");
} else if (results.decode_type == RC5) { 
    Serial.println("RC5");
} else if (results.decode_type == PIONEER) {
    Serial.println("PIONEER");

} else if (results.decode_type == FUJITSU_AC) {
    Serial.println("FUJITSU");
}
else if (results.decode_type == MAGIQUEST) {
    Serial.println("MAGIQUEST");
}



else {
    Serial.println("UNKNOWN");
}

irrecv.resume();

}


}


String toBinary(uint64_t recieverResult){



}


















