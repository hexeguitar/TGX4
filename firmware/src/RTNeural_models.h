/**
 * @file RTNeural_models.h
 * @author Piotr Zapart www.hexefx.com
 * @brief Based on Seed pedal design by
 * 		 Keith Bloemer (GuitarML) https://github.com/GuitarML/Seed
 * 		Ported to Teensy4.1 with the following additions/changes:
 * 			- adjusted model volume to match unity gain with bypass signal
 * 			- dual mono input and output
 * 			- added bypass system
 * @version 0.1
 * @date 2024-01-31
 */

#include<vector>

struct modelData {
  std::vector<std::vector<float>> rec_weight_ih_l0; 
  std::vector<std::vector<float>> rec_weight_hh_l0;  
  std::vector<std::vector<float>> lin_weight;
  std::vector<float> lin_bias;
  std::vector<std::vector<float>> rec_bias;
  float levelAdjust;
};

extern modelData Model1; 
extern modelData Model2; 
extern modelData Model3;
extern modelData Model4; 
extern modelData Model5; 
extern modelData Model6; 
extern modelData Model7;
extern modelData Model8; 

extern std::vector<modelData> model_collection;

void setupWeights();