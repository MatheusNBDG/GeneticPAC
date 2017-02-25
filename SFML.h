#ifndef SFML_H_INCLUDED
#define SFML_H_INCLUDED

#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <cstdlib>
#include <cmath>


using namespace std;

void print_header(sf::RenderWindow &_window)
{
        sf::Text text;

        sf::Font font;
        font.loadFromFile("sansation.ttf");
        text.setFont(font);

        text.setString("PAC-MAN Google Program: ");
        text.setPosition(10, 10);
        text.setCharacterSize(24);
        text.setColor(sf::Color::Yellow);
        _window.draw(text);

        text.setString("This is a Genetic Algorithm approximation of the problem. \n First put the number of genomes in each generation on console. ");
        text.setPosition(10, 40);
        text.setCharacterSize(16);
        text.setColor(sf::Color::Yellow);
        _window.draw(text);

        text.setString("Press G to get out");
        text.setPosition(510, 5);
        text.setCharacterSize(15);
        text.setColor(sf::Color::Red);
        _window.draw(text);

        text.setString("Press G to return to the main interface.");
        text.setPosition(155, 700);
        text.setCharacterSize(15);
        text.setColor(sf::Color::Yellow);
        _window.draw(text);

}

void genome_info(sf::RenderWindow &_window, const size_t &_num_genomes, const size_t &_genome,
                 const double &_score, const size_t _best_score, const size_t _height, const size_t &_generation)
{
    sf::Text text;
    sf::Font font;
    font.loadFromFile("sansation.ttf");
    text.setFont(font);

    text.setString(" Working with genome: ");
    text.setPosition( 10, _height);
    text.setCharacterSize(20);
    text.setColor(sf::Color::Magenta);
    _window.draw(text);

    stringstream iss;
    iss<<_genome+1;
    iss<<"/";
    iss<< _num_genomes;
    text.setString(iss.str());
    text.setPosition( 91, _height+30);
    text.setCharacterSize(20);
    text.setColor(sf::Color::Magenta);
    _window.draw(text);

    std::stringstream iss2;
    iss2<<_score;
    iss2<<"/";
    iss2<<_best_score;
    text.setString(" Actual score: "+iss2.str());
    text.setPosition( 10, _height+60);
    text.setCharacterSize(20);
    text.setColor(sf::Color::Magenta);
    _window.draw(text);

    std::stringstream iss4;
    iss4<<_generation;
    text.setString(" Generation: "+iss4.str());
    text.setPosition( 10, _height+90);
    text.setCharacterSize(20);
    text.setColor(sf::Color::Magenta);
    _window.draw(text);

    if(_score<_best_score){
        sf::RectangleShape rectangle(sf::Vector2f(_score/_best_score*400, 110));
        rectangle.setFillColor(sf::Color(255, 127, 80));
        rectangle.setPosition(260, _height);
        _window.draw(rectangle);
    }
    else{
        sf::RectangleShape rectangle(sf::Vector2f(400, 110));
        rectangle.setFillColor(sf::Color(0, 255, 0));
        rectangle.setPosition(260, _height);
        _window.draw(rectangle);
    }
}

void print_bar(sf::RenderWindow &_window, const size_t _height, const int &_c, const std::string _string)
{
        sf::Text text;

        sf::Font font;
        font.loadFromFile("sansation.ttf");
        text.setFont(font);

        stringstream iss;
        iss<<_c;
        text.setString("Current value of variable "+_string+" :"+iss.str());
        text.setPosition(10, _height);
        text.setCharacterSize(16);
        text.setColor(sf::Color::Blue);
        _window.draw(text);

        sf::RectangleShape rectangle(sf::Vector2f(_c/3, 16));
        rectangle.setFillColor(sf::Color(100, 250, 50));
        rectangle.setPosition(273, _height);
        _window.draw(rectangle);

        text.setStyle(sf::Text::Bold | sf::Text::Underlined);
}

void direction_printer(sf::RenderWindow &_window, const size_t _height,
                       const vector<vector<Ambient>> &_ambiente, const vector<vector<int>> &_velocity)
{
    sf::Text text;

    sf::Font font;
    font.loadFromFile("sansation.ttf");
    text.setFont(font);

    for(size_t i=0; i!=4;i++){
        std::string direction;
        if(i==0) direction=" upper ";
        if(i==3) direction=" down ";
        if(i==2) direction=" right ";
        if(i==1) direction=" left ";
        stringstream iss0;
        iss0<< _ambiente[i].size();
        text.setString("The input gained by "+direction+" direction with "+iss0.str()+" number of possibility of scape is: ");
        text.setPosition(10, _height+i*100);
        text.setCharacterSize(16);
        text.setColor(sf::Color::Yellow);
        _window.draw(text);
        print_bar(_window, _height+i*100+18 ,_ambiente[i][0].d, "distance");

        text.setString("Color of: ");
        text.setPosition(10, _height+i*100+2*18);
        text.setCharacterSize(16);
        text.setColor(sf::Color::Blue);
        _window.draw(text);

        for(size_t k=0; k!=_ambiente[i].size();k++){
            sf::RectangleShape rectangle(sf::Vector2f(50, 16));
            rectangle.setFillColor(sf::Color(_ambiente[i][k].red, _ambiente[i][k].green, _ambiente[i][k].blue));
            rectangle.setPosition(180+70*k, _height+i*100+2*18);
            _window.draw(rectangle);
        }


        print_bar(_window, _height+i*100+3*18 ,_ambiente[i][0].reward, "reward");

        stringstream iss;
        iss<<_velocity[i][0];
        text.setString("Current value of variable velocity :"+iss.str());
        text.setPosition(10, _height+i*100+4*18);
        text.setCharacterSize(16);
        text.setColor(sf::Color::Blue);
        _window.draw(text);


        sf::RectangleShape rectangle2(sf::Vector2f(_velocity[i][0], 16));
        if(_velocity[i][0]>=0) rectangle2.setFillColor(sf::Color(0,0,255));
        else rectangle2.setFillColor(sf::Color(255,0,0));
        rectangle2.setPosition(480, _height+i*100+4*18);
        _window.draw(rectangle2);
    }
}

void main_interface(sf::RenderWindow &_window, const bool _control_S, const bool _control_A, const bool _control_L)
{
    sf::Text text;

    sf::Font font;
    font.loadFromFile("sansation.ttf");
    text.setFont(font);

    text.setString("Press C to continue. ");
    text.setPosition(300,300);
    text.setCharacterSize(16);
    text.setColor(sf::Color::Yellow);
    _window.draw(text);

    if(_control_S==1) text.setString("Press S to save the current Genome. SAVED");
    else text.setString("Press S to save the current Genome.");
    text.setPosition(300, 400);
    text.setCharacterSize(15);
    text.setColor(sf::Color::Yellow);
    _window.draw(text);

    if(_control_A==1) text.setString("Press A to save the all Genomes. SAVED");
    else text.setString("Press A to save the all Genomes.");
    text.setPosition(300, 500);
    text.setCharacterSize(15);
    text.setColor(sf::Color::Yellow);
    _window.draw(text);

    if(_control_L==1) text.setString("Press L to loading the genomes. LOAD");
    else text.setString("Press L to loading the genomes.");
    text.setPosition(300,600);
    text.setCharacterSize(16);
    text.setColor(sf::Color::Blue);
    _window.draw(text);

}





#endif // SFML_H_INCLUDED
