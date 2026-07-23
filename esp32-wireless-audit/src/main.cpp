#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <vector>
#include <algorithm>

TFT_eSPI tft = TFT_eSPI();
WebServer server(80);

struct AP { String ssid,bssid,enc; int32_t rssi; int32_t ch; };
struct BLEDev { String name,addr; int rssi; };
std::vector<AP> aps;
std::vector<BLEDev> bles;
int screen=0;
const int BTN1=35, BTN2=0;

String esc(String s){s.replace("&","&amp;");s.replace("<","&lt;");s.replace(">","&gt;");s.replace("\"","&quot;");return s;}
String encName(wifi_auth_mode_t e){ if(e==WIFI_AUTH_OPEN)return "OPEN"; if(e==WIFI_AUTH_WEP)return "WEP"; if(e==WIFI_AUTH_WPA_PSK)return "WPA"; if(e==WIFI_AUTH_WPA2_PSK)return "WPA2"; if(e==WIFI_AUTH_WPA_WPA2_PSK)return "WPA/WPA2"; return "SECURED"; }

void scanWiFi(){
  aps.clear(); tft.fillScreen(TFT_BLACK); tft.setCursor(12,30); tft.println("Scanning Wi-Fi...");
  int n=WiFi.scanNetworks(false,true);
  for(int i=0;i<n;i++) aps.push_back({WiFi.SSID(i),WiFi.BSSIDstr(i),encName(WiFi.encryptionType(i)),WiFi.RSSI(i),WiFi.channel(i)});
  std::sort(aps.begin(),aps.end(),[](const AP&a,const AP&b){return a.rssi>b.rssi;});
  WiFi.scanDelete();
}
void scanBLE(){
  bles.clear(); tft.fillScreen(TFT_BLACK); tft.setCursor(12,30); tft.println("Scanning BLE...");
  BLEScan *s=BLEDevice::getScan(); s->setActiveScan(true); BLEScanResults r=s->start(4,false);
  for(int i=0;i<r.getCount();i++){ BLEAdvertisedDevice d=r.getDevice(i); bles.push_back({d.haveName()?String(d.getName().c_str()):String("Unknown"),String(d.getAddress().toString().c_str()),d.getRSSI()}); }
  s->clearResults();
}
void draw(){
  tft.fillScreen(TFT_BLACK); tft.setTextColor(TFT_CYAN,TFT_BLACK); tft.setTextSize(2); tft.setCursor(10,8); tft.println("WIRELESS AUDIT");
  tft.setTextSize(1); tft.setTextColor(TFT_WHITE,TFT_BLACK);
  if(screen==0){ tft.setCursor(10,42);tft.println("> Wi-Fi Scanner");tft.setCursor(10,62);tft.println("  BLE Scanner");tft.setCursor(10,82);tft.println("  Dashboard"); }
  else if(screen==1){ tft.setCursor(8,35);tft.printf("Wi-Fi: %d networks\n",aps.size()); int y=52; for(int i=0;i<(int)aps.size()&&i<8;i++){tft.setCursor(8,y);tft.printf("%2d %4ddBm CH%02d %.15s",i+1,aps[i].rssi,aps[i].ch,aps[i].ssid.c_str());y+=18;} }
  else if(screen==2){ tft.setCursor(8,35);tft.printf("BLE: %d devices\n",bles.size()); int y=52;for(int i=0;i<(int)bles.size()&&i<8;i++){tft.setCursor(8,y);tft.printf("%2d %4ddBm %.17s",i+1,bles[i].rssi,bles[i].name.c_str());y+=18;} }
  else {tft.setCursor(8,38);tft.println("Audit AP active");tft.setCursor(8,58);tft.println("SSID: ESP32-AUDIT");tft.setCursor(8,78);tft.print("IP: ");tft.println(WiFi.softAPIP());tft.setCursor(8,105);tft.printf("Wi-Fi: %d | BLE: %d",aps.size(),bles.size());}
  tft.setTextColor(TFT_DARKGREY,TFT_BLACK);tft.setCursor(8,220);tft.println("BTN1 scan/menu   BTN2 next");
}
String page(){
  String h=R"HTML(<!doctype html><html><head><meta name=viewport content='width=device-width,initial-scale=1'><meta charset=utf-8><title>Wireless Audit</title><style>body{font-family:system-ui;background:#071018;color:#e9f7ff;margin:0}header{padding:22px;background:#0b1722;position:sticky;top:0}h1{margin:0;color:#63e6ff}.wrap{padding:18px;max-width:1100px;margin:auto}.cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:12px}.card,table{background:#0e1d29;border:1px solid #18384a;border-radius:16px}.card{padding:18px}table{width:100%;border-collapse:collapse;overflow:hidden;margin-top:16px}th,td{padding:10px;text-align:left;border-bottom:1px solid #18384a}button{background:#63e6ff;border:0;padding:12px 18px;border-radius:12px;font-weight:700}small{color:#8aa7b8}</style></head><body><header><h1>ESP32 Wireless Audit</h1><small>Passive inventory for authorized networks</small></header><div class=wrap><div class=cards><div class=card><b>Wi-Fi networks</b><h2>)HTML"+String(aps.size())+R"HTML(</h2></div><div class=card><b>BLE devices</b><h2>)HTML"+String(bles.size())+R"HTML(</h2></div><div class=card><form action=/scan method=post><button>Run authorized scan</button></form></div></div><h2>Wi-Fi inventory</h2><table><tr><th>SSID</th><th>BSSID</th><th>RSSI</th><th>CH</th><th>Security</th></tr>)HTML";
  for(auto&a:aps)h+="<tr><td>"+esc(a.ssid)+"</td><td>"+a.bssid+"</td><td>"+String(a.rssi)+"</td><td>"+String(a.ch)+"</td><td>"+a.enc+"</td></tr>";
  h+="</table><h2>BLE inventory</h2><table><tr><th>Name</th><th>Address</th><th>RSSI</th></tr>";for(auto&b:bles)h+="<tr><td>"+esc(b.name)+"</td><td>"+b.addr+"</td><td>"+String(b.rssi)+"</td></tr>";h+="</table></div></body></html>";return h;
}
void setup(){
  Serial.begin(115200); pinMode(BTN1,INPUT_PULLUP);pinMode(BTN2,INPUT_PULLUP);pinMode(TFT_BL,OUTPUT);digitalWrite(TFT_BL,HIGH);
  tft.init();tft.setRotation(1);tft.fillScreen(TFT_BLACK);tft.setTextColor(TFT_CYAN);tft.setTextSize(2);tft.setCursor(75,55);tft.println("FAUST");delay(1500);
  WiFi.mode(WIFI_AP_STA);WiFi.softAP("ESP32-AUDIT");BLEDevice::init("ESP32-AUDIT");
  server.on("/",HTTP_GET,[]{server.send(200,"text/html",page());});
  server.on("/scan",HTTP_POST,[]{scanWiFi();scanBLE();screen=3;draw();server.sendHeader("Location","/");server.send(303);});server.begin();draw();
}
void loop(){
  server.handleClient(); static uint32_t last=0;if(millis()-last>300&&digitalRead(BTN2)==LOW){screen=(screen+1)%4;draw();last=millis();}
  if(millis()-last>300&&digitalRead(BTN1)==LOW){if(screen==0||screen==1){scanWiFi();screen=1;}else if(screen==2){scanBLE();}else screen=0;draw();last=millis();}
}
