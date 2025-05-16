#include "acequia_manager.h"

#include <algorithm>

#include <iostream>


void solveProblems(AcequiaManager &manager) {

    auto regions = manager.getRegions();

    auto canals = manager.getCanals();


    for (int hour = 0; hour < manager.SimulationMax; ++hour) {

        bool progress = false; /* Track if progress is being made*/


        /*Sort regions by water deficit (descending order)*/

        std::sort(regions.begin(), regions.end(), [](Region* a, Region* b) {

            return (a->waterNeed - a->waterLevel) > (b->waterNeed - b->waterLevel);

        });


        /*Distribute water from surplus regions to deficit regions*/

        for (auto &canal : canals) {

            auto sourceRegion = canal->sourceRegion;

            auto destinationRegion = canal->destinationRegion;

/*cheking if the source region is above needed water level and destination is under its needed water level*/
if (sourceRegion->waterLevel > (sourceRegion->waterNeed +1) &&


destinationRegion->waterLevel < (destinationRegion->waterNeed+1)) {

                

                double surplus = sourceRegion->waterLevel - sourceRegion->waterNeed;

                double deficit = destinationRegion->waterNeed - destinationRegion->waterLevel;

                double transferAmount = std::min(surplus, deficit);



/*checking to see if the surplus is above the amount of water needed to transfer in order not to cause a deficit in source region */                

if (surplus >transferAmount && destinationRegion->waterLevel < destinationRegion->waterNeed) {

    /* Set flow rate dynamically */

                double flowRate = std::min(transferAmount / 10.0, 1.0); /* Cap flow rate at 1.0*/

                canal->setFlowRate(flowRate);

                canal->toggleOpen(true);


                sourceRegion->updateWaterLevel(-transferAmount); /* Deduct water from source*/

                destinationRegion->updateWaterLevel(transferAmount); /*Add water to destination*/

                

                progress = true;

          }

   } 



/*checking if a region is in drought if none of them are above water need */

else if (!sourceRegion->isInDrought) {


/*setting surplus to be all water about drought levels*/

double surplus = sourceRegion->waterLevel - (sourceRegion->waterCapacity * .2);

double deficit = (destinationRegion->waterNeed + 1) - destinationRegion->waterLevel;

double transferAmount = std::min(surplus, deficit);


/*if region is already solved then the canal will not open*/

if (sourceRegion->waterLevel > sourceRegion->waterNeed && sourceRegion->waterLevel < (sourceRegion->waterNeed + transferAmount)) {

canal->toggleOpen(false);

}


/*checking to see if the surplus is above the amount of water needed to transfer in order not to cause a deficit in source region*/

else if (surplus > transferAmount && destinationRegion->waterLevel < destinationRegion->waterNeed) {

/*Set flow rate dynamically*/

double flowRate = std::min(transferAmount / 10.0, 1.0);  /*Cap flow rate at 1.0*/

canal->setFlowRate(flowRate);

canal->toggleOpen(true);

sourceRegion->updateWaterLevel(-transferAmount); /*Deduct water from source*/

destinationRegion->updateWaterLevel(transferAmount); /*Add water to destination*/


progress = true;

}

}

else {

                canal->toggleOpen(false);

            }

        }


        /*Advance simulation by one hour*/

        manager.nexthour();


        /* Monitor penalties*/

        int currentPenalties = manager.penalties();

        std::cout << "Hour " << hour << ": Current Penalties = " << currentPenalties << std::endl;


        /* If no progress is made, adjust the strategy*/

        if (!progress) {

            std::cerr << "No progress made in hour " << hour << ". Adjusting strategy!" << std::endl;

            break;

        }


        /* Check if the simulation is solved*/

        if (manager.isSolved) {

            std::cout << "Simulation solved at hour " << hour << "." << std::endl;

            break;

        }

    }

}
