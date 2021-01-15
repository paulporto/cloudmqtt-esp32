#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>


// configuracion del wifi
const char* ssid     = "nombre de tu wifi aqui";
const char* password = "clave de wifi aqui";

// este es el token que me provee shiftr
// mqtt://tecaperupaul:TAzMDQyMTEyNTVa@tecaperupaul.cloud.shiftr.io
// hay que separar el nombre de dominio, nombre de usuario y password

//esta es la configuracion que provee CLoudMQTT.com en el "Instance info"

const char* serverHostname = "postman.cloudmqtt.com";
const char* username       = "oindbzDp";
const char* password_mqtt  = "TAzMDQyMTEyNTVa";
const int   port           =  25785; // este es el puerto que me indica la "Instance info"
//es el puerto de mqttS o SSL. no confundir con el puerto sin cifrado.

// para obtener los certificados hay que ejecutar el comando
// openssl s_client -showcerts -connect postman.cloudmqtt.com:25785
// los certificados suelen cambiar cada año

const char* ca_cert = \ 
"-----BEGIN CERTIFICATE-----\n" \
"MIIFvjCCBKagAwIBAgIRAI/uup0YBW5gXnE4e7gBxBAwDQYJKoZIhvcNAQELBQAw\n" \
"gY8xCzAJBgNVBAYTAkdCMRswGQYDVQQIExJHcmVhdGVyIE1hbmNoZXN0ZXIxEDAO\n" \
"BgNVBAcTB1NhbGZvcmQxGDAWBgNVBAoTD1NlY3RpZ28gTGltaXRlZDE3MDUGA1UE\n" \
"AxMuU2VjdGlnbyBSU0EgRG9tYWluIFZhbGlkYXRpb24gU2VjdXJlIFNlcnZlciBD\n" \
"QTAeFw0yMDEwMTYwMDAwMDBaFw0yMTEwMTYyMzU5NTlaMBoxGDAWBgNVBAMMDyou\n" \
"Y2xvdWRtcXR0LmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMaB\n" \
"+ItHXsxPG3uTgvHPnQJE5MW6g3b034Bzogvqwzvj1ridmyYEf/RuZGuP1xx0n91i\n" \
"0a2BXfsg17rpV/mA1Sb4K9LEMD91xOPoGbYH9XI4fJ39WcgsK62AwZmgRU9OF1x3\n" \
"LWujo4bEfgpiKAuXyBeEDQFBVSnqYcEIMmiQ0g7ZiQ+Qg6wjE13LNULKtgGfXyGc\n" \
"ZdExGLY0GyxQogGxliVILralDalLNRmA/KkDJZNgoqyqYZSyX0tE+2Hajd2sip1B\n" \
"WOGS06lnPpHYIkF3XO+E2XHeffKby6JPfXaifmcgA1ZnwomHuENvbnUeiLHrfwma\n" \
"khcxofRTzINFWfwRvZECAwEAAaOCAocwggKDMB8GA1UdIwQYMBaAFI2MXsRUrYrh\n" \
"d+mb+ZsF4bgBjWHhMB0GA1UdDgQWBBQQ3GglUX0+OHxbywSO5dN5GQptBDAOBgNV\n" \
"HQ8BAf8EBAMCBaAwDAYDVR0TAQH/BAIwADAdBgNVHSUEFjAUBggrBgEFBQcDAQYI\n" \
"KwYBBQUHAwIwSQYDVR0gBEIwQDA0BgsrBgEEAbIxAQICBzAlMCMGCCsGAQUFBwIB\n" \
"FhdodHRwczovL3NlY3RpZ28uY29tL0NQUzAIBgZngQwBAgEwgYQGCCsGAQUFBwEB\n" \
"BHgwdjBPBggrBgEFBQcwAoZDaHR0cDovL2NydC5zZWN0aWdvLmNvbS9TZWN0aWdv\n" \
"UlNBRG9tYWluVmFsaWRhdGlvblNlY3VyZVNlcnZlckNBLmNydDAjBggrBgEFBQcw\n" \
"AYYXaHR0cDovL29jc3Auc2VjdGlnby5jb20wKQYDVR0RBCIwIIIPKi5jbG91ZG1x\n" \
"dHQuY29tgg1jbG91ZG1xdHQuY29tMIIBBQYKKwYBBAHWeQIEAgSB9gSB8wDxAHYA\n" \
"fT7y+I//iFVoJMLAyp5SiXkrxQ54CX8uapdomX4i8NcAAAF1MY1C3AAABAMARzBF\n" \
"AiEAlDbG0T4I3/rsOR5ZSIvfYIl8q0nujIjSW1B7lteEV7oCIHBIH81Fz3emxihI\n" \
"TnN9sDeLh5oGBRqXHI2jGWvCJYjhAHcAlCC8Ho7VjWyIcx+CiyIsDdHaTV5sT5Q9\n" \
"YdtOL1hNosIAAAF1MY1DBQAABAMASDBGAiEA7M0d2/2MOS8tk2bmL+hndZA5f9FE\n" \
"8GYVXlVbrePf60ACIQCnl9qELgk+L6plQ6raFkMizrD1tqChJzzgvvhyDWozDDAN\n" \
"BgkqhkiG9w0BAQsFAAOCAQEAY3kkOk/u4pouNXBpltIxWl3C/pkOh+khrv1Q1AvL\n" \
"IYrJtYGRlkJ32E1ChSxvmBbozx65LfUUeUvjgD/WqHqG9SN+D60dxrFwHhsSjVxQ\n" \
"EKEg7lT7bL2OuvZ75bU9pHUzFIc/nv5BWsEYxS/3CvDLDeOosNoNbww5AwlPM+h2\n" \
"5GCddt14U2KOGu/uprW5Rvemh4CNFRkg/NAB96nLLNV7ECZNsoBKPR388R0ltfNO\n" \
"gFw5eHRsPHt0sfMpVDUmCf+noPuKgv3sNxRCU7xXRKn2lyGL79RWTQzAVca8E/El\n" \
"5R+2zGSv9+jtOxhoRiehN50puL++B2Cl2R4Fxi0zDHsjWQ==\n" \
"-----END CERTIFICATE-----\n" \
"-----BEGIN CERTIFICATE-----\n" \
"MIIGEzCCA/ugAwIBAgIQfVtRJrR2uhHbdBYLvFMNpzANBgkqhkiG9w0BAQwFADCB\n" \
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTgx\n" \
"MTAyMDAwMDAwWhcNMzAxMjMxMjM1OTU5WjCBjzELMAkGA1UEBhMCR0IxGzAZBgNV\n" \
"BAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEYMBYGA1UE\n" \
"ChMPU2VjdGlnbyBMaW1pdGVkMTcwNQYDVQQDEy5TZWN0aWdvIFJTQSBEb21haW4g\n" \
"VmFsaWRhdGlvbiBTZWN1cmUgU2VydmVyIENBMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
"AQ8AMIIBCgKCAQEA1nMz1tc8INAA0hdFuNY+B6I/x0HuMjDJsGz99J/LEpgPLT+N\n" \
"TQEMgg8Xf2Iu6bhIefsWg06t1zIlk7cHv7lQP6lMw0Aq6Tn/2YHKHxYyQdqAJrkj\n" \
"eocgHuP/IJo8lURvh3UGkEC0MpMWCRAIIz7S3YcPb11RFGoKacVPAXJpz9OTTG0E\n" \
"oKMbgn6xmrntxZ7FN3ifmgg0+1YuWMQJDgZkW7w33PGfKGioVrCSo1yfu4iYCBsk\n" \
"Haswha6vsC6eep3BwEIc4gLw6uBK0u+QDrTBQBbwb4VCSmT3pDCg/r8uoydajotY\n" \
"uK3DGReEY+1vVv2Dy2A0xHS+5p3b4eTlygxfFQIDAQABo4IBbjCCAWowHwYDVR0j\n" \
"BBgwFoAUU3m/WqorSs9UgOHYm8Cd8rIDZsswHQYDVR0OBBYEFI2MXsRUrYrhd+mb\n" \
"+ZsF4bgBjWHhMA4GA1UdDwEB/wQEAwIBhjASBgNVHRMBAf8ECDAGAQH/AgEAMB0G\n" \
"A1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjAbBgNVHSAEFDASMAYGBFUdIAAw\n" \
"CAYGZ4EMAQIBMFAGA1UdHwRJMEcwRaBDoEGGP2h0dHA6Ly9jcmwudXNlcnRydXN0\n" \
"LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9uQXV0aG9yaXR5LmNybDB2Bggr\n" \
"BgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6Ly9jcnQudXNlcnRydXN0LmNv\n" \
"bS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAlBggrBgEFBQcwAYYZaHR0cDov\n" \
"L29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0BAQwFAAOCAgEAMr9hvQ5Iw0/H\n" \
"ukdN+Jx4GQHcEx2Ab/zDcLRSmjEzmldS+zGea6TvVKqJjUAXaPgREHzSyrHxVYbH\n" \
"7rM2kYb2OVG/Rr8PoLq0935JxCo2F57kaDl6r5ROVm+yezu/Coa9zcV3HAO4OLGi\n" \
"H19+24rcRki2aArPsrW04jTkZ6k4Zgle0rj8nSg6F0AnwnJOKf0hPHzPE/uWLMUx\n" \
"RP0T7dWbqWlod3zu4f+k+TY4CFM5ooQ0nBnzvg6s1SQ36yOoeNDT5++SR2RiOSLv\n" \
"xvcRviKFxmZEJCaOEDKNyJOuB56DPi/Z+fVGjmO+wea03KbNIaiGCpXZLoUmGv38\n" \
"sbZXQm2V0TP2ORQGgkE49Y9Y3IBbpNV9lXj9p5v//cWoaasm56ekBYdbqbe4oyAL\n" \
"l6lFhd2zi+WJN44pDfwGF/Y4QA5C5BIG+3vzxhFoYt/jmPQT2BVPi7Fp2RBgvGQq\n" \
"6jG35LWjOhSbJuMLe/0CjraZwTiXWTb2qHSihrZe68Zk6s+go/lunrotEbaGmAhY\n" \
"LcmsJWTyXnW0OMGuf1pGg+pRyrbxmRE1a6Vqe8YAsOf4vmSyrcjC8azjUeqkk+B5\n" \
"yOGBQMkKW+ESPMFgKuOXwIlCypTPRpgSabuY0MLTDXJLR27lk8QyKGOHQ+SwMj4K\n" \
"00u/I5sUKUErmgQfky3xxzlIPK1aEn8=\n" \
"-----END CERTIFICATE-----\n" \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFgTCCBGmgAwIBAgIQOXJEOvkit1HX02wQ3TE1lTANBgkqhkiG9w0BAQwFADB7\n" \
"MQswCQYDVQQGEwJHQjEbMBkGA1UECAwSR3JlYXRlciBNYW5jaGVzdGVyMRAwDgYD\n" \
"VQQHDAdTYWxmb3JkMRowGAYDVQQKDBFDb21vZG8gQ0EgTGltaXRlZDEhMB8GA1UE\n" \
"AwwYQUFBIENlcnRpZmljYXRlIFNlcnZpY2VzMB4XDTE5MDMxMjAwMDAwMFoXDTI4\n" \
"MTIzMTIzNTk1OVowgYgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpOZXcgSmVyc2V5\n" \
"MRQwEgYDVQQHEwtKZXJzZXkgQ2l0eTEeMBwGA1UEChMVVGhlIFVTRVJUUlVTVCBO\n" \
"ZXR3b3JrMS4wLAYDVQQDEyVVU0VSVHJ1c3QgUlNBIENlcnRpZmljYXRpb24gQXV0\n" \
"aG9yaXR5MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAgBJlFzYOw9sI\n" \
"s9CsVw127c0n00ytUINh4qogTQktZAnczomfzD2p7PbPwdzx07HWezcoEStH2jnG\n" \
"vDoZtF+mvX2do2NCtnbyqTsrkfjib9DsFiCQCT7i6HTJGLSR1GJk23+jBvGIGGqQ\n" \
"Ijy8/hPwhxR79uQfjtTkUcYRZ0YIUcuGFFQ/vDP+fmyc/xadGL1RjjWmp2bIcmfb\n" \
"IWax1Jt4A8BQOujM8Ny8nkz+rwWWNR9XWrf/zvk9tyy29lTdyOcSOk2uTIq3XJq0\n" \
"tyA9yn8iNK5+O2hmAUTnAU5GU5szYPeUvlM3kHND8zLDU+/bqv50TmnHa4xgk97E\n" \
"xwzf4TKuzJM7UXiVZ4vuPVb+DNBpDxsP8yUmazNt925H+nND5X4OpWaxKXwyhGNV\n" \
"icQNwZNUMBkTrNN9N6frXTpsNVzbQdcS2qlJC9/YgIoJk2KOtWbPJYjNhLixP6Q5\n" \
"D9kCnusSTJV882sFqV4Wg8y4Z+LoE53MW4LTTLPtW//e5XOsIzstAL81VXQJSdhJ\n" \
"WBp/kjbmUZIO8yZ9HE0XvMnsQybQv0FfQKlERPSZ51eHnlAfV1SoPv10Yy+xUGUJ\n" \
"5lhCLkMaTLTwJUdZ+gQek9QmRkpQgbLevni3/GcV4clXhB4PY9bpYrrWX1Uu6lzG\n" \
"KAgEJTm4Diup8kyXHAc/DVL17e8vgg8CAwEAAaOB8jCB7zAfBgNVHSMEGDAWgBSg\n" \
"EQojPpbxB+zirynvgqV/0DCktDAdBgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rID\n" \
"ZsswDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQFMAMBAf8wEQYDVR0gBAowCDAG\n" \
"BgRVHSAAMEMGA1UdHwQ8MDowOKA2oDSGMmh0dHA6Ly9jcmwuY29tb2RvY2EuY29t\n" \
"L0FBQUNlcnRpZmljYXRlU2VydmljZXMuY3JsMDQGCCsGAQUFBwEBBCgwJjAkBggr\n" \
"BgEFBQcwAYYYaHR0cDovL29jc3AuY29tb2RvY2EuY29tMA0GCSqGSIb3DQEBDAUA\n" \
"A4IBAQAYh1HcdCE9nIrgJ7cz0C7M7PDmy14R3iJvm3WOnnL+5Nb+qh+cli3vA0p+\n" \
"rvSNb3I8QzvAP+u431yqqcau8vzY7qN7Q/aGNnwU4M309z/+3ri0ivCRlv79Q2R+\n" \
"/czSAaF9ffgZGclCKxO/WIu6pKJmBHaIkU4MiRTOok3JMrO66BQavHHxW/BBC5gA\n" \
"CiIDEOUMsfnNkjcZ7Tvx5Dq2+UUTJnWvu6rvP3t3O9LEApE9GQDTF1w52z97GA1F\n" \
"zZOFli9d31kWTz9RvdVFGD/tSo7oBmF0Ixa1DVBzJ0RHfxBdiSprhTEUxOipakyA\n" \
"vGp4z7h/jnZymQyd/teRCBaho1+V\n" \
"-----END CERTIFICATE-----\n" ;

/* create an instance of WiFiClientSecure */
WiFiClientSecure espClient;
PubSubClient client(espClient);

/*LED GPIO pin*/
const char led = 2;//

/* topics */
#define COUNTER_TOPIC    "smarthome/room1/counter"
#define LED_TOPIC     "puerta" /* 1=on, 0=off */

long lastMsg = 0;
char msg[20];
int counter = 0;

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  /* we got '1' -> on */
  if ((char)payload[0] == '0' and (char)payload[1] == '0' and (char)payload[2] == 'f' and (char)payload[3] == 'f') {
    digitalWrite(13, LOW);
    for(int i=0;i<128;i++){
      digitalWrite( led, not digitalRead(led) ); 
      delay(90);
      }
      digitalWrite(led, LOW);
      digitalWrite(13, HIGH);
  } else {
    /* we got '0' -> on */
    digitalWrite(led, LOW);
  }

}

void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str(),username,password_mqtt)) {
      Serial.println("connected");
      /* subscribe topic */
      client.subscribe(LED_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void setup() {
  pinMode(led, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(115200);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(3000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  /* set led as output to control led on-off */
  

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  espClient.setCACert(ca_cert);
  
  client.setServer(serverHostname, port); 
  client.setCallback(receivedCallback);
  
}
void loop() {
  /* if client was disconnected then try to reconnect again */
  if (!client.connected()) {
    mqttconnect();
  }
  /* this function will listen for incomming 
  subscribed topic-process-invoke receivedCallback */
  client.loop();
  /* we increase counter every 3 secs
  we count until 3 secs reached to avoid blocking program if using delay()*/
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    if (counter < 100) {
      counter++;
      snprintf (msg, 20, "%d", counter);
      // publish the message 
      client.publish(COUNTER_TOPIC, msg);
    }else {
      counter = 0;  
    }
  }//*/
}
