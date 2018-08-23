#include "mbed.h"
#include "MQTTEthernet.h"
#include "MQTTClient.h"

DigitalIn button(PC_6);        // push switch 
DigitalOut led(LED1);    // LED_RED on WIZwiki-W7500

char hostname[] = "192.168.77.1";   //Give the IP Address of the MQTT Broker.
int port = 1883;		                // Port number of the MQTT broker.  
char topic[] = "wall_switch"; 

int main(void) {
	MQTTEthernet ipstack = MQTTEthernet();
	MQTT::Client<MQTTEthernet, Countdown> client = MQTT::Client<MQTTEthernet, Countdown>(ipstack);
	
	int rc = ipstack.connect(hostname, port);
	if (rc != 0) {
		return 0;
	}
	char MQTTClientID[30];
    
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
	data.MQTTVersion = 3;
	sprintf(MQTTClientID,"WIZwiki-W7500-client-%d",rand()%1000);
	data.clientID.cstring = MQTTClientID;
	data.username.cstring = "";
	data.password.cstring = "";  
	
	if ((rc = client.connect(data)) != 0) {
		printf("rc from MQTT connect is %d\n", rc);
	}
	
	int value;
	bool sent;
	
	while(1) {
		value = button.read();
		if (value == 1) {
			led = 0;
			sent = 0;
		}
		else {
			led = 1;
			
			if (sent == 0) {
				sent = 1;
			
				MQTT::Message message;
				char buf[16] = "{'status':'on'}";
				message.qos = MQTT::QOS0;
				message.retained = false;
				message.dup = false;
			
				message.payload = buf;
				message.payloadlen = strlen(buf);
			
				rc = client.publish(topic, message);
				client.yield(60);
			}
		}
		wait(0.3);
	}
}