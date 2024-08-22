#include <TFT_eSPI.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>

// Include image header files
#include "image1.h"
#include "image2.h"
#include "image3.h"
#include "image9_general.h"

// Pin Definitions
#define RX_PIN 18  
#define TX_PIN 17 // GPS RX
#define DISPLAY_POWER_PIN 15

// Create display and GPS objects
TFT_eSPI tft = TFT_eSPI();
TinyGPSPlus gps;
HardwareSerial GPS_Serial(1);

struct HistoricalLocation {
  String Location;
  bool isSpecific;
  String HistoricalFigureEvent;
  String Theory;
  double Latitude;
  double Longitude;
  String HistoricalDescription;
  String HistoricalYears;
  String Image;
};

HistoricalLocation locations[9];
HistoricalLocation generalLocation;
HistoricalLocation lastDisplayedLocation;

const char* json = R"rawliteral(
[
	{
		"Location": "Church of Saint Demetrius",
		"isSpecific": "True",
		"Historical Figure/Event": "Saint Demetrius",
		"Theory": "Christian Martyrdom, Patron Saint",
		"Latitude": "40.6389177183036",
		"Longitude": "22.94784423758354",
		"Historical Description": "Saint Demetrius is the patron saint of Thessaloniki, known for his martyrdom during the Roman persecutions of Christians.",
		"Historical Years": "Martyrdom in 306 AD"
	},
	{
		"Location": "Aristotelous Square",
		"isSpecific": "True",
		"Historical Figure/Event": "Aristotle",
		"Theory": "Public Space, Urban Planning",
		"Latitude": "40.6325409827037",
		"Longitude": "22.940825868009295",
		"Historical Description": "Named after the philosopher Aristotle, this central square reflects modern urban planning inspired by classical ideals.",
		"Historical Years": "Designed in 1918"
	},
	{
		"Location": "Arch of Galerius",
		"isSpecific": "True",
		"Historical Figure/Event": "Roman Emperor Galerius",
		"Theory": "Victory Monument, Roman Triumph",
		"Latitude": "40.63238651447119",
		"Longitude": "22.95178328334984",
		"Historical Description": "The Arch of Galerius celebrates the victories of Emperor Galerius, symbolizing Roman power and architectural grandeur.",
		"Historical Years": "Built in 298 - 299 AD"
	},
	{
		"Location": "White Tower",
		"isSpecific": "True",
		"Historical Figure/Event": "Byzantine and Ottoman Era",
		"Theory": "Military Architecture, Fortification",
		"Latitude": "40.62659276947053",
		"Longitude": "22.948414951074533",
		"Historical Description": "The White Tower is a prominent monument and museum, representing the city's strategic importance through Byzantine and Ottoman eras.",
		"Historical Years": "Built in 15th century"
	},
	{
		"Location": "Rotunda of Galerius",
		"isSpecific": "True",
		"Historical Figure/Event": "Roman Engineering",
		"Theory": "Architecture, Engineering",
		"Latitude": "40.6323694728569",
		"Longitude": "22.951766305087553",
		"Historical Description": "The Rotunda of Galerius is an architectural marvel from the Roman period, showcasing the engineering prowess of its time.",
		"Historical Years": "Built in 306 AD"
	},
	{
		"Location": "Ancient Agora",
		"isSpecific": "True",
		"Historical Figure/Event": "Galen",
		"Theory": "Medicine, Anatomy",
		"Latitude": "40.63803186663262",
		"Longitude": "22.945912541020306",
		"Historical Description": "Galen, a prominent Greek physician, lived in the Roman period. His work influenced medical science for centuries.",
		"Historical Years": "129 - 200 AD"
	},
	{
		"Location": "Heptapyrgion Fortress",
		"isSpecific": "True",
		"Historical Figure/Event": "Cyril and Methodius",
		"Theory": "Christianity, Slavic Alphabets",
		"Latitude": "40.644084500763555",
		"Longitude": "22.961856941020553",
		"Historical Description": "Cyril and Methodius, Byzantine Greek brothers, developed the Glagolitic alphabet and spread Christianity among Slavs.",
		"Historical Years": "826 - 869 AD (Cyril), 815 - 885 AD (Methodius)"
	},
	{
		"Location": "HELEXPO",
		"isSpecific": "True",
		"Historical Figure/Event": "Modern Greek Thought",
		"Theory": "Innovation, Economic Development",
		"Latitude": "40.62741915529737",
		"Longitude": "22.95464239784777",
		"Historical Description": "HELEXPO is the largest exhibition and conference center in Greece, hosting the Thessaloniki International Fair, which plays a crucial role in the economic and cultural development of the region.",
		"Historical Years": "Founded in 1926"
	},
	{
		"Location": "General Thessaloniki",
		"isSpecific": "False",
		"Historical Figure/Event": "",
		"Theory": "Ethics, History, Philosophy",
		"Latitude": "40.64059646804942",
		"Longitude": "22.941563300759032",
		"Historical Description": "Thessaloniki is a city rich in history, blending ancient, Roman, Byzantine, and Ottoman influences, shaping its cultural heritage.",
		"Historical Years": "Continuous"
	}
]

)rawliteral";

void parseJson() {
  StaticJsonDocument<4096> doc;
  deserializeJson(doc, json);

  int specificIndex = 0;
  for (int i = 0; i < doc.size(); i++) {
    HistoricalLocation location;
    location.Location = doc[i]["Location"].as<String>();
    location.isSpecific = doc[i]["isSpecific"].as<String>() == "True";
    location.HistoricalFigureEvent = doc[i]["Historical Figure/Event"].as<String>();
    location.Theory = doc[i]["Theory"].as<String>();
    location.Latitude = doc[i]["Latitude"].as<double>();
    location.Longitude = doc[i]["Longitude"].as<double>();
    location.HistoricalDescription = doc[i]["Historical Description"].as<String>();
    location.HistoricalYears = doc[i]["Historical Years"].as<String>();
    location.Image = doc[i]["Image"].as<String>();
    
    if (location.isSpecific) {
      locations[specificIndex++] = location;
    } else {
      generalLocation = location;
    }
  }
}

double haversine(double lat1, double lon1, double lat2, double lon2) {
  const double R = 6371e3; // Earth radius in meters
  double phi1 = lat1 * (PI / 180);
  double phi2 = lat2 * (PI / 180);
  double deltaPhi = (lat2 - lat1) * (PI / 180);
  double deltaLambda = (lon2 - lon1) * (PI / 180);

  double a = sin(deltaPhi / 2) * sin(deltaPhi / 2) +
             cos(phi1) * cos(phi2) *
             sin(deltaLambda / 2) * sin(deltaLambda / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  return R * c; // Distance in meters
}

void displayImage(const uint16_t* image, uint16_t width, uint16_t height) {
  int x = -30;  // Shift image 30 pixels to the left
  int y = (tft.height() - height) / 2;
  tft.pushImage(x, y, width, height, image);
}

void drawWrappedString(String text, int x, int y, int width) {
  int cursorX = x;
  int cursorY = y;
  int lineHeight = 20; // Height of a line of text
  String line = "";

  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    line += c;
    if (c == ' ' || c == '\n') {
      if (tft.textWidth(line.c_str()) > width) {
        line.remove(line.length() - 1);
        tft.drawString(line, cursorX, cursorY);
        line = String(c);
        cursorY += lineHeight;
      }
    }
  }

  if (line.length() > 0) {
    tft.drawString(line, cursorX, cursorY);
  }
}

void displayLocation(const HistoricalLocation& location, double lat, double lon, int sats) {
  tft.fillScreen(TFT_BLACK);

  // Display the corresponding image on the left side, shifted 30 pixels left
  if (location.Image == "image1") {
    displayImage(image1, image1_width, image1_height);
  } else if (location.Image == "image2") {
    displayImage(image2, image2_width, image2_height);
  } else if (location.Image == "image3") {
    displayImage(image3, image3_width, image3_height);
  } else {
    displayImage(image9_general, image9_general_width, image9_general_height);
  }

  // Display the text data on the right side
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM); // Top left datum
  int textX = tft.width() / 2 + 10; // Start text 10 pixels from the middle
  int textY = 10; // Start text from 10 pixels from the top
  int textWidth = tft.width() / 2 - 20; // Width of text area
  tft.setTextSize(1); // Reduce text size

  drawWrappedString("Location: " + location.Location, textX, textY, textWidth);
  drawWrappedString("Event: " + location.HistoricalFigureEvent, textX, textY + 40, textWidth);
  drawWrappedString("Latitude: " + String(lat, 6), textX, textY + 80, textWidth);
  drawWrappedString("Longitude: " + String(lon, 6), textX, textY + 100, textWidth);
  drawWrappedString("Satellites: " + String(sats), textX, textY + 120, textWidth);
}

void updateSatellites(int sats) {
  tft.fillRect(tft.width() / 2 + 10, 130, tft.width() / 2 - 20, 20, TFT_BLACK); // Clear the area
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  tft.drawString("Satellites: " + String(sats), tft.width() / 2 + 10, 130);
}

void updateCoordinates(double lat, double lon) {
  tft.fillRect(tft.width() / 2 + 10, 90, tft.width() / 2 - 20, 40, TFT_BLACK); // Clear the area
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  tft.drawString("Latitude: " + String(lat, 6), tft.width() / 2 + 10, 90);
  tft.drawString("Longitude: " + String(lon, 6), tft.width() / 2 + 10, 110);
}

void setup() {
  // Initialize serial monitor for debugging
  Serial.begin(115200);

  // Turn on display power
  pinMode(DISPLAY_POWER_PIN, OUTPUT);
  digitalWrite(DISPLAY_POWER_PIN, HIGH);
  delay(500); // Small delay to ensure power is stabilized

  // Initialize the display
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);

  // Initialize the GPS serial communication
  GPS_Serial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  // Parse the JSON data
  parseJson();

  // Display initial message
  tft.setCursor(0, 0);
  tft.print("Waiting for GPS...");

}

void loop() {
  static int lastSatellites = -1;
  static double lastLatitude = 0;
  static double lastLongitude = 0;
  
  while (GPS_Serial.available() > 0) {
    char c = GPS_Serial.read();
    Serial.write(c);  // Print raw GPS data to serial monitor for debugging

    if (gps.encode(c)) {
      if (gps.location.isUpdated()) {
        double latitude = gps.location.lat();
        double longitude = gps.location.lng();
        int satellites = gps.satellites.value();

        bool foundSpecificLocation = false;
        HistoricalLocation newLocation = generalLocation;

        for (int i = 0; i < 9; i++) {
          if (locations[i].isSpecific) {
            double distance = haversine(latitude, longitude, locations[i].Latitude, locations[i].Longitude);
            if (distance <= 200) { // 200 meter range
              newLocation = locations[i];
              foundSpecificLocation = true;
              break;
            }
          }
        }

        // Only update the display if the location has changed
        if (newLocation.Location != lastDisplayedLocation.Location) {
          displayLocation(newLocation, latitude, longitude, satellites);
          lastDisplayedLocation = newLocation;
        }

        // Always update the satellites count and coordinates
        if (satellites != lastSatellites) {
          updateSatellites(satellites);
          lastSatellites = satellites;
        }

        if (latitude != lastLatitude || longitude != lastLongitude) {
          updateCoordinates(latitude, longitude);
          lastLatitude = latitude;
          lastLongitude = longitude;
        }
      }
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("No GPS detected", 0, 0);
    while (true);
  }
}
