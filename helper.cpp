#include <Arduino.h>
#include "config.h"
#include <math.h>
#include "helper.h"
#include <vector>
#define SERIAL_BAUDRATE                 115200

using namespace std;

const double pi = 3.14159265359;
const double r = CISTERN_HEIGHT / 2.0;

double getAlpha(int h){
  double dH = (double)(h);
  double cosAkHptn = (r-dH)/dH;
  double alpha = (2*acos((r-dH)/r));
  return alpha;
  }

double getSurface(int iH){
  double h = (double)iH;
  
  // formula for surface
  // A=r²/2 * (alpha(rad) sin alpha(rad))
  double alpha = getAlpha(iH);
  double surface = r*r/2 * (alpha - sin(alpha));
  return surface;
  }

int getVolume(double surface){
  double volume = (surface*CISTERN_LENGTH/1000.0);
  return (int)volume;
  }

int getLevel(int volume, double maxVolume){
  int level = (int)(maxVolume - volume);
  return level;
  }

double levelPercent(int level, double maxVolume){
  double percent = (double)level / maxVolume *100.0;
  return percent;
  }


vector<int> filteredResult(vector<int> dataset){
  int datasetLength = dataset.size();
  vector<int> reducedDataset;
  for (int i = 0; i < datasetLength; i++){
    int meanOfAllButThisOne = getMeanOfAllButOne(i, dataset);
    int deviance = getDeviance(i, meanOfAllButThisOne, dataset);
    printf("deviance: %d ", deviance);
    if(deviance > 25){
      printf("Filtered Result : %d", getMeanOfAllButOne(i, dataset));
      reducedDataset.push_back(getMeanOfAllButOne(i, dataset));
      for(int j=i+1; j< datasetLength; j++){
        reducedDataset[j] = dataset[j]; 
      }
      return filteredResult(reducedDataset);
    }
    else{
      if(dataset[i] > CISTERN_HEIGHT){
        dataset[i] = CISTERN_HEIGHT;
      }
      
      reducedDataset.push_back(dataset[i]);
    }
  return reducedDataset;
  }
}

int getDeviance(int index, int mean, vector<int> dataset){
  int difference = abs(mean - dataset[index]);
  double deviance = (double)difference / (double)mean *100;
  return round(deviance);
  }

int getMeanOfAllButOne(int excludedIndex, vector<int> dataset){
  int datasetLength = dataset.size(); 
  printf("datasetLength: %d\n", datasetLength);
  double sum=0;
  for(int i = 0; i < datasetLength; i++){
    if(i != excludedIndex){
      printf("dataset: %d\n", dataset[i]);
      sum = sum + (double)dataset[i];
    }
  printf("sum: %2f\n", sum);
  }
  double mean = sum / (double)(datasetLength - 1);
  printf("Mean: %2f\n", mean);
  return round(mean);
}

int getMean(vector<int> dataset){
  int datasetLength = dataset.size(); 
  printf("datasetLength: %d\n", datasetLength);
  double sum=0;
  for(int i = 0; i < datasetLength; i++){
    printf("dataset: %d\n", dataset[i]);
    sum = sum + (double)dataset[i];
    printf("sum: %2f\n", sum);
  }
  double mean = sum / (double)(datasetLength);
  printf("Mean: %2f\n", mean);
  return round(mean);
}
