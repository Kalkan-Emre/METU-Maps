#include "Graph.h"
#include "GraphExceptions.h"

#include <iostream>
#include <iomanip>
#include <queue>
#include <fstream>
#include <sstream>
#include <cstdlib>

// Literally do nothing here
// default constructors of the std::vector is enough
Graph::Graph()
{}

Graph::Graph(const std::string& filePath)
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    // Tokens
    std::string tokens[3];

    std::ifstream mapFile(filePath.c_str());
    // Read line by line
    std::string line;
    while (std::getline(mapFile, line))
    {
        // Empty Line Skip
        if(line.empty()) continue;
        // Comment Skip
        if(line[0] == '#') continue;

        // Tokenize the line
        int i = 0;
        std::istringstream stream(line);
        while(stream >> tokens[i]) i++;

        // Single token (Meaning it is a vertex)
        if(i == 1)
        {
            InsertVertex(tokens[0]);
        }
        // Exactly three tokens (Meaning it is an edge)
        else if(i == 3)
        {
            int weight = std::atoi(tokens[0].c_str());
            if(!ConnectVertices(tokens[1], tokens[2], weight))
            {
                std::cerr << "Duplicate edge on "
                          << tokens[0] << "-"
                          << tokens[1] << std::endl;
            }
        }
        else std::cerr << "Token Size Mismatch" << std::endl;
    }
}

void Graph::InsertVertex(const std::string& vertexName)
{
    // TODO
    
    int i;
    
    for(i=0;i<(vertexList.size());i++)
    {
        if(vertexList[i].name==vertexName)
        {
            throw DuplicateVertexNameException();
            return;
        }
    }
    
    GraphVertex new_vertex;
    new_vertex.edgeCount = 0;
    new_vertex.name = vertexName;
    vertexList.push_back(new_vertex);
    
}

bool Graph::ConnectVertices(const std::string& fromVertexName,
                            const std::string& toVertexName,
                            int weight)
{
    // TODO
    
    int i;
    GraphVertex fromVertex;
    GraphVertex toVertex;
    bool isSetted1 = false;
    bool isSetted2 = false;
    int toVertexid = -1;
    int fromVertexid = -1;
    for(i=0;i<(vertexList.size());i++)
    {
        if(vertexList[i].name==fromVertexName)
        {
            fromVertex = vertexList[i];
            isSetted1 = true;
            fromVertexid = i;
        }
        
        if(vertexList[i].name == toVertexName)
        {
            toVertex = vertexList[i];
            isSetted2 = true;
            toVertexid = i;
        }
    }
    if(isSetted2 && isSetted1)
    {
        if(toVertexid==fromVertexid)
            return false;
            
        for(i=0;i<(edgeList.size());i++)
        {
            if(edgeList[i].vertexId0 == toVertexid && edgeList[i].vertexId1==fromVertexid)
            {
                return false;
            }
            if(edgeList[i].vertexId1 == toVertexid && edgeList[i].vertexId0==fromVertexid)
            {
                return false;
            }
        }
        
        if((fromVertex.edgeCount >= MAX_EDGE_PER_VERTEX) || (toVertex.edgeCount >= MAX_EDGE_PER_VERTEX))
        {
            throw TooManyEdgeOnVertexExecption();
        }
        else{
            GraphEdge newEdge;
            newEdge.weight = weight;
            newEdge.vertexId0 = fromVertexid;
            newEdge.vertexId1 = toVertexid;
            newEdge.masked = false;
            edgeList.push_back(newEdge);
            vertexList[fromVertexid].edgeCount++;
            vertexList[toVertexid].edgeCount++;
            vertexList[fromVertexid].edgeIds[vertexList[fromVertexid].edgeCount-1]=edgeList.size()-1;
            vertexList[toVertexid].edgeIds[vertexList[toVertexid].edgeCount-1]=edgeList.size()-1;
            return true;
        }
    }
    else{
        throw VertexNotFoundException();
    }
    
}

bool Graph::ShortestPath(std::vector<int>& orderedVertexIdList,
                         const std::string& from,
                         const std::string& to) const
{
    // TODO
    int i;
    bool isSetted1 = false;
    bool isSetted2 = false;
    int toVertexid = -1;
    int fromVertexid = -1;
    for(i=0;i<(vertexList.size());i++)
    {
        if(vertexList[i].name==from)
        {
            isSetted1 = true;
            fromVertexid = i;
        }
        
        if(vertexList[i].name == to)
        {
            isSetted2 = true;
            toVertexid = i;
        }
    }
    if(isSetted1&&isSetted2) 
    {
        std::vector<int> path; //Keeps previous nodes
        std::vector<DistanceVertexIdPair> weights; //Keeps total weights to each node from starting node

        // Initialize vectors declared above
        for(int i=0; i<vertexList.size();i++)
        {
            weights.push_back(DistanceVertexIdPair(i,LARGE_NUMBER));
            path.push_back(INVALID_INDEX);
        }

        weights[fromVertexid].currentWeight=0;
        std::priority_queue<DistanceVertexIdPair> pq;
        DistanceVertexIdPair curr(fromVertexid,0);
        pq.push(curr);
        while(!pq.empty())
        {
            curr = pq.top();
            pq.pop();
            GraphVertex currenVertex = vertexList[curr.vId];
            
            for(int i=0; i<currenVertex.edgeCount; i++)
            {
                int newId = (edgeList[currenVertex.edgeIds[i]].vertexId0 != curr.vId) ? edgeList[currenVertex.edgeIds[i]].vertexId0 : edgeList[currenVertex.edgeIds[i]].vertexId1;
                if(!edgeList[currenVertex.edgeIds[i]].masked)
                {
                    if((weights[curr.vId].currentWeight+edgeList[currenVertex.edgeIds[i]].weight)<weights[newId].currentWeight)
                    {
                        weights[newId].currentWeight = (weights[curr.vId].currentWeight+edgeList[currenVertex.edgeIds[i]].weight);
                        path[newId]=curr.vId;
                        pq.push(weights[newId]);
                    }
                }
                else
                    continue;
            }
            
        }
        std::vector<int> result;
        if(path[toVertexid]==-1)
        {
            orderedVertexIdList = result;
            return false;
        }
        else{
            
            int iterator=toVertexid;
            result.push_back(iterator);
            while(iterator!=fromVertexid)
            {
                result.push_back(path[iterator]);
                iterator=path[iterator];
            }
            
            for(int i=result.size()-1;i>=0;i--)
            {
                orderedVertexIdList.push_back(result[i]);
            }
            
        }

    }
    else{
        throw VertexNotFoundException();
    }
}



int Graph::MultipleShortPaths(std::vector<std::vector<int> >& orderedVertexIdList,
                              const std::string& from,
                              const std::string& to,
                              int numberOfShortestPaths)
{
    // TODO
    if(numberOfShortestPaths>0)
    {
        std::vector<int> result;
        UnMaskAllEdges();
        ShortestPath(result,from,to);
        orderedVertexIdList.push_back(result);
        numberOfShortestPaths--;
        while(numberOfShortestPaths>0)
        {
            int max = -1;
            int max_index = -1;
            for(int i=0; i<result.size()-1;i++)
            {
                int tmp = -1;
                int tmp_index = -1;
                for(int j=0; j<edgeList.size();j++)
                {
                    if((edgeList[j].vertexId0==result[i]&&edgeList[j].vertexId1==result[i+1])||
                        (edgeList[j].vertexId1==result[i]&&edgeList[j].vertexId0==result[i+1]))
                    {
                        tmp = edgeList[j].weight;
                        tmp_index = j;
                        break;
                    }
                }
                if(tmp>max)
                {
                    max_index = tmp_index;
                    max=tmp;
                }
                    
            }

            edgeList[max_index].masked = true;

            std::vector<int> new_result;
            ShortestPath(new_result,from,to);
            if(new_result==result)
            {
                break;
            }
            else{
                orderedVertexIdList.push_back(new_result);
                result = new_result;
            }
            numberOfShortestPaths--;
        }
        UnMaskAllEdges();
        return orderedVertexIdList.size();
    }
}

void Graph::MaskEdges(const std::vector<StringPair>& vertexNames)
{
    // TODO

    for(int i=0;i<vertexNames.size();i++)
    {
        bool isSetted0 = false;
        bool isSetted1 = false;
        int i0;
        int i1;
        for(int j=0;j<vertexList.size();j++)
        {
            if(vertexList[j].name == vertexNames[i].s0)
            {
                isSetted0 = true;
                i0 = j;
            } 
            if(vertexList[j].name ==vertexNames[i].s1)
            {
                isSetted1 = true;
                i1 = j;
            }
        }
        if(isSetted1&&isSetted0)
        {
            for(int k=0; k<edgeList.size();k++)
            {
                if((edgeList[k].vertexId0 == i0 && edgeList[k].vertexId1 == i1) || 
                   (edgeList[k].vertexId1 == i0 && edgeList[k].vertexId0 == i1) )
                    {
                        edgeList[k].masked = true;
                    }
            }
        }
        else{
            throw VertexNotFoundException();
        }
    }
}

void Graph::UnMaskEdges(const std::vector<StringPair>& vertexNames)
{
    // TODO

    for(int i=0;i<vertexNames.size();i++)
    {
        bool isSetted0 = false;
        bool isSetted1 = false;
        int i0;
        int i1;
        for(int j=0;j<vertexList.size();j++)
        {
            if(vertexList[j].name == vertexNames[i].s0)
            {
                isSetted0 = true;
                i0 = j;
            } 
            if(vertexList[j].name ==vertexNames[i].s1)
            {
                isSetted1 = true;
                i1 = j;
            }
        }
        if(isSetted1&&isSetted0)
        {
            for(int k=0; k<edgeList.size();k++)
            {
                if((edgeList[k].vertexId0 == i0 && edgeList[k].vertexId1 == i1) || 
                   (edgeList[k].vertexId1 == i0 && edgeList[k].vertexId0 == i1) )
                    {
                        edgeList[k].masked = false;
                    }
            }
        }
        else{
            throw VertexNotFoundException();
        }
    }
}

void Graph::UnMaskAllEdges()
{
    // TODO
    for(int i=0; i<edgeList.size();i++)
    {
        edgeList[i].masked = false;
    }
}

void Graph::MaskVertexEdges(const std::string& name)
{
    // TODO
    bool isThere = false;
    int index;
    for(int i=0; i<vertexList.size();i++)
    {
        if(vertexList[i].name==name)
        {
            isThere = true;
            index = i;
        }
            
    }
    if(isThere)
    {
        for(int j=0;j<vertexList[index].edgeCount;j++)
        {
            edgeList[vertexList[index].edgeIds[j]].masked = true;
        }
    }
    else{
        throw VertexNotFoundException();
    }
}

void Graph::UnMaskVertexEdges(const std::string& name)
{
    // TODO
    bool isThere = false;
    int index;
    for(int i=0; i<vertexList.size();i++)
    {
        if(vertexList[i].name==name)
        {
            isThere = true;
            index = i;
        }
            
    }
    if(isThere)
    {
        for(int j=0;j<vertexList[index].edgeCount;j++)
        {
            edgeList[vertexList[index].edgeIds[j]].masked = false;
        }
    }
    else{
        throw VertexNotFoundException();
    }
}

void Graph::ModifyEdge(const std::string& vName0,
                       const std::string& vName1,
                       float newWeight)
{
    // TODO
    
    int i;
    GraphVertex fromVertex;
    GraphVertex toVertex;
    bool isSetted1 = false;
    bool isSetted2 = false;
    int toVertexid = -1;
    int fromVertexid = -1;
    for(i=0;i<(vertexList.size());i++)
    {
        if(vertexList[i].name==vName0)
        {
            fromVertex = vertexList[i];
            isSetted1 = true;
            fromVertexid = i;
        }
        
        if(vertexList[i].name == vName1)
        {
            toVertex = vertexList[i];
            isSetted2 = true;
            toVertexid = i;
        }
    }
    if(isSetted2 && isSetted1)
    {
        for(int j=0; j<edgeList.size();j++)
        {
            if((edgeList[j].vertexId0==fromVertexid && edgeList[j].vertexId1==toVertexid)||(edgeList[j].vertexId1==fromVertexid && edgeList[j].vertexId0==toVertexid))
            {
                edgeList[j].weight=newWeight;
            }
        }
    }
    else{
        throw VertexNotFoundException();
    }
}

void Graph::ModifyEdge(int vId0, int vId1,
                       float newWeight)
{
    for(int j=0; j<edgeList.size();j++)
    {
        if((edgeList[j].vertexId0==vId0 && edgeList[j].vertexId1==vId1)||(edgeList[j].vertexId1==vId0 && edgeList[j].vertexId0==vId1))
        {
            edgeList[j].weight=newWeight;
        }
    }
}

void Graph::PrintAll() const
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    for(size_t i = 0; i < vertexList.size(); i++)
    {
        const GraphVertex& v = vertexList[i];
        std::cout << v.name << "\n";
        for(int j = 0; j < v.edgeCount; j++)
        {
            int edgeId = v.edgeIds[j];
            const GraphEdge& edge = edgeList[edgeId];
            // Skip printing this edge if it is masked
            if(edge.masked)
                continue;

            // List the all vertex names and weight
            std::cout << "-" << std::setfill('-')
                             << std::setw(2) << edge.weight
                             << "-> ";
            int neigVertexId = (static_cast<int>(i) == edge.vertexId0)
                                 ? edge.vertexId1
                                 : edge.vertexId0;
            std::cout << vertexList[neigVertexId].name << "\n";
        }
    }
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

void Graph::PrintPath(const std::vector<int>& orderedVertexIdList,
                      bool sameLine) const
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this file !       //
    // ============================= //
    for(size_t i = 0; i < orderedVertexIdList.size(); i++)
    {
        int vertexId = orderedVertexIdList[i];
        if(vertexId >= static_cast<int>(vertexList.size()))
            throw VertexNotFoundException();

        const GraphVertex& vertex = vertexList[vertexId];
        std::cout << vertex.name;
        if(!sameLine) std::cout << "\n";
        // Only find and print the weight if next is available
        if(i == orderedVertexIdList.size() - 1) break;
        int nextVertexId = orderedVertexIdList[i + 1];
        if(nextVertexId >= static_cast<int>(vertexList.size()))
            throw VertexNotFoundException();

        // Find the edge between these two vertices
        int edgeId = INVALID_INDEX;
        if(vertexId     < static_cast<int>(vertexList.size()) &&
           nextVertexId < static_cast<int>(vertexList.size()))
        {
            // Check all of the edges of vertex
            // and try to find
            const GraphVertex& fromVert = vertexList[vertexId];
            for(int i = 0; i < fromVert.edgeCount; i++)
            {
                int eId = fromVert.edgeIds[i];
                // Since the graph is not directional
                // check the both ends
                if((edgeList[eId].vertexId0 == vertexId &&
                    edgeList[eId].vertexId1 == nextVertexId)
                ||
                   (edgeList[eId].vertexId0 == nextVertexId &&
                    edgeList[eId].vertexId1 == vertexId))
                {
                    edgeId = eId;
                    break;
                }
            }
        }
        if(edgeId != INVALID_INDEX)
        {
            const GraphEdge& edge = edgeList[edgeId];
            std::cout << "-" << std::setfill('-')
                      << std::setw(2)
                      << edge.weight << "->";
        }
        else
        {
            std::cout << "-##-> ";
        }
    }
    // Print endline on the last vertex if same line is set
    if(sameLine) std::cout << "\n";
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

int Graph::TotalVertexCount() const
{
    // TODO
    return vertexList.size();
}

int Graph::TotalEdgeCount() const
{
    // TODO
    return edgeList.size();
}

std::string Graph::VertexName(int vertexId) const
{
    // TODO
    if(vertexId<0 || vertexId>=vertexList.size())
        return "";
    else
        return vertexList[vertexId].name;
}

int Graph::TotalWeightInBetween(std::vector<int>& orderedVertexIdList)
{
    // TODO
    int result=0;
    for(int i=0;i<orderedVertexIdList.size();i++)
    {
        if(orderedVertexIdList[i]<0||orderedVertexIdList[i]>=vertexList.size())
        {
            throw VertexNotFoundException();
        }
        else{
            if(i==orderedVertexIdList.size()-1)
            {
                return result;
            }
            else{
                bool isThere=false;
                for(int j=0; j<edgeList.size();j++)
                {
                    int index0 = orderedVertexIdList[i];
                    int index1 = orderedVertexIdList[i+1];
                    if((edgeList[j].vertexId0==index0 && edgeList[j].vertexId1==index1)||(edgeList[j].vertexId1==index0 && edgeList[j].vertexId0==index1))
                    {
                        isThere=true;
                        result+=edgeList[j].weight;
                        break;
                    }
                }
                if(!isThere)
                {
                    return -1;
                }
            }
        }
    }
    return result;
}
