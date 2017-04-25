#include <iostream>
//#include <SFML/Graphics.hpp>
#include <fstream>
#include <chrono>
#include <vector>

#include "include/octree.h"
#include "include/diffusion.h"

using namespace std;
//const int windowWidth = 1200;
//const int windowHeight = 600;

//const sf::Color backgroundColor{0, 0, 0};

//sf::RenderWindow
//        window(sf::VideoMode(windowWidth, windowHeight), "Graphical test");
//sf::RectangleShape rect;
//float rectSize = 7;
//

DiffusionParams params;
Diffusion *diff;
//std::ofstream log_file("Diffusion_with_octree.txt");


void init() {
    srand((unsigned int) time(0));

    params.T = 200;
    params.L = 100;
    params.D = 0.9;
    params.mu = 0.1;
    params.finalNumberCells = 3;

    params.speed = 10;
    params.divThreshold = 100;
    params.pathThreshold = 0.5;

    diff = new Diffusion(params);
//
//    rect.setSize({rectSize, rectSize});

}

void release() {
    delete diff;
}


void calculate() {
//    using namespace std::chrono;
//    high_resolution_clock clock;
//
//    auto begin = clock.now();
////    auto begin1 = clock.now();
////    auto end1 = clock.now();
//
//    if (diff->cells_count < params.finalNumberCells) {
//
        diff->produceSubstances();
//
////        begin1 = clock.now();
        diff->runDiffusionStep();
//        end1 = clock.now();
//
//        diff->runDecayStep();
//        diff->cellMovementAndDuplication();
//    } else {
//        diff->produceSubstances();
//        diff->runDiffusionStep();
//        diff->runDecayStep();
//        diff->runDiffusionClusterStep();
//        for( int i = 0; i < diff->cells_count; i++){
//            diff->cells[i].pos += diff->cells[i].currMov;
//        }
//    }
//    for( int i = 0; i < diff->cells_count; i++) {
//        Cell& currentCell = diff->cells[i];
//        if (currentCell.pos < Point<float>{0,0,0}) currentCell.pos = {0,0,0};
//
//        if (currentCell.pos.x > 1) currentCell.pos.x = 1;
//        if (currentCell.pos.y > 1) currentCell.pos.y = 1;
//        if (currentCell.pos.z > 1) currentCell.pos.z = 1;
//    }
//
//
//    auto elapsedTime = duration_cast<duration<double> >(clock.now() - begin);
////    auto elapsedTime1 = duration_cast<duration<double> >(end1 - begin1);
////    std::cout << "Calc took: " << elapsedTime.count() << std::endl;
////    std::cout << "Calc2 took: " << elapsedTime1.count() << std::endl;
//    log_file << "Calc took: " << elapsedTime.count() << std::endl;
}

void draw() {
//    window.clear(backgroundColor);

//    const auto& conc = *diff->conc;
//
//    for (auto& p : conc) {
//        float a = p.second * 100000000;
//        if (a < 25) continue;
//        if (a > 255) a = 255;
//        auto x = p.first.x;
//        auto y = p.first.y;
//        rect.setPosition(300 + x * rectSize, y * rectSize);
//        rect.setFillColor(sf::Color(a, 0, 0, 64));
//        window.draw(rect);
//    }
//
//    window.display();
}

void processEvents() {
//    sf::Event event;
//    while (window.pollEvent(event))
//    {
//        switch(event.type){
//            case sf::Event::Closed:
//                window.close();
//                break;
//            case sf::Event::KeyPressed:
//                if( event.key.code == sf::Keyboard::Escape )
//                    window.close();
//                break;
//            default:
//                break;
//        }
//    }
}


int main() {
    init();

    using namespace std::chrono;
    high_resolution_clock clock;

    const auto& oc = *diff->conc; // const octree ref

    ofstream file("diff_time_nodes");
    vector<double> times;
    for (int i = 0; i < 25; i++) {
        auto begin = clock.now();
        calculate();
        auto end = clock.now();
        auto elapsedTime = duration_cast<duration<double> >(end - begin).count();
        times.push_back(elapsedTime);
        file << oc.size() << ' ';
        cout << "done: " << i*4  << ' ' << elapsedTime << endl;
    }

    file << endl;

    for (auto t : times)
        file << t << ' ';
    file << endl;

    file.close();

//    while (window.isOpen()) {
//        processEvents();
//        calculate();
//        draw();
//    }
    release();


    // 2 source
    // 4 picture
    // n / t

    return 0;
}







