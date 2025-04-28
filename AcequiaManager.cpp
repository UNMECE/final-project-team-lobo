#include "acequia_manager.h"
#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <sstream>


//functions for the acequia manager


//Region implementation
//initializing a region component
Region::Region(std::string name, double waterLevel, double waterNeed, double waterCapacity, bool isFlooded, bool isInDrought, int overflow, int drought)
	: name(name), waterLevel(waterLevel), waterNeed(waterNeed), waterCapacity(waterCapacity), isFlooded(isFlooded), isInDrought(isInDrought), overflow(overflow), drought(drought){}

//this function adjusts the changes to the waterLevel. This can be a negative for positive change.
	void Region::updateWaterLevel(double change){
		waterLevel += change;
		if(waterLevel >= waterCapacity){ 	//condition for when waterLevel reaches waterCapacity
			waterLevel =waterCapacity;
			isFlooded= true;
			isInDrought = false;
			overflow++;
			//std::cout<<"Overflow in region, "<<name <<". Count increasing. Count: " <<overflow <<std::endl;
		}
		else if(waterLevel < waterCapacity && waterLevel>waterNeed)
		{
			isFlooded = false;
			isInDrought= false;
		}
		else if(waterLevel >= 0.2*waterCapacity)
		{
			isFlooded = false;
			isInDrought = false;
		}
		else if(waterLevel <= 0.2*waterCapacity){	//conditions for when waterLevel reaches drought levels
			isInDrought = true;
			isFlooded = false;
			drought++;
		}
		if(waterLevel < 0){		//conditions for when waterLevel is empty
			waterLevel=0;
			isInDrought = true;
			isFlooded = false;
		}
	}
	void Region::addWaterSource(WaterSource *source){
		suppliedWater.push_back(source);
	}


//WaterSource implementation
/*The waterSource class describes the different sources of water provided to each region based on geographic location
and/or aquifers. These sources or water can provide water to several regions depending on whether there is an established
connection from the source to the region. Most waterSources are assigned to the region based on location.*/

WaterSource::WaterSource(std::string name, WaterSourceType type, double waterLevel)
	:name(name), type(type), waterLevel(waterLevel){}

/*void WaterSource::addSuppliedRegion(Region * region){
	suppliedRegions.push_back(region);
}
*/
//Proposed functions
	//a function to update waterSource waterlevel
void WaterSource::updateWaterLevel(double change)
{
	waterLevel += change;
}
// a funciton to add a canal to this waterSource 
/*
void WaterSource::addCanal(Canal *canal)
{
	canals.push_back(canal);
}	
*/


//Canal implementations
/*The canal class is intended to connect the regions and allow a region to send water resources to another region
by opening the canals that connect the regions and set a specific FlowRate. 
 */

//initializing a canal object
Canal::Canal(std::string name, Region* sourceRegion, Region* destinationRegion, WaterSource* waterSource)
	:name(name), sourceRegion(sourceRegion), destinationRegion(destinationRegion), waterSource(waterSource), flowRate(0.0), isOpen(false){}

//setting a flow rate for a canal object
void Canal::setFlowRate(double rate){
	flowRate = rate;

}
//setting a canal as open for water to flow
void Canal::toggleOpen(bool open){
	isOpen = open;
}

//this function updates the water exchange between the regions and waterSources only if the canal is open. 
void Canal::updateWater(int time)	//seconds
{
	if(!isOpen){
		return;
	}
	double change=0;				//change is the amount in gallons being moved
	for(int i = 0; i<time; i++)
	{	
		change += flowRate;			
	}
	double amount;
	amount = change/1000;			// current conversion to allow for readable change during the simulation
//	amount = change/325851;			//conversion to acre-foots
	sourceRegion->updateWaterLevel(-amount);		//this changes the waterLevel of the source region
	destinationRegion->updateWaterLevel(amount);	//this changes the waterLevel of the Destination region					
}

//AcequiaManager implementation
AcequiaManager::AcequiaManager(){}
//AcequiaManager destructor
AcequiaManager::~AcequiaManager(){
	for(auto region : regions)
		delete region;
	for(auto source : waterSources)
		delete source;
	for(auto canal : canals)
		delete canal;
}
//initializing the Random parameters. using helper functions to initialize conditions
void AcequiaManager::initializeRandomParameters(){
	initializeRegions();
	initializeWaterSources();
	initializeCanals();
	initializeConstraints();
	initializeTime();
}
//initializing the time settings
void AcequiaManager::initializeTime()
{
	// std::random_device rd;
	// std::mt19937 gen(rd());
	// std::uniform_int_distribution<int> timeRandom(50,120);

	hour = 0;
	solvedTime=0;
	//SimulationMax = 100;	
	//random function  
//	SimulationMax = timeRandom(gen);
	isSolved = false;
}

void AcequiaManager::initializeRegions(){
//METHOD A (use this first)
//setting fixed values at execution time
//students can read the AcequiaManager to view the initial values and build their logic based on this
/*	
		regions.push_back(new Region("North", 53.0, 49.5, 55, false, false,0,0));
		regions.push_back(new Region("South", 40.0, 39, 45 ,true, false,0,0));
		regions.push_back(new Region("East", 25, 26, 50, false, true,0,0));
*/	

//Method B (this works fine too, but students will not be able to see the random values until after they execute their program)
//setting random initial values for each region waterlevels at execution time
/*	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> waterLevelRandom(0,100);
	std::uniform_real_distribution<double> waterNeedRandom(10,100);
	std::uniform_real_distribution<double> waterCapacityRandom(100,200);

	this initialization produces random values at execution time
		regions.push_back(new Region("North", waterLevelRandom(gen), waterNeedRandom(gen), waterCapacityRandom(gen), false, false,0,0));
		regions.push_back(new Region("South", waterLevelRandom(gen), waterNeedRandom(gen), waterCapacityRandom(gen) ,true, false,0,0));
		regions.push_back(new Region("East", waterLevelRandom(gen), waterNeedRandom(gen), waterCapacityRandom(gen), false, true,0,0));
*/

//Method C (still figuring this one out)
//This option reads in random values for the region and assigns them to the respective region

std::ifstream InputFile;
InputFile.open("RandomValues.dat", std::ios::in);
std::string line;
int line_ct = 0;
	while(!InputFile.eof())
	{	
		std::getline(InputFile, line);
		std::string value;
		std::string name;
		double RegionWaterLevel;
		double RegionWaterNeed;
		double RegionWaterCapacity;
		int ct =0;
		if(line_ct == 0 || line_ct == 2)
		{
			line_ct++;
			continue;
		}
		else if(line_ct == 1)
		{
			SimulationMax = stoi(line);
			line_ct++;
			continue;
		}
		std::stringstream data(line);
		while(std::getline(data, value, ','))
		{
			switch(ct)
			{
					case 0:
						name = value;
						break;
					case 1:
						RegionWaterLevel = stoi(value);
						break;
					case 2:
						RegionWaterNeed = stoi(value);
						break;
					case 3:
						RegionWaterCapacity = stoi(value);
						break;
			}
			ct++;
		}
		if(ct>0)
		{
			regions.push_back(new Region(name, RegionWaterLevel, RegionWaterNeed, RegionWaterCapacity, false,false, 0 ,0));
		}
		line_ct++;
	}	
InputFile.close();




	/*Additional region considerations
		regions.push_back(new Region("Northwest", waterLevel , waterNeed, waterCapacity,false , false))
	*/
}

void AcequiaManager::initializeWaterSources(){
	//Example water source

		waterSources.push_back(new WaterSource("Rio Grande", WaterSourceType::RIVER, 100.0));
		waterSources.push_back(new WaterSource("ABQ Underground Aquifer", WaterSourceType::UNDERGROUND, 200.0));
		waterSources.push_back(new WaterSource("Elephant Butte Dam", WaterSourceType::DAM, 150.0));
		waterSources.push_back(new WaterSource("Pecos", WaterSourceType::RIVER, 80));
	/*Other water sources to consider:
		waterSources.push_back(new WaterSource("San Juan", WaterSourceType::RIVER, 80));
		waterSources.push_back(new WaterSource("Gila", WaterSourceType::RIVER, 30));
		waterSources.push_back(new WaterSource("Chama", WaterSourceType::RIVER, 50));
	*/

	//connecting waterSources to regions
		regions[0]->addWaterSource(waterSources[0]);
		regions[1]->addWaterSource(waterSources[0]);
		regions[0]->addWaterSource(waterSources[1]);
		regions[1]->addWaterSource(waterSources[2]);
		regions[0]->addWaterSource(waterSources[3]);
		regions[2]->addWaterSource(waterSources[3]);

}


void AcequiaManager::initializeCanals(){
	//Example Canals
		canals.push_back(new Canal("Canal A", regions[0], regions[1], waterSources[0]));
		canals.push_back(new Canal("Canal B", regions[1], regions[2], waterSources[2]));
		canals.push_back(new Canal("Canal C", regions[0], regions[2], waterSources[3]));
		canals.push_back(new Canal("Canal D", regions[2], regions[0], waterSources[2]));		
}

void AcequiaManager::initializeConstraints(){
	//changing the boolean values for each region based on the randomly generated variables
	for(const auto& region:regions)
	{
		region->updateWaterLevel(0);
		region->overflow = 0;
		region->drought = 0;
	}
}

/*This function moves time forward while also brokering the water transfers that have been taken by the student. */
void AcequiaManager::nexthour(){
	for(const auto& canal: canals){
		if(canal->isOpen == true)
		{
			canal->updateWater(3600);
		}
	}
	//this checks to see if the criteria for being solved has been met.
	isSolved = solved();
	//if simulation is solved, the duration to solve is stored and the simulation ends
	if(isSolved == true)
	{
		solvedTime = hour;
	}
	hour++;
}



//this function checks to see if each region meets all the criteria and returns a boolean 
//if any criteria is NOT met, the boolean returns as false
//otherwise, the boolean will return as true
bool AcequiaManager::solved()
{
	bool test = true;
	for(const auto& region: regions)
	{
		if(region->isFlooded || region->isInDrought || region->waterLevel <= region->waterNeed){
			test=false;
		}
	}
	return test;
}

//a function that counts every instance a region has been over capacity or completely droughted
int AcequiaManager::penalties(){
	int count = 0;

	for(const auto& region: regions)
	{
		count += region->overflow;
		count += region->drought; 
	}
	return count;
}

//showing the current state of each ergion
void AcequiaManager::displayState()const{
	std::cout<<"Current State: \n";
	std::cout<<"-----------------\n";
	for(const auto& region: regions){
		std::cout<<"Region: " <<region->name <<", Water Level: " <<region->waterLevel <<", Water Need: " <<region->waterNeed
		<<", Flooded: " <<(region->isFlooded?"Yes" : "No")
		<<", Drought: " <<(region->isInDrought?"Yes" : "No") <<"\n";
	}
	std::cout<<"------------------\n";
}

//evaluating the state of each region after the simulation has completed its run
void AcequiaManager::evaluateSolution(){
	//Evaluate the soulution based on how well the problems are addressed
	double score = 0.0;
	for(const auto& region: regions){
		if(!region->isFlooded && !region->isInDrought &&region->waterLevel >= region->waterNeed){
			score += 10.0;
		}
	}
	/*penalties calculation*/
	int penalty = 0;
	penalty = penalties();
	score -= penalty;

	//checking if the simulation was solved
	if(isSolved)		//if solved, report the time for solved
	{
		score += 50;	//a point bonus for solving in time
		std::cout<<"Time solved = " <<solvedTime <<std::endl;
	}
	else if(!isSolved)
	{					//if not solved, let the user know 
		std::cout<<"Not all regions were solved in time." <<std::endl;
	}



	//reporting the final score
	leaderboard["StudentSolution"] = score;
	std::cout<<"--------------------\n" <<std::endl;
}


//displaying the leaderboard for multiple users
void AcequiaManager::displayLeaderboard() const{
	std::cout<<"----------------\n";
	std::cout<<"Leaderboard: \n";
	for(const auto& entry: leaderboard){
		std::cout<<entry.first <<":" <<entry.second <<"\n";
	}
}

const std::vector<Region *> & AcequiaManager::getRegions() const{
	return regions;
}

const std::vector<WaterSource*> &AcequiaManager::getWaterSources()const{
	return waterSources;
}

const std::vector<Canal*> &AcequiaManager::getCanals() const{
	return canals;
}

