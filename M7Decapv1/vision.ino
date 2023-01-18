//Functions used by the detection algorithm
// Could add a term that is proportional to its distance from the middle
//Calls GetPos() and check its reliability over 2 iterations
long finalPos() {
  //Init the camera
  cam.setStandby(false); //Put the camera out of standby
  delay(5);

  //Check that a flask is present and that the camera is properly working
  //isFlaskPresent();
  uint8_t tol = 5; //Tolerance between 2 algo runs to validate position of edge
  long temp1 = detectEdges();
  long temp2 = detectEdges();
  uint8_t i = 0;

  while(abs(temp1-temp2)>tol && i<5){
    cam.begin(RESOLUTION,IMAGE_MODE,15); //reset the camera
    delay(30);
    temp1 = detectEdges();
    delay(5);
    temp2 = detectEdges();
    delay(5);
    i++;
  }
  

  cam.setStandby(true); //Camera back in standby mode
  double dist1 = 0;
  double dist2 = 0;
  double pixtomm = 0.21111; //space between 2 bumps = 10mm = 45 px
  double pi = 3.14159265358979323846;  //is definitely right
  double distToCap = 38; //Distance between the camera and the cap
  double radiusCap = 22.5;
  double capCenter = 152; //Position of the center of the cap on the camera (in px)
  //Distance between the center and the edge in mm
  dist1 = (temp1-capCenter) * pixtomm; //Compute the dist with the center
  // The positive direction for C is clockwise
  //calibration is the base position
  //24° is the angle between 2 bumps -> 15 bumps on each cap -> 360/15 = 24
  //We want to move between 2 bumps
  dist1 = ceil(uSToTurnC*(atan2(dist1,radiusCap)/(2*pi))+uSToTurnC/30);
  //+calibration -dist1*dist1*caliProp;
  dist2 = (temp2-capCenter) * pixtomm;
  dist2 = ceil(uSToTurnC*(atan2(dist2,radiusCap)/(2*pi))+uSToTurnC/30);
  Serial.print("Motor should move ");Serial.print(ceil((dist1+dist2)/2));
  Serial.println(" steps to align its claws");
  return (long)ceil((dist1+dist2)*0.5);
}

long detectEdges() {

  float cropped2D[lx][ly];
  float filter2D[3][3] = {{-4,0,4},{-4,0,4},{-4,0,4}};
  float result2D[lx][ly];
  float gaussian2D[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
  float lineAvg[ly];
  float line[ly];
  int edgePos[100]; //Max size if every single pixel is a max (impossible)
  float edge[100];
  if (cam.grabFrame(FB) == 0){
    //Serial.println("Capture done");
    Pfb = FB.getBuffer();
  }
  else{
    //Serial.println("Couldn't take a capture");
    return 0;
    }
  for(int i = 0; i<lx; i++){ //Crop the framebuffer and put it in 2D
    for(int j = 0; j<ly;j++) {
      cropped2D[i][j] = 0;
      cropped2D[i][j] = *(Pfb+ (j+cropy[0])*imgW+(i+cropx[0]));
      //cropped2D[i][j] = fb[(j+cropy[0])*imgW+(i+cropx[0])];
      result2D[i][j] = 0;
      //Serial.print(cropped2D[i][j]);Serial.print(" , ");
    }
    //Serial.println("");
  }
  //Serial.println("---------------------------");
  //Compute gaussian avg on the picture with a convolution
  //Serial.println("Gaussian");
  convolution_2D(cropped2D,gaussian2D,result2D);
  for(int i = 0; i<lx; i++){
    for(int j = 0; j<ly; j++){
        cropped2D[i][j] = result2D[i][j]/16;
        result2D[i][j] = 0;
        //Serial.print(cropped2D[i][j]);Serial.print(" , ");
    }
    //Serial.println("");
  }
  //Serial.println("-------------------------");
  //Serial.println("Sobel");
  //Use sobel filter on the averaged picture to detect the edges
  convolution_2D(cropped2D,filter2D,result2D);
  for(int i = 0; i<lx; i++){
    for(int j = 0; j<ly; j++){
        result2D[i][j] = abs(result2D[i][j]);
        if(j<3 || j>ly-3) result2D[i][j] = 0; //remove border
        //Serial.print(result2D[i][j]);Serial.print(" , ");
    }
    //Serial.println("");
  }
  //Serial.println("--------------------------------");
  //Serial.println("Line average");
  //Compute avg on each lines
  for(int i=0;i<ly;i++){
    line[i] = 0;
    lineAvg[i] = 0;
    for(int j = 0; j<lx;j++){
      line[i] = line[i] + result2D[j][i]; //Compute sum on each line
    }
    line[i] = line[i]/lx; // Avg each line
    //Serial.print(line[i]);Serial.print(" , ");
  }
  //Serial.println("");
  //Serial.println("-------------------");
  //Serial.println("moving average 2 & intensity thresholding");
  movingAverage(line,lineAvg,2,ly);
  //Intensity Thresholding
  int int_thresh = 35; //Could add a smart way to compute the threshold by doing histograms
  for(int i = 0; i<ly; i++){
      if(lineAvg[i]> int_thresh)
          lineAvg[i] = lineAvg[i]*2;
      else
          lineAvg[i] = 0;
      line[i] = lineAvg[i];
      //Serial.print(line[i]);Serial.print(" , ");
  }
  //Serial.println("");
  //Serial.println("-----------------");
  //Serial.println("Moving average 3");
  movingAverage(line,lineAvg,3,ly);
  for (int i = 1;i<ly;i++) {
    //Serial.print(lineAvg[i]);Serial.print(" , ");
  }
    //Serial.println("");
  //Serial.println("-----------------");
  //Edge finding -> doesn't consider the 5 pixels on the edge && verify that it is a local max
  int k = 0;
  for(int i = 5; i<ly-5;i++){ // don't consider the 5 pixel on the borders
    if(lineAvg[i]>int_thresh && lineAvg[i]>=lineAvg[i-1] && lineAvg[i] >= lineAvg[i+1]){
      edgePos[k] = i;
      edge[k] = lineAvg[i];
      k++;
    }
  }
  //It selects the edge closest to the front of the camera
  Serial.print("Found : ");Serial.print(k);Serial.println(" edges");
  int max_index = 0;
  float max_val = 0;
  for(int i = 0;i<k;i++){
    if(edge[i]>max_val){
      max_index = i;
      max_val = edge[i];
    }
    Serial.print("Edge : ");Serial.print(i);Serial.print(" is at position : ");
    Serial.print(edgePos[i]);Serial.print(" with value : ");
    Serial.println(edge[i]);
  }
  Serial.print("Maximum edge is at position : ");Serial.print(edgePos[max_index]);
  Serial.print(" with value : ");Serial.println(edge[max_index]);
  return edgePos[max_index] + cropy[0];
  
}

//Takes 2 vectors in & out and put the moving average (2*n +1) in out
void movingAverage(float in[], float out[], int n, int l) {
  for(int i = n;i<l-n;i++){
    out[i] = 0;
    for(int j = i-n;j<=i+n;j++){
      out[i] = out[i] + in[j];
    }
    out[i] = out[i]/(2*n+1);
  }
}
//Perform a 2D convolution P = N*M where N is the original matrix, M a mask and P the result
void convolution_2D(float N[lx][ly], float M[3][3], float P[lx][ly]) {

// find center position of kernel (half of kernel size)

  for (int i = 0; i < lx; i++)              // rows
  {
      for (int j = 0; j < ly; j++)          // columns
      {
          for (int m = 0; m < 3; m++)     // kernel rows
          {
              for (int n = 0; n < 3; n++) // kernel columns
              {
                  // index of input signal, used to check boundary
                  int ii = i + (m - 1);
                  int jj = j + (n - 1);
  
                  // ignore input samples which are out of bound
                  if (ii >= 0 && ii < lx && jj >= 0 && jj < ly)
                    P[i][j] = P[i][j] + N[ii][jj] * M[m][n];
              }
          }
          //P[i][j] = abs(P[i][j]);
      }
  }
}
//Takes a capture and prints it
void printCapture(){
  /*
  if (cam.grabFrame(FB) == 0){
    Serial.println("Capture done");
    Pfb = FB.getBuffer();
    for(int i = 0; i<imgW;i++){
      for(int j = 0; j<imgH;j++){
        Serial.print(*(Pfb+(i*imgH + j))); Serial.print(", "); //The image is kept by column -> 240,240,240
      }
      Serial.println();
    }
  }
  else{Serial.println("Couldn't take a capture");}*/
}

void printCrop(){
    if (cam.grabFrame(FB) == 0){
    Serial.println("Capture done");
    Pfb = FB.getBuffer();
    for(int i = 0; i<lx;i++){  //20 column
      for(int j = 0; j<ly;j++){ //100 lines
        Serial.print(*(Pfb+ (j+cropy[0])*imgW+(i+cropx[0]))); Serial.print(", ");
      }
      Serial.println();
    }
  }
  else{Serial.println("Couldn't take a capture");}
}
