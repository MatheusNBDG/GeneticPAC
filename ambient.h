#ifndef AMBIENT_H_INCLUDED
#define AMBIENT_H_INCLUDED

using namespace std;

class Ambient
{
public:
    Ambient():
        d(0), d_2(0), d_3(0),red(0),green(0),blue(0),reward(0), reward_2(0), reward_3(0) {};
    Ambient(const size_t &_dis, const size_t &_red,const size_t &_green,const size_t &_blue,const size_t &_reward):
        d(_dis), d_2(0), d_3(0),red(_red),green(_green),blue(_blue),reward(_reward), reward_2(0), reward_3(0)  {}
    Ambient &operator=(const Ambient &_ambient)
    {
        d=_ambient.d;
        d_2=_ambient.d_2;
        d_3=_ambient.d_3;
        red=_ambient.red;
        green=_ambient.green;
        blue=_ambient.blue;
        reward=_ambient.reward;
        reward=_ambient.reward_2;
        reward=_ambient.reward_3;
    }
    Ambient add_distance(const size_t &_distance) { d+=_distance;}
    Ambient add_reward(const size_t &_reward) { reward+=_reward;}
    size_t d;
    size_t d_2;
    size_t d_3;
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    size_t reward;
    size_t reward_2;
    size_t reward_3;
private:
};



class pos
{
public:
    pos();
    pos(const size_t &_x, const size_t &_y) :
        x(_x),y(_y) {}
    pos &operator=(const pos &_pos)
     {
         x=_pos.x;
         y=_pos.y;
     }
    int x;
    int y;
private:
};

#endif // AMBIENT_H_INCLUDED
