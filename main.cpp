#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <chrono>

#include "octree.h"
#include "diffusion.h"

using namespace std;
const int windowWidth = 1200;
const int windowHeight = 600;

const sf::Color backgroundColor{0,0,0};

sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Graphical test");
sf::RectangleShape rect;
float rectSize = 7;


DiffusionParams params;
Diffusion *diff;
//OctreeDiffusion *octreeDiff;

std::ofstream log_file("Diffusion_with_octree.txt");


void init(){
    srand(time(0));
    params.T = 200;
    params.L = 75;
    params.D = 5;
    params.mu = 0.1;
    params.finalNumberCells = 25;

    params.speed = 10;
    params.divThreshold = 100;
    params.pathThreshold = 0.5;

    diff = new Diffusion(params);

    rect.setSize({rectSize,rectSize});

}

void release(){
    delete diff;
}


void calculate() {
    using namespace std::chrono;
    high_resolution_clock clock;

    auto begin = clock.now();
//    auto begin1 = clock.now();
//    auto end1 = clock.now();

    if(diff->cells_count < params.finalNumberCells) {

        diff->produceSubstances();

//        begin1 = clock.now();
        diff->runDiffusionStep();
//        end1 = clock.now();

        diff->runDecayStep();
//        diff->cellMovementAndDuplication();
    } else {
        diff->produceSubstances();
        diff->runDiffusionStep();
        diff->runDecayStep();
//        diff->runDiffusionClusterStep();
        for( int i = 0; i < diff->cells_count; i++){
            diff->cells[i].pos += diff->cells[i].currMov;
        }
    }
    for( int i = 0; i < diff->cells_count; i++) {
        Cell& currentCell = diff->cells[i];
        if (currentCell.pos < Point<float>{0,0,0}) currentCell.pos.x = 0;

        if (currentCell.pos.x > 1) currentCell.pos.x = 1;
        if (currentCell.pos.y > 1) currentCell.pos.y = 1;
        if (currentCell.pos.z > 1) currentCell.pos.z = 1;
    }


    auto elapsedTime = duration_cast<duration<double> >(clock.now() - begin);
//    auto elapsedTime1 = duration_cast<duration<double> >(end1 - begin1);
//    std::cout << "Calc took: " << elapsedTime.count() << std::endl;
//    std::cout << "Calc2 took: " << elapsedTime1.count() << std::endl;
    log_file << "Calc took: " << elapsedTime.count() << std::endl;
}

void draw() {
    window.clear(backgroundColor);

    for( auto& p: *diff->conc ) {
        int x = p.first.x;
        int y = p.first.y;
//        float a = (*p.second)[0];
        std::cout << (*p.second) << std::endl;
//        a *= 255;
//        a = 255;
//        if( a > 255) a = 255;
        rect.setPosition(300 + x*rectSize,y*rectSize);
        rect.setFillColor(sf::Color{(sf::Uint8) 255, (sf::Uint8) 255, (sf::Uint8) 255,255});
        window.draw(rect);
    }

    window.display();
}

void processEvents() {
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch(event.type){
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if( event.key.code == sf::Keyboard::Escape )
                    window.close();
                break;
            default:
                break;
        }
    }
}


int main() {


//    init();
//    while (window.isOpen())
//    {
//        processEvents();
//        calculate();
//        draw();
//    }
//    release();
//    return 0;

    using OctreeType = Octree< float >;
    OctreeType o({0,0,0}, {100,100,100},{1,1,1});

    o.set(20,30,80,4);
    o.set(60,20,80,1);


    o.set(25,23,83,3);
    o.set(40,20,40,5);
//
//
////    auto it = o.begin();
//
//
    for( auto& p : o){
        cout << "Leaf(" << p.first.x << ", " << p.first.y << ", " << p.first.z << "): {" << (*p.second) << '}' << endl;
    }
//
////    auto n = o.getRoot();
////    stack<OctreeType::Node*> stack1;
////
////    stack1.push(n);
////
////    while( !stack1.empty() ){
////        auto n = stack1.top();
////
////        stack1.pop();
////
//////        cout << n->type << endl;
////        if(  n->type == OctreeType::Leaf )
////            cout << "Leaf: {" << n->value_[0] << ", " <<  n->value_[1] << '}' << endl;
////        else {
////            for( auto c : n->child )
////                if(c!= nullptr)
////                    stack1.push(c);
////
////        }
////
////
////    }
//
////    Octree< std::array<float,2> > o1(o);
////    cout << o1(0,50,50)[1] << endl;
////
//////    while(  )
//////    auto it = o.getIterator();
//////    auto its = o.getAll();
//////    for( auto &l : o.getAll() ) {
//////        cout << l->value_ << endl;
//////
//////    }
////
//////    for( auto i : o.size.coords){
//////        cout << i << endl;
////    }
//
//    return 0;
}







