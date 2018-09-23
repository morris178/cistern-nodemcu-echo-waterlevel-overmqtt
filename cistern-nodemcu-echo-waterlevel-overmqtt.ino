
#include <ArduinoUnit.h>
#include "helper.h"
#include <vector>
using namespace std;

test(filterResultTest1)
{
  vector<int> dataset;// {151,150,90,149,151, 151}
  dataset.push_back(151);
  dataset.push_back(150);
  dataset.push_back(90);
  dataset.push_back(149);
  dataset.push_back(151);
  dataset.push_back(151);
  assertEqual(getMean(filteredResult(dataset)), 150);
}

test(filterResultTest2)
{
  vector<int> dataset; //{90,91,90,90,90, 90}
  dataset.push_back(90);
  dataset.push_back(91);
  dataset.push_back(90);
  dataset.push_back(90);
  dataset.push_back(90);
  dataset.push_back(90);
  assertEqual(getMean(filteredResult(dataset)), 90);
}

test(filterResultTest3)
{
  vector<int> dataset; 
  dataset.push_back(90);
  dataset.push_back(91);
  dataset.push_back(56);
  dataset.push_back(90);
  dataset.push_back(56);
  dataset.push_back(90);
  dataset.push_back(90);
  dataset.push_back(90);
  dataset.push_back(56);
  dataset.push_back(90);
  dataset.push_back(90);
  dataset.push_back(90);  
  assertEqual(getMean(filteredResult(dataset)), 90);
}

test(alphaPositiveIfReadingExceedsMaxHeight){
  const int h = CISTERN_HEIGHT ;
  assertMore(getAlpha(h), 0);
}



void setup()
{
  Serial.begin(115200);
  while(!Serial) {} // Portability for Leonardo/Micro
}

void loop()
{
  Test::run();
}
