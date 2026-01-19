/*
 * AI Water User
 * ESP32-based web interface for AI interaction (powered by Claude)
 * with real-time water consumption tracking via electrolysis
 *
 * Hardware: ESP32, Relay/MOSFET on GPIO 26
 */

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

// Pin definitions
#define RELAY_PIN 26

// WiFi AP credentials (for accessing the web interface)
const char* ap_ssid = "AI-Water-User";
const char* ap_password = "watermeter123";

// Web server
WebServer server(80);

// Preferences for NVS storage
Preferences preferences;

// Settings structure
struct Settings {
  String apiKey;
  String modelName;        // Claude model to use (e.g., "claude-3-haiku-20240307")
  String wifiSSID;         // WiFi network name for internet access
  String wifiPassword;     // WiFi password for internet access
  float voltage;           // V (volts)
  float vThreshold;        // V_th (volts)
  float conductivity;      // σ (S/m)
  float area;              // A (cm²)
  float gap;               // d (cm)
  float safetyResistor;    // R_safety (ohms) - limits current if electrodes short
  float costPerSecond;     // dollars per second of electrolysis
  float maxDuration;       // Maximum single electrolysis duration (seconds)
  float maxWaterPerSession; // Maximum water per session (grams)
};

Settings settings;

// Session state
struct SessionState {
  float totalCost;
  float totalWaterUsed;
  unsigned long electrolysisStartTime;
  bool isElectrolysisActive;
  String conversationHistory;
};

SessionState session;

// Function prototypes
void loadSettings();
void saveSettings();
void handleRoot();
void handleSettings();
void handleSaveSettings();
void handleGetStatus();
void handleChat();
void handleNewSession();
void handleGetModels();
void handleNotFound();
void startElectrolysis(float cost);
void updateElectrolysis();
float calculateWaterUsed(float seconds);
String makeAnthropicRequest(String message);
String fetchAvailableModels();

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nAI Water User Starting...");

  // Initialize GPIO
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Load settings from NVS
  loadSettings();

  // Initialize session
  session.totalCost = 0;
  session.totalWaterUsed = 0;
  session.isElectrolysisActive = false;
  session.conversationHistory = "";

  // Try to connect to WiFi if credentials are configured
  bool wifiConnected = false;

  if (settings.wifiSSID.length() > 0) {
    Serial.print("Connecting to WiFi: ");
    Serial.println(settings.wifiSSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.wifiSSID.c_str(), settings.wifiPassword.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 40) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      Serial.println("\nWiFi connected!");
      Serial.print("Access web interface at: http://");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\nWiFi connection failed!");
    }
  }

  // If WiFi connection failed or no credentials, start AP as fallback
  if (!wifiConnected) {
    Serial.println("Starting Access Point mode for configuration...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    IPAddress AP_IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(AP_IP);
    Serial.println("Connect to 'AI-Water-User' and configure WiFi in settings.");
  }

  // Configure web server routes
  server.on("/", handleRoot);
  server.on("/favicon.svg", []() {
    Serial.println("Favicon requested");

    // Check if file exists
    if (!SPIFFS.exists("/favicon.svg")) {
      Serial.println("ERROR: /favicon.svg does not exist in SPIFFS!");
      server.send(404, "text/plain", "Favicon not found in SPIFFS");
      return;
    }

    File file = SPIFFS.open("/favicon.svg", "r");
    if (file) {
      Serial.print("Serving favicon.svg, size: ");
      Serial.println(file.size());
      server.streamFile(file, "image/svg+xml");
      file.close();
    } else {
      Serial.println("ERROR: Could not open /favicon.svg");
      server.send(404, "text/plain", "Favicon could not be opened");
    }
  });
  server.on("/settings", HTTP_GET, handleSettings);
  server.on("/settings", HTTP_POST, handleSaveSettings);
  server.on("/status", HTTP_GET, handleGetStatus);
  server.on("/models", HTTP_GET, handleGetModels);
  server.on("/chat", HTTP_POST, handleChat);
  server.on("/new-session", HTTP_POST, handleNewSession);
  server.on("/debug/files", []() {
    String output = "Files in SPIFFS:\n";
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      output += file.name();
      output += " (";
      output += String(file.size());
      output += " bytes)\n";
      file = root.openNextFile();
    }
    server.send(200, "text/plain", output);
  });
  server.on("/debug/settings", []() {
    String output = "Current Settings:\n";
    output += "costPerSecond: $" + String(settings.costPerSecond, 8) + "/s\n";
    output += "maxDuration: " + String(settings.maxDuration) + "s\n";
    output += "voltage: " + String(settings.voltage) + "V\n";
    output += "conductivity: " + String(settings.conductivity) + " S/m\n";
    output += "area: " + String(settings.area) + " cm²\n";
    output += "gap: " + String(settings.gap) + " cm\n";
    output += "safetyResistor: " + String(settings.safetyResistor) + " Ω\n";

    output += "\nConversation History:\n";
    output += "History JSON size: " + String(session.conversationHistory.length()) + " bytes\n";
    output += "Warning threshold: 35,000 bytes\n";
    output += "Maximum allowed: 40,000 bytes\n";

    if (session.conversationHistory.length() > 0) {
      float utilization = (session.conversationHistory.length() / 40000.0) * 100.0;
      output += "Buffer utilization: " + String(utilization, 1) + "%\n";

      DynamicJsonDocument doc(32768);
      if (deserializeJson(doc, session.conversationHistory) == DeserializationError::Ok) {
        output += "Message count: " + String(doc.as<JsonArray>().size()) + " messages\n";
      }

      if (session.conversationHistory.length() > 35000) {
        output += "⚠ WARNING: Approaching limit - start new session soon!\n";
      } else if (session.conversationHistory.length() > 40000) {
        output += "❌ ERROR: Over limit - must start new session!\n";
      }
    } else {
      output += "Message count: 0 (new session)\n";
      output += "Buffer utilization: 0.0%\n";
    }

    output += "\nMemory:\n";
    output += "Free heap: " + String(ESP.getFreeHeap()) + " bytes\n";
    output += "Heap size: " + String(ESP.getHeapSize()) + " bytes\n";

    output += "\nFor $0.0002 API call:\n";
    output += "Duration: " + String(0.0002 / settings.costPerSecond, 2) + " seconds\n";
    float testWater = calculateWaterUsed(0.0002 / settings.costPerSecond);
    output += "Water: " + String(testWater * 1000, 3) + " mg\n";
    server.send(200, "text/plain", output);
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  updateElectrolysis();
}

void loadSettings() {
  preferences.begin("water-meter", false);

  // Load settings with defaults
  settings.apiKey = preferences.getString("apiKey", "");
  settings.modelName = preferences.getString("modelName", "claude-3-haiku-20240307"); // Haiku is cheapest and most available
  settings.wifiSSID = preferences.getString("wifiSSID", "");
  settings.wifiPassword = preferences.getString("wifiPass", "");
  settings.voltage = preferences.getFloat("voltage", 5.0);
  settings.vThreshold = preferences.getFloat("vThreshold", 2.0);
  settings.conductivity = preferences.getFloat("conductivity", 1.0);
  settings.area = preferences.getFloat("area", 10.0);
  settings.gap = preferences.getFloat("gap", 2.0);
  settings.safetyResistor = preferences.getFloat("safetyRes", 10.0); // 10 ohms default
  settings.costPerSecond = preferences.getFloat("costPerSec", 0.00005); // $0.00005 per second (20x longer runs)
  settings.maxDuration = preferences.getFloat("maxDuration", 300.0); // 5 minutes max
  settings.maxWaterPerSession = preferences.getFloat("maxWater", 100.0); // 100g max

  preferences.end();

  Serial.println("Settings loaded from NVS");
}

void saveSettings() {
  preferences.begin("water-meter", false);

  preferences.putString("apiKey", settings.apiKey);
  preferences.putString("modelName", settings.modelName);
  preferences.putString("wifiSSID", settings.wifiSSID);
  preferences.putString("wifiPass", settings.wifiPassword);
  preferences.putFloat("voltage", settings.voltage);
  preferences.putFloat("vThreshold", settings.vThreshold);
  preferences.putFloat("conductivity", settings.conductivity);
  preferences.putFloat("area", settings.area);
  preferences.putFloat("gap", settings.gap);
  preferences.putFloat("safetyRes", settings.safetyResistor);
  preferences.putFloat("costPerSec", settings.costPerSecond);
  preferences.putFloat("maxDuration", settings.maxDuration);
  preferences.putFloat("maxWater", settings.maxWaterPerSession);

  preferences.end();

  Serial.println("Settings saved to NVS");
  Serial.println("Note: If WiFi credentials changed, please restart ESP32 to apply.");
}

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(500, "text/plain", "Failed to open index.html");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void handleSettings() {
  // Return current settings as JSON
  StaticJsonDocument<1024> doc;
  doc["apiKey"] = settings.apiKey;
  doc["modelName"] = settings.modelName;
  doc["wifiSSID"] = settings.wifiSSID;
  doc["wifiPassword"] = settings.wifiPassword;
  doc["voltage"] = settings.voltage;
  doc["vThreshold"] = settings.vThreshold;
  doc["conductivity"] = settings.conductivity;
  doc["area"] = settings.area;
  doc["gap"] = settings.gap;
  doc["safetyResistor"] = settings.safetyResistor;
  doc["costPerSecond"] = settings.costPerSecond;
  doc["maxDuration"] = settings.maxDuration;
  doc["maxWaterPerSession"] = settings.maxWaterPerSession;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSaveSettings() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    // Update settings
    if (doc.containsKey("apiKey")) settings.apiKey = doc["apiKey"].as<String>();
    if (doc.containsKey("modelName")) settings.modelName = doc["modelName"].as<String>();
    if (doc.containsKey("wifiSSID")) settings.wifiSSID = doc["wifiSSID"].as<String>();
    if (doc.containsKey("wifiPassword")) settings.wifiPassword = doc["wifiPassword"].as<String>();
    if (doc.containsKey("voltage")) settings.voltage = doc["voltage"];
    if (doc.containsKey("vThreshold")) settings.vThreshold = doc["vThreshold"];
    if (doc.containsKey("conductivity")) settings.conductivity = doc["conductivity"];
    if (doc.containsKey("area")) settings.area = doc["area"];
    if (doc.containsKey("gap")) settings.gap = doc["gap"];
    if (doc.containsKey("safetyResistor")) settings.safetyResistor = doc["safetyResistor"];
    if (doc.containsKey("costPerSecond")) settings.costPerSecond = doc["costPerSecond"];
    if (doc.containsKey("maxDuration")) settings.maxDuration = doc["maxDuration"];
    if (doc.containsKey("maxWaterPerSession")) settings.maxWaterPerSession = doc["maxWaterPerSession"];

    saveSettings();
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

void handleGetStatus() {
  StaticJsonDocument<512> doc;
  doc["totalCost"] = session.totalCost;
  doc["totalWaterUsed"] = session.totalWaterUsed;
  doc["isElectrolysisActive"] = session.isElectrolysisActive;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleChat() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"No message received\"}");
    return;
  }

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  String message = doc["message"];

  // Make request to Anthropic API
  String response = makeAnthropicRequest(message);

  server.send(200, "application/json", response);
}

void handleNewSession() {
  session.totalCost = 0;
  session.totalWaterUsed = 0;
  session.conversationHistory = "";

  // Stop any active electrolysis
  if (session.isElectrolysisActive) {
    digitalWrite(RELAY_PIN, LOW);
    session.isElectrolysisActive = false;
  }

  server.send(200, "application/json", "{\"success\":true}");
}

void handleGetModels() {
  String modelsResponse = fetchAvailableModels();
  server.send(200, "application/json", modelsResponse);
}

String fetchAvailableModels() {
  if (settings.apiKey == "" || settings.apiKey.length() < 10) {
    return "{\"error\":\"API key not configured\"}";
  }

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(15000);

  HTTPClient http;
  http.setTimeout(20000);
  http.begin(client, "https://api.anthropic.com/v1/models");
  http.addHeader("anthropic-version", "2023-06-01");
  http.addHeader("x-api-key", settings.apiKey);

  Serial.println("Fetching available models from Anthropic API...");
  int httpCode = http.GET();

  Serial.print("Models API Response code: ");
  Serial.println(httpCode);

  String response;
  if (httpCode == 200) {
    response = http.getString();
    Serial.println("Models fetched successfully");
  } else {
    Serial.print("Models API Error: ");
    Serial.println(http.getString());
    response = "{\"error\":\"Failed to fetch models, code: " + String(httpCode) + "\"}";
  }

  http.end();
  return response;
}

void handleNotFound() {
  // Try to serve from SPIFFS
  String path = server.uri();

  Serial.print("handleNotFound: ");
  Serial.print(path);
  Serial.print(" - exists: ");
  Serial.println(SPIFFS.exists(path) ? "yes" : "no");

  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    String contentType = "text/plain";
    if (path.endsWith(".html")) contentType = "text/html";
    else if (path.endsWith(".css")) contentType = "text/css";
    else if (path.endsWith(".js")) contentType = "application/javascript";
    else if (path.endsWith(".json")) contentType = "application/json";
    else if (path.endsWith(".svg")) contentType = "image/svg+xml";
    else if (path.endsWith(".png")) contentType = "image/png";
    else if (path.endsWith(".jpg") || path.endsWith(".jpeg")) contentType = "image/jpeg";
    else if (path.endsWith(".ico")) contentType = "image/x-icon";

    Serial.print("Serving as: ");
    Serial.print(contentType);
    Serial.print(", size: ");
    Serial.println(file.size());

    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "Not Found: " + path);
  }
}

String makeAnthropicRequest(String message) {
  if (settings.apiKey == "" || settings.apiKey.length() < 10) {
    StaticJsonDocument<256> errorDoc;
    errorDoc["error"] = "API key not configured";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    return errorResponse;
  }

  WiFiClientSecure client;
  client.setInsecure(); // Skip certificate verification for simplicity
  client.setTimeout(15000); // 15 second timeout for SSL handshake

  HTTPClient http;
  http.setTimeout(20000); // 20 second timeout for HTTP request
  http.begin(client, "https://api.anthropic.com/v1/messages");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-api-key", settings.apiKey);
  http.addHeader("anthropic-version", "2023-06-01");

  // Build conversation history (use DynamicJsonDocument to avoid stack overflow)
  DynamicJsonDocument requestDoc(49152);  // 48KB buffer for conversation history
  requestDoc["model"] = settings.modelName.length() > 0 ? settings.modelName : "claude-3-haiku-20240307";
  requestDoc["max_tokens"] = 2048;  // Allow longer responses

  JsonArray messages = requestDoc.createNestedArray("messages");

  // Check conversation history size before adding it
  // We use 48KB for the full request, so warn if history + new message approaches ~35KB
  const size_t HISTORY_WARNING_SIZE = 35000;  // bytes - leaves room for new message
  const size_t HISTORY_MAX_SIZE = 40000;       // bytes - hard limit

  if (session.conversationHistory.length() > HISTORY_MAX_SIZE) {
    Serial.print("ERROR: Conversation history too large (");
    Serial.print(session.conversationHistory.length());
    Serial.println(" bytes)");

    StaticJsonDocument<512> errorDoc;
    errorDoc["error"] = "Conversation too long - please click 'New Session' to start fresh";
    errorDoc["historySize"] = session.conversationHistory.length();
    errorDoc["maxSize"] = HISTORY_MAX_SIZE;
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    http.end();
    return errorResponse;
  }

  if (session.conversationHistory.length() > HISTORY_WARNING_SIZE) {
    Serial.print("WARNING: Conversation history approaching limit (");
    Serial.print(session.conversationHistory.length());
    Serial.print("/");
    Serial.print(HISTORY_MAX_SIZE);
    Serial.println(" bytes). Consider starting a new session soon.");
  }

  // Add conversation history if exists
  if (session.conversationHistory.length() > 0) {
    DynamicJsonDocument historyDoc(32768);  // 32KB buffer for history deserialization
    DeserializationError historyError = deserializeJson(historyDoc, session.conversationHistory);

    if (historyError) {
      Serial.print("ERROR: History deserialization failed: ");
      Serial.println(historyError.c_str());
      Serial.print("History length: ");
      Serial.println(session.conversationHistory.length());

      StaticJsonDocument<512> errorDoc;
      errorDoc["error"] = "Conversation history corrupted - please click 'New Session'";
      errorDoc["details"] = String(historyError.c_str());
      String errorResponse;
      serializeJson(errorDoc, errorResponse);
      http.end();
      return errorResponse;
    } else if (historyDoc.overflowed()) {
      Serial.println("ERROR: History buffer overflowed during deserialization!");

      StaticJsonDocument<512> errorDoc;
      errorDoc["error"] = "Conversation too long - please click 'New Session' to start fresh";
      String errorResponse;
      serializeJson(errorDoc, errorResponse);
      http.end();
      return errorResponse;
    } else {
      // Normal case - add all history
      JsonArray historyArray = historyDoc.as<JsonArray>();
      Serial.print("Loading conversation history: ");
      Serial.print(historyArray.size());
      Serial.print(" messages (");
      Serial.print(session.conversationHistory.length());
      Serial.println(" bytes)");

      for (JsonVariant msg : historyArray) {
        messages.add(msg);
      }
    }
  }

  // Add new user message
  JsonObject userMsg = messages.createNestedObject();
  userMsg["role"] = "user";
  userMsg["content"] = message;

  String requestBody;
  serializeJson(requestDoc, requestBody);

  // Check for buffer overflow
  if (requestDoc.overflowed()) {
    Serial.println("ERROR: Request buffer overflowed!");
    Serial.print("Conversation history length: ");
    Serial.println(session.conversationHistory.length());

    StaticJsonDocument<256> errorDoc;
    errorDoc["error"] = "Conversation too long - please start a new session";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    http.end();
    return errorResponse;
  }

  Serial.println("Sending request to Anthropic API...");
  Serial.print("Request body length: ");
  Serial.println(requestBody.length());

  int httpCode = http.POST(requestBody);

  Serial.print("HTTP Response code: ");
  Serial.println(httpCode);

  DynamicJsonDocument responseDoc(16384);  // Increased for longer Claude responses

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.print("Response payload length: ");
    Serial.println(payload.length());
    DeserializationError error = deserializeJson(responseDoc, payload);

    if (error) {
      Serial.print("JSON deserialization failed: ");
      Serial.println(error.c_str());
      Serial.print("Payload preview (first 500 chars): ");
      Serial.println(payload.substring(0, 500));

      StaticJsonDocument<256> errorDoc;
      errorDoc["error"] = "Failed to parse API response: " + String(error.c_str());
      String errorResponse;
      serializeJson(errorDoc, errorResponse);
      http.end();
      return errorResponse;
    }

    String assistantResponse = responseDoc["content"][0]["text"];

    // Extract usage and cost
    int inputTokens = responseDoc["usage"]["input_tokens"];
    int outputTokens = responseDoc["usage"]["output_tokens"];

    // Calculate cost (Claude 3.5 Sonnet pricing)
    float inputCost = inputTokens * 0.000003;  // $3 per 1M tokens
    float outputCost = outputTokens * 0.000015; // $15 per 1M tokens
    float totalCost = inputCost + outputCost;

    session.totalCost += totalCost;

    // Start electrolysis
    startElectrolysis(totalCost);

    // Update conversation history
    JsonObject assistantMsg = messages.createNestedObject();
    assistantMsg["role"] = "assistant";
    assistantMsg["content"] = assistantResponse;

    String newHistory;
    serializeJson(messages, newHistory);
    session.conversationHistory = newHistory;

    // Prepare response (use DynamicJsonDocument to avoid stack overflow)
    DynamicJsonDocument finalDoc(assistantResponse.length() + 512);  // Size based on response length
    finalDoc["response"] = assistantResponse;
    finalDoc["cost"] = totalCost;
    finalDoc["inputTokens"] = inputTokens;
    finalDoc["outputTokens"] = outputTokens;

    String finalResponse;
    serializeJson(finalDoc, finalResponse);
    http.end();
    return finalResponse;
  }

  // Error handling
  Serial.print("API Error. Response body: ");
  String errorPayload = http.getString();
  Serial.println(errorPayload);

  responseDoc.clear();
  String errorMessage;

  if (httpCode == -11) {
    errorMessage = "Request timeout - check WiFi connection";
  } else if (httpCode == 400) {
    errorMessage = "Bad request (400) - request may be malformed. Check serial monitor.";
    Serial.println("400 Error Debug Info:");
    Serial.print("Request body length: ");
    Serial.println(requestBody.length());
    Serial.print("History length: ");
    Serial.println(session.conversationHistory.length());
    Serial.println("Request body preview (first 500 chars):");
    Serial.println(requestBody.substring(0, 500));
  } else if (httpCode == 404) {
    errorMessage = "Model not found. Try: claude-3-haiku-20240307 or claude-3-5-sonnet-20241022";
  } else if (httpCode == 401) {
    errorMessage = "Invalid API key";
  } else {
    errorMessage = "API request failed with code: " + String(httpCode);
  }

  responseDoc["error"] = errorMessage;
  String errorResponse;
  serializeJson(responseDoc, errorResponse);
  http.end();
  return errorResponse;
}

void startElectrolysis(float cost) {
  // Calculate duration based on cost
  float duration = cost / settings.costPerSecond;

  // Apply safety limits
  if (duration > settings.maxDuration) {
    duration = settings.maxDuration;
  }

  // Check if we would exceed max water per session
  float projectedWater = calculateWaterUsed(duration);
  if (session.totalWaterUsed + projectedWater > settings.maxWaterPerSession) {
    duration = 0; // Don't start if it would exceed limit
    Serial.println("Max water per session reached, skipping electrolysis");
    return;
  }

  if (duration > 0.1) { // Only activate if duration is meaningful (>0.1s)
    if (session.isElectrolysisActive) {
      // Already running - just extend the duration, don't reset timer
      Serial.print("Electrolysis already active - extending duration by ");
      Serial.print(duration);
      Serial.println(" seconds");
    } else {
      // Start fresh
      session.isElectrolysisActive = true;
      session.electrolysisStartTime = millis();
      digitalWrite(RELAY_PIN, HIGH);
      Serial.print("Electrolysis started for ");
      Serial.print(duration);
      Serial.println(" seconds");
    }
  }
}

void updateElectrolysis() {
  if (session.isElectrolysisActive) {
    unsigned long elapsed = millis() - session.electrolysisStartTime;
    float elapsedSeconds = elapsed / 1000.0;

    // Calculate expected duration
    float expectedDuration = session.totalCost / settings.costPerSecond;
    if (expectedDuration > settings.maxDuration) {
      expectedDuration = settings.maxDuration;
    }

    // Update water usage in real-time
    session.totalWaterUsed = calculateWaterUsed(elapsedSeconds);

    // Check if we should stop
    if (elapsedSeconds >= expectedDuration) {
      digitalWrite(RELAY_PIN, LOW);
      session.isElectrolysisActive = false;
      Serial.println("Electrolysis stopped");
    }
  }
}

float calculateWaterUsed(float seconds) {
  // With safety resistor, the formula becomes:
  // m = 9.328e-5 * (V - V_th) * σ * A * t / (R_safety * σ * A + 100 * d)
  //
  // Derivation:
  // - Solution resistance: R_sol = 100*d/(σ*A) [with d in cm, A in cm²]
  // - Total resistance: R_total = R_safety + R_sol
  // - Current: I = (V - V_th) / R_total
  // - From Faraday's law (2H2O → 2H2 + O2 needs 4 electrons):
  //   m = I * t * 9.328e-5 [grams]
  //
  // When R_safety = 0, this reduces to:
  // m = 9.328e-7 * (V - V_th) * σ * A * t / d

  float numerator = 9.328e-5 * (settings.voltage - settings.vThreshold) *
                    settings.conductivity * settings.area * seconds;

  float denominator = settings.safetyResistor * settings.conductivity *
                      settings.area + 100.0 * settings.gap;

  // Avoid division by zero
  if (denominator < 0.001) {
    return 0;
  }

  float m = numerator / denominator;
  return m;
}
