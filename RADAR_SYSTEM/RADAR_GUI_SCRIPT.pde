import processing.serial.*;

Serial myPort;       
String serialData = "";
float angle = 0;
float distance = 0;
float maxDistance = 60; // Matches the 60cm threshold in your STM32 code

void setup() {
  size(1000, 600); // Radar canvas dimensions
  background(0);
  
  // 1. Prints your available COM ports to the console below
  println("--- AVAILABLE COM PORTS ---");
  printArray(Serial.list());
  println("---------------------------");
  
  // 2. AUTOMATIC COM5 DETECTION LOGIC
  int portIndex = -1;
  for (int i = 0; i < Serial.list().length; i++) {
    if (Serial.list()[i].equals("COM5")) {
      portIndex = i;
      break;
    }
  }
  
  // Safety check: Fall back to index 0 if COM5 isn't found
  if (portIndex == -1) {
    println("WARNING: COM5 not found in the list! Falling back to index 0.");
    portIndex = 0; 
  } else {
    println("Success! Automatically locked onto COM5 at list index: " + portIndex);
  }
  
  // Initialize serial communication at 9600 baud using the found index
  String portName = Serial.list()[portIndex]; 
  myPort = new Serial(this, portName, 9600);
  
  // Only trigger serialEvent() when a complete line ends with a newline (\n)
  myPort.bufferUntil('\n'); 
}

void draw() {
  // Creates a beautiful, glowing fade-trail effect
  fill(0, 12); 
  noStroke();
  rect(0, 0, width, height);
  
  // Render layout segments
  drawRadarGrid();
  drawSweepLine();
  drawDetectedTarget();
}

// Automatically catches background byte packets from the CP2102
void serialEvent(Serial myPort) {
  try {
    serialData = myPort.readStringUntil('\n');
    if (serialData != null) {
      serialData = trim(serialData); // Drop whitespace or hidden bits
      
      // Split "Angle,Distance" at the comma separator
      String[] dataset = split(serialData, ',');
      
      if (dataset.length == 2) {
        angle = float(dataset[0]);
        distance = float(dataset[1]);
      }
    }
  } catch(Exception e) {
    // Gracefully catch parsing hiccups without crashing the visuals
    println("Skipping bad frame packet: " + e.getMessage());
  }
}

void drawRadarGrid() {
  pushMatrix();
  translate(width/2, height - 50); // Shifts origin point to bottom-middle
  noFill();
  strokeWeight(2);
  stroke(0, 130, 0); // Dim tactical military green
  
  // Draw layout concentric distance arcs (20cm, 40cm, 60cm targets)
  arc(0, 0, 300, 300, PI, TWO_PI);
  arc(0, 0, 600, 600, PI, TWO_PI);
  arc(0, 0, 900, 900, PI, TWO_PI);
  
  // Draw bearing projection lines every 30 degrees
  for (int i = 30; i <= 150; i += 30) {
    float rad = radians(i);
    line(0, 0, -450 * cos(rad), -450 * sin(rad));
  }
  
  // Draw HUD dashboard readings
  fill(0, 220, 0);
  textSize(14);
  text("20 cm", 160, -10);
  text("40 cm", 310, -10);
  text("60 cm", 460, -10);
  
  textSize(18);
  text("BEARING: " + int(angle) + "°", -450, 35);
  text("RANGE: " + int(distance) + " cm", 280, 35);
  popMatrix();
}

void drawSweepLine() {
  pushMatrix();
  translate(width/2, height - 50);
  strokeWeight(3);
  stroke(0, 255, 60); // Vibrant green emitter sweep line
  
  // Convert standard polar coordinate vectors to screen 2D Cartesian plane
  float rad = radians(angle);
  float sweepX = -450 * cos(rad);
  float sweepY = -450 * sin(rad); 
  
  line(0, 0, sweepX, sweepY);
  popMatrix();
}

void drawDetectedTarget() {
  // Drop out-of-bounds readings or ghost reflections
  if (distance > 2 && distance <= maxDistance) {
    pushMatrix();
    translate(width/2, height - 50);
    
    // Maps physical sensor window (0-60cm) into screen pixel space (0-450px)
    float radialPixelDist = map(distance, 0, maxDistance, 0, 450);
    float rad = radians(angle);
    
    float targetX = -radialPixelDist * cos(rad);
    float targetY = -radialPixelDist * sin(rad);
    
    // Draw neon crimson threat indicator point
    fill(255, 10, 10, 200); 
    noStroke();
    ellipse(targetX, targetY, 14, 14); 
    popMatrix();
  }
}
