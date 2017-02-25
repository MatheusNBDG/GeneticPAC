#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <random>

#include "network.h"
#include "sfml.h"
#include "windows.h"

/*
-------------------------------------------- ********* ------------------------------------------------

    This is a simple application of a genetic algorithm for artificial intelligence in games.
    To this work is needed to run this code aside of the pacman doodle of google.
    I have programed this in a 1366x678 screen, and some steps maybe are screen-dependent
    (but I think than this will run).

    For compile this do you will need the Eigen and Sfml library for C++.

    Eventually I should revise this program too. Thank you.

-------------------------------------------- ********* ------------------------------------------------
*/

using namespace std;
using namespace Eigen;

typedef Matrix<double, Dynamic, Dynamic> MatrixXd;
typedef Matrix<double, Dynamic, 1> VectorXd;


void open_to_main(sf::RenderWindow &_window, const size_t &_i, vector<Genome> &_population)
{
    bool control_S=0, control_A=0, control_L=0;
    while (TRUE)
    {
        sf::Event event;
        while (_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                _window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            print_actual_genome(_population[_i],_i);
            control_S=1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            for(size_t i=0; i!=_population.size();i++)
                print_actual_genome(_population[i],i);
                control_A=1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)){
            load_genome(_population);
            control_L=1;
        }
        _window.clear(sf::Color::Black);
        main_interface(_window, control_S,control_A,control_L);
        _window.display();
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::C))
            break;
    }
}

vector<vector<int>> velocity_f(vector<vector<Ambient>> &_amb, pos &_pacman, vector<pos> &_store_pos, sf::RenderWindow &_window)
{
    vector<vector<Ambient>> amb(_amb.size());
    size_t control=0;
    double screen_width=(::GetSystemMetrics( SM_CXSCREEN ));
    double screen_height=(::GetSystemMetrics( SM_CYSCREEN ));
    sf::Event event;
    while(TRUE){
        while(TRUE){
            if(control==400){
                size_t close = 4;
                press_arrow(close);
                Sleep(100);
                move_cusor(_store_pos[0].x, _store_pos[0].y);
                left_click();
                control=0;
            }
            amb=scan_ambient(_pacman,_store_pos,screen_width,screen_height);
            control++;
            if(_pacman.x!=0) break;
            while (_window.pollEvent(event))
                if (event.type == sf::Event::Closed)
                    _window.close();
            }
        _amb=scan_ambient(_pacman,_store_pos,screen_width,screen_height);
        if(_pacman.x==0) control++;
        else break;
        while (_window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                _window.close();
    }
    vector<vector<int>> velocity(amb.size());
    for(size_t i=0;i!=_amb.size();i++){
        vector<int> velocity_mem(_amb[i].size());
        for(size_t j=0; j<_amb[i].size();j++){
            velocity_mem[j]=_amb[i][j].d-amb[i][j].d;
            if(abs(velocity_mem[j])>5) velocity_mem[j]=velocity_mem[j]/abs(velocity_mem[j]);
        }
        velocity[i]=velocity_mem;  // pixels / 2 seconds
    }
    return velocity;
}

void train_network(const size_t &_initial_num_genomes, const size_t &_num_genomes)
{
    //Initialization
    Network net;
    vector<pos> store_pos;
    size_t max_score=0;
    pos pacman(0,0);
    double screen_width=(::GetSystemMetrics( SM_CXSCREEN ));
    double screen_height=(::GetSystemMetrics( SM_CYSCREEN ));
    store_pos=initialization(screen_width,screen_height);
    store_pos.push_back(pacman); // score
    vector<vector<Ambient>> ambient;
    ambient=scan_ambient(pacman,store_pos,screen_width,screen_height);
    vector<Genome> population;
    random_device rd;
    mt19937 mt(rd());
    for(size_t i=0;i!=_initial_num_genomes; i++){
        Genome gen(mt);
        population.push_back(gen);
    }
    //Screen initialization
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(683, 768), "SFML shapes", sf::Style::Default,settings);
    window.setPosition(sf::Vector2i(675, 0));
    int generation=1;
    bool control_main=1;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if(control_main){
            open_to_main(window,0,population);
            control_main=0;
        }
        vector<unsigned int> score_mem(population.size(),0);
        for(size_t i=0;i!=population.size(); i++){
            size_t control=0;
            while(TRUE){
                window.clear(sf::Color::Black);
                print_header(window);
                score_mem[i]=store_pos.back().x;
                vector<vector<int>> velocity=velocity_f(ambient, pacman, store_pos, window);
                if(score_mem[i]>store_pos.back().x) break;
                press_arrow(net.action(ambient, pacman, velocity, store_pos, population[i]));
                genome_info(window,population.size(),i,store_pos.back().x,max_score ,80,generation); // i need to fix this
                direction_printer(window,200,ambient,velocity);
                press_arrow(5); // conterbug....
                population[i].score=store_pos.back().x;
                if(population[i].score>max_score) max_score=population[i].score;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
                    control_main=1;
                if(abs(pacman.x-store_pos[2].x)>27 || abs(pacman.y-store_pos[2].y)>40)
                    control=0;
                if(abs(pacman.x-store_pos[2].x)<27 && abs(pacman.y-store_pos[2].y)<40){
                    control++;
                    if(control==150){
                        size_t close = 4;
                        press_arrow(close);
                        control=0;
                    }
                }
                if(control_main){
                    open_to_main(window,i,population);
                    control_main=0;
                }
                while (window.pollEvent(event)){
                    if (event.type == sf::Event::Closed)
                    window.close();
                }
                window.display();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
                control_main=1;
            population[i].score=score_mem[i];
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
            control_main=1;
        natural_selection(population, _num_genomes,_num_genomes, mt);
        max_score=*max_element(score_mem.begin(),score_mem.end());
        cout << " Max Score: " << max_score << endl;
        generation++;
    }
}

int main()
{

    cout << "This is a prototype of a GA for pac-mam!"<< endl;
    cout << "Put the number of genomes than you want train together per generation and the initial number of genomes: ";
    size_t num_genomes=0, initial_num=0;
    cin>> num_genomes >> initial_num;
    train_network(initial_num,num_genomes);
    return 0;
}
