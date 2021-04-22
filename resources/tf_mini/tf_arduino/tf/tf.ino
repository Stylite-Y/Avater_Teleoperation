/*
this script is used to read two tf mini sensor, and return angle
*/

#define FilterBufferNum 7

byte buffer1[9];          // data buffer for TF mini sensor 1
byte buffer2[9];          // data buffer for TF mini sensor 2
const int HEADER=0x59;    // frame header of data package

int dist[2];
int strength[2];
byte check[2];

int buffer_filter1[FilterBufferNum];
int buffer_filter2[FilterBufferNum];
int loop_idx1 = 0;
int loop_idx2 = 0;

byte return_buffer[7] = {0};
byte inByte = 0;

int find_middle(int idx){
    int i=0;
    int j=0;
    int tmp=0; 
    int temp_buffer[FilterBufferNum];
    if(idx==0){
        for(int k=0;k<FilterBufferNum;k++){
            temp_buffer[k] = buffer_filter1[k];
        }
        for (j = 0; j < FilterBufferNum - i - 1; j++) {  
            if (buffer_filter1[j] > buffer_filter1[j+1]) {  
                tmp = temp_buffer[j];  
                temp_buffer[j] = temp_buffer[j+1];  
                temp_buffer[j+1] = tmp;  
            }
        }
        return temp_buffer[(FilterBufferNum+1)/2]; 
    }
    if(idx==1){
        for(int k=0;k<FilterBufferNum;k++){
            temp_buffer[k] = buffer_filter2[k];
        }
        for (j = 0; j < FilterBufferNum - i - 1; j++) {  
            if (buffer_filter1[j] > buffer_filter1[j+1]) {  
                tmp = temp_buffer[j];  
                temp_buffer[j] = temp_buffer[j+1];  
                temp_buffer[j+1] = tmp;  
            }
        }
        return temp_buffer[(FilterBufferNum+1)/2]; 
    }         
}  

void serialEvent1(){
    // Serial.readBytes(buffer1,9);
    if(Serial1.read() == HEADER){
        buffer1[0] = HEADER;
        if(Serial1.read() == HEADER){
            buffer1[1] = HEADER;
            if(Serial1.readBytes(buffer1+2,7) == 7){
                check[0] = buffer1[0]+buffer1[1]+buffer1[2]+buffer1[3]+buffer1[4]+buffer1[5]+buffer1[6]+buffer1[7];
                if(check[0]==buffer1[8]){
                    buffer_filter1[loop_idx1] = buffer1[2] + buffer1[3]*256;
                    loop_idx1 ++;
                    loop_idx1 = loop_idx1%FilterBufferNum;
                    // dist[0] = buffer1[2] + buffer1[3]*256;
                    dist[0] = find_middle(0);
                    strength[0] = buffer1[4] + buffer1[5]*256;
                }
            }
        }
    }
}

void serialEvent2(){
    // Serial.readBytes(buffer1,9);
    if(Serial2.read() == HEADER){
        buffer2[0] = HEADER;
        if(Serial2.read() == HEADER){
            buffer2[1] = HEADER;
            if(Serial2.readBytes(buffer2+2,7) == 7){
                check[1] = buffer2[0]+buffer2[1]+buffer2[2]+buffer2[3]+buffer2[4]+buffer2[5]+buffer2[6]+buffer2[7];
                if(check[1]==buffer2[8]){
                    buffer_filter2[loop_idx2] = buffer2[2] + buffer2[3]*256;
                    loop_idx2 ++;
                    loop_idx2 = loop_idx2%FilterBufferNum;
                    //dist[1] = buffer2[2] + buffer2[3]*256;
                    dist[1] = find_middle(1);
                    strength[1] = buffer2[4] + buffer2[5]*256;
                }
            }
        }
    }
}

void setup(){
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1);
    Serial2.begin(115200, SERIAL_8N1);
    Serial1.setTimeout(10);  // 100hz
    Serial2.setTimeout(10);  // 100hz
}

void loop(){
    /*
    //delay(10);
    //Serial.print("dest1=");
    Serial.print(dist[0]);
    //Serial.print("dest2=");
    Serial.print(" ");
    Serial.print(dist[1]);
    Serial.print(" ");
    //Serial.print(strength[0]);
    Serial.print("\n");
    */
    delay(8);
    return_buffer[0] = 0x55;
    return_buffer[1] = 0x56;
    return_buffer[2] = dist[0] & 0xff;
    return_buffer[3] = (dist[0] >> 8) & 0xff;
    return_buffer[4] = dist[1] & 0xff;
    return_buffer[5] = (dist[1] >> 8) & 0xff;
    return_buffer[6] = return_buffer[2] + return_buffer[3] + return_buffer[4] + return_buffer[5];
    /*
    if(Serial.available()>0){
        inByte = Serial.read();
        if(inByte == 0x55){
            Serial.write(return_buffer, 5);
        }
    }
    */
    Serial.write(return_buffer, 7);
}
