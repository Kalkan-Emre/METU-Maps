#include "METUMaps.h"
#include "GraphExceptions.h"

#include <iostream>

void METUMaps::PrintNotInJourney() const
{
    std::cout << "Device is not in a journey!" << std::endl;
}

void METUMaps::PrintUnableToChangeDestination() const
{
    std::cout << "Cannot change Destination during journey!" << std::endl;
}

void METUMaps::PrintUnableToChangeStartingLoc() const
{
    std::cout << "Cannot change Starting Location during journey!" << std::endl;
}

void METUMaps::PrintAlreadyInJourney() const
{
    std::cout << "Device is already in a journey!" << std::endl;
}

void METUMaps::PrintJourneyIsAlreadFinished() const
{
    std::cout << "Journey is already finished!" << std::endl;
}

void METUMaps::PrintLocationNotFound() const
{
    std::cout << "One (or both) of the locations are not found in the maps!" << std::endl;
}

void METUMaps::PrintJourneyCompleted() const
{
    std::cout << "Journey Completed!" << std::endl;
}

void METUMaps::PrintCachedLocationFound(const std::string& location0,
                                        const std::string& location1) const
{
    std::cout << "Route between \""
              << location0 << "\" and \""
              << location1 << "\" is in cache, using that..."
              << std::endl;
}

void METUMaps::PrintCalculatingRoutes(const std::string& location0,
                                      const std::string& location1) const
{
    std::cout << "Calculating Route(s) between \""
              << location0 << "\" and \""
              << location1 << "\"..."
              << std::endl;
}

std::string METUMaps::GenerateKey(const std::string& location0,
                                  const std::string& location1)
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    return location0 + "/" + location1;
}

METUMaps::METUMaps(int potentialPathCount,
                   const std::string& mapFilePath)
{
    // TODO
    
    this->potentialPathCount = potentialPathCount;
    map = Graph(mapFilePath);
    inJourney = false;
    cachedPaths = KeyedHashTable(potentialPathCount*map.TotalVertexCount());
    startingLoc = "";
    currentLoc = "";
    destination = "";
    currentRoute = std::vector<int>(0);
    
}

void METUMaps::SetDestination(const std::string& name)
{
    // TODO
    if(inJourney)
        PrintUnableToChangeDestination();
    else{
        destination = name;
    }
}

void METUMaps::SetStartingLocation(const std::string& name)
{
    // TODO
    if(inJourney)
        PrintUnableToChangeStartingLoc();
    else{
        startingLoc = name;
    }
}

void METUMaps::StartJourney()
{
    // TODO
    if (inJourney)
    {
        PrintAlreadyInJourney();
        return;
    }
    PrintCalculatingRoutes(startingLoc,destination);
    
    if(startingLoc==""||destination=="")
    {
        PrintLocationNotFound();
        return;
    }


    try {
        std::vector<int> currentPath;
        map.ShortestPath(currentPath,startingLoc,destination);
        }
    catch(VertexNotFoundException()){
        PrintLocationNotFound();
        return;
    }
       

    
    std::vector<std::vector<int> > multipleShortestPaths;
    std::vector<int> currentPath;
    std::vector<int> subPath;
    
    map.MultipleShortPaths(multipleShortestPaths,startingLoc,destination,potentialPathCount);
    
    currentLoc = startingLoc;
    for(int i=0;i<multipleShortestPaths.size();i++)
    {
        currentPath = multipleShortestPaths[i];
        
        for(int j=0; j<currentPath.size(); j++)
        {
            for(int k=j;k<currentPath.size()-j;k++)
            {
                subPath.push_back(currentPath[k]);
            }
            if(subPath[0]!=subPath[subPath.size()-1])
            {
                cachedPaths.Insert(GenerateKey(map.VertexName(subPath[0]),map.VertexName(subPath.size()-1)),subPath);
                subPath.resize(0);
            }
        }
        
    }
    
    if(multipleShortestPaths.size()>0)
        currentRoute = multipleShortestPaths[0];
    inJourney=true;
}

void METUMaps::EndJourney()
{
    // TODO
    if(!inJourney)
    {
        PrintJourneyIsAlreadFinished();
    }
    cachedPaths.ClearTable();
    startingLoc="";
    destination="";
    currentLoc="";
    inJourney=false;
    
}

void METUMaps::UpdateLocation(const std::string& name)
{
    // TODO
    if(!inJourney)
    {
        PrintNotInJourney();
        return;
    }

    if(name==destination)
    {
        PrintJourneyCompleted();
        return;
    }

    try {
        std::vector<int> currentPath;
        map.ShortestPath(currentPath,name,destination);
        }
    catch(VertexNotFoundException()){
        PrintLocationNotFound();
        return;
    }

    std::vector<int> outVector;
    cachedPaths.Find(outVector,GenerateKey(name,destination));

    if(outVector.size()==0){
        PrintCalculatingRoutes(name,destination);

        std::vector<int> currentPath;
        std::vector<int> subPath;

        map.ShortestPath(currentPath,name,destination);
        
        for(int j=0; j<currentPath.size(); j++)
        {
            for(int k=j;k<currentPath.size()-j;k++)
            {
                subPath.push_back(currentPath[k]);
            }
            if(subPath[0]!=subPath[subPath.size()-1])
            {
                cachedPaths.Insert(GenerateKey(map.VertexName(subPath[0]),map.VertexName(subPath.size()-1)),subPath);
            }
        }

    }
        
    else{
        PrintCachedLocationFound(name,destination);
        currentRoute=outVector;
    }
}

void METUMaps::Display()
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    if(!inJourney)
    {
        // We are not in journey, this function
        // shouldn't be called
        PrintNotInJourney();
        return;
    }

    int timeLeft = map.TotalWeightInBetween(currentRoute);

    // Print last known location
    std::cout << "Journey         : " << startingLoc << "->"
              << destination << "\n";
    std::cout << "Current Location: " << currentLoc << "\n";
    std::cout << "Time Left       : " << timeLeft << " minutes\n";
    std::cout << "Current Route   : ";
    map.PrintPath(currentRoute, true);

    std::cout.flush();
}