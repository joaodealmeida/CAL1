//
//  main.cpp
//  CAL Project One
//
//  MIEIC 2013/2014
//  Eduardo Almeida, Joao Almeida and Joao de Figueiredo
//

#include <iostream>
#include <map>

#include "Graph.h"

#include "Coordinate.h"
#include "City.h"

#include "Reader.h"

#include "Additions.h"

#include "graphviewer.h"

void _methodOneSetupVertex(Vertex<City> *vertex, double maxDistance) {
    //
    //  Fill the vertex object with information about how many cities
    //  it can serve (within a maximum distance) if a health care center
    //  is built on it.
    //
    
    unsigned int viableCount = 0;
    
    vector<Edge<City>> edges = vertex -> getAdj();
    
    for (auto it2 = edges.begin(); it2 != edges.end(); ++it2)
        if ((* it2).getWeight() <= maxDistance)
            viableCount++;
    
    City cityInfo = vertex -> getInfo();
    
    cityInfo.setHealthCenterViableCount(viableCount);
    
    vertex -> setInfo(cityInfo);
}

int _fillByOrder(std::list<Vertex<City> *> orderedVertices, double maxDistance = NOT_CALCULATED, int maxHospitals = NOT_CALCULATED) {
    int insertedHospitals = 0;
    
    for (auto it = orderedVertices.begin(); it != orderedVertices.end(); ++it) {
        if (! ((* it) -> getInfo().getNeedsHealthCenter()) )
            continue;
        
        if (maxHospitals != NOT_CALCULATED) {
            if (!maxHospitals)
                break;
            
            maxHospitals--;
        }
        
        City cityInfo = (* it) -> getInfo();
        
        cityInfo.setContainsHealthCenter(true);
        cityInfo.setNeedsHealthCenter(false);
        
        insertedHospitals++;
        
        (* it) -> setInfo(cityInfo);
        
        vector<Edge<City>> adj = (* it) -> getAdj();
        
        for (auto it2 = adj.begin(); it2 != adj.end(); ++it2) {
            Vertex<City> *dest = (* it2).getDest();
            
            City newCityInfo = dest -> getInfo();
        }
        
        for (auto it2 = adj.begin(); it2 != adj.end(); ++it2) {
            Vertex<City> *dest = (* it2).getDest();
            
            City newCityInfo = dest -> getInfo();
            
            if (!newCityInfo.distanceTo(cityInfo) || maxDistance == NOT_CALCULATED || newCityInfo.distanceTo(cityInfo) > maxDistance)
                continue;
            
            newCityInfo.setNeedsHealthCenter(false);
            
            dest -> setInfo(newCityInfo);
        }
    }
    
    return insertedHospitals;
}

int fillWithHealthCentersMethodOne(Graph<City> *cityGraph, double maxDistance) {
    //
    //  Construct Graph's Edges
    //
    
    std::vector<Vertex<City> *> vertices = cityGraph -> getVertexSet();
    
    std::list<Vertex<City> *> orderedVertices;
    
    for (int i = 0; i < vertices.size(); i++) {
        Vertex<City> *vertex = vertices[i];
        
        for (int j = i + 1; j < vertices.size(); j++) {
            Vertex<City> *vertex2 = vertices[j];
            
            double edgeWidth = vertex -> getInfo().distanceTo(vertex2 -> getInfo());
            
            cityGraph -> addEdge(vertex -> getInfo(), vertex2 -> getInfo(), edgeWidth);
        }
        
        //
        //  Let's do it this way:
        //
        //  Let's maintain an ordered list (by edge count) of the vertices...
        //  And start filling! (but after. not now.)
        //
        
        _methodOneSetupVertex(vertex, maxDistance);
        
        if (!orderedVertices.size())
            orderedVertices.insert(orderedVertices.begin(), vertex);
        else {
            bool placed = false;
            
            for (auto it = orderedVertices.begin(); it != orderedVertices.end(); ++it) {
                if (vertex -> getInfo().getHealthCareViableCount() > (* it) -> getInfo().getHealthCareViableCount()) {
                    orderedVertices.insert(it, vertex);
                    
                    placed = true;
                    
                    break;
                }
            }
            
            if (!placed)
                orderedVertices.insert(orderedVertices.end(), vertex);
        }
    }
    
    //
    //  We should now have a graph and an ordered list of vertices.
    //
    
    return _fillByOrder(orderedVertices, maxDistance);
}

int fillWithHealthCentersMethodTwo(Graph<City> *cityGraph, int healthCenterCount) {
    //
    //  Construct Graph's Edges
    //
    
    std::vector<Vertex<City> *> vertices = cityGraph -> getVertexSet();
    
    std::list<Vertex<City> *> orderedVertices;
    
    for (int i = 0; i < vertices.size(); i++) {
        Vertex<City> *vertex = vertices[i];
        
        int populationServed = vertex -> getInfo().getPopulation();
        
        for (int j = 0; j < vertex -> getAdj().size(); j++)
            populationServed += vertex -> getAdj()[j].getDest() -> getInfo().getPopulation();
        
        City info = vertex -> getInfo();
        
        info.setMaxPopulationServed(populationServed);
        
        vertex -> setInfo(info);
        
        for (int j = i; j < vertices.size(); j++) {
            Vertex<City> *vertex2 = vertices[j];
            
            double edgeWidth = vertex -> getInfo().distanceTo(vertex2 -> getInfo());
            
            cityGraph -> addEdge(vertex -> getInfo(), vertex2 -> getInfo(), edgeWidth);
        }
    }
    
    for (int i = 0; i < vertices.size(); i++) {
        Vertex<City> *vertex = vertices[i];
        
        if (!orderedVertices.size())
            orderedVertices.insert(orderedVertices.begin(), vertex);
        else {
            bool placed = false;
            
            for (auto it = orderedVertices.begin(); it != orderedVertices.end(); ++it) {
                if (vertex -> getInfo().getMaxPopulationServed() > (* it) -> getInfo().getMaxPopulationServed()) {
                    orderedVertices.insert(it, vertex);
                    
                    placed = true;
                    
                    break;
                }
            }
            
            if (!placed)
                orderedVertices.insert(orderedVertices.end(), vertex);
        }
    }
    
    //
    //  We should now have a graph and an ordered list of vertices.
    //
    
    return _fillByOrder(orderedVertices, NOT_CALCULATED, healthCenterCount);
}

int main(int argc, const char * argv[]) {
    bool validPath = false;
    
    Reader *rd = nullptr;
    
    while (!validPath) {
        try {
            std::string path;
            std::cout << "Path to cities file: ";
            std::cin >> path;
            
            rd = new Reader(path);
            
            validPath = true;
        } catch (const char *exc) {
            std::cout << "Can't open file. Please retry." << std::endl;
        }
    }
    
    int method = 0;
    
    while (method != 1 && method != 2) {
        std::cout << "Method Number (1/2): ";
        
        std::cin >> method;
        
        if (method != 1 && method != 2)
            std::cout << "Invalid method number. Please retry." << std::endl;
    }
    
    float arg = 0.0f;
    
    while (!arg) {
        string arg_h;
        
        if (method == 1)
            std::cout << "Maximum Distance between Health Centers: ";
        else
            std::cout << "Number of Health Centers: ";
        
        std::cin >> arg_h;
        
        if (!Additions::checkForOnlyNumeric(arg_h))
            std::cout << "You may only use numbers and the decimal character." << std::endl;
        else
            arg = atof(arg_h.c_str());
    }
    
    std::cout << "Starting Job..." << std::endl;
    
    uint64_t init_time = Additions::getTimeMS64();
    
    Graph<City> cityGraph = rd -> generateGraph();
    
    if (method == 1)
        std::cout << "[Method One Result] Minimum Health Centers: " << fillWithHealthCentersMethodOne(&cityGraph, arg) << std::endl;
    else
        std::cout << "[Method Two Result] Placed Health Centers: " << fillWithHealthCentersMethodTwo(&cityGraph, (int) arg) << std::endl;
    
    std::cout << "Done. Time Elapsed: " << Additions::getTimeMS64() - init_time << " ms." << std::endl;
    
    std::cout << "Starting GraphViewer..." << std::endl;
    
    GraphViewer *gv = new GraphViewer(600, 600, true);
    
    gv -> createWindow(600, 600);
    
    gv -> defineVertexColor("black");
    
    std::map<Vertex<City> *, int> vertexToId;
    
    for (int i = 0; i < cityGraph.getVertexSet().size(); i++) {
        City city = cityGraph.getVertexSet()[i]->getInfo();
        
        gv -> addNode(i, city.getCoordinates().getLatitude(), city.getCoordinates().getLongitude());
        
        if (city.getContainsHealthCenter())
            gv -> setVertexColor(i, "green");
        
        gv -> setVertexLabel(i, city.getName());
        
        vertexToId.insert(std::pair<Vertex<City> *, int>(cityGraph.getVertexSet()[i], i));
        
        gv -> rearrange();
    }
    
    for (int i = 0; i < cityGraph.getVertexSet().size(); i++) {
        Vertex<City> *vertex = cityGraph.getVertexSet()[i];
        
        int vertexId = vertexToId[vertex];
        
        std::vector<Vertex<City> *> passed;
        
        for (int j = i; j < vertex -> getAdj().size(); j++) {
            Edge<City> edge = vertex -> getAdj()[j];
            
            int id = i * 1000 + j;
            
            int vertexId2 = vertexToId[edge.getDest()];
            
            double weight = vertex -> getAdj()[j].getWeight();
            
            if (!weight || vertexId == vertexId2 || std::find(passed.begin(), passed.end(), edge.getDest()) != passed.end())
                continue;
            
            passed.insert(passed.end(), edge.getDest());
            
            gv -> addEdge(id, vertexId, vertexId2, EdgeType::UNDIRECTED);
            gv -> setEdgeWeight(id, weight * 1000);
            
            std::cout << "[Graph Viewer Debug] Adding edge from " << vertexId << " to " << vertexId2 << " with weight " << weight << "..." << std::endl;
            
            gv -> rearrange();
        }
    }
    
    return EXIT_SUCCESS;
}
