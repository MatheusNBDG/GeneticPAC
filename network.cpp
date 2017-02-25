#include "network.h"
#include <cmath>
#include <windows.h>
#include "wtypes.h"
#include <ole2.h>
#include <olectl.h>
#include <iostream>

using namespace std;
using namespace Eigen;

int color_interpreter(const size_t &_red, const size_t &_green, const size_t &_blue)
{
    if(_red==0&&_green==51&&_blue==255) return 1;
    if(_red==222&&_green==151&&_blue==81) return 1;
    if(_red==255&&_green==153&&_blue==204) return -1;
    if(_red==0&&_green==0&&_blue==0) return -1;
    if(_red==255,_green==0,_blue==0) return -1;
    if(_red==255,_green==204,_blue==51) return -1;
    if(_red==51&&_green==255&&_blue==255) return -1;
    return 0;
}

void Network::sigmoid(bool version)
{
    if(version)
        for(size_t i=0;i!=hidden.size();i++)
            hidden[i]=1/(1+exp(-hidden[i]));
    else
        for(size_t i=0;i!=output.size();i++)
            output[i]=1/(1+exp(-output[i]));
}

void Network::feed_forward(const Genome &_genome)
{
    hidden.tail(hidden.size()-1)=_genome.f_genome*input;
    sigmoid(1);
    hidden[0]=1;
    output=_genome.s_genome*hidden;
    sigmoid(0);
}

class index
{
public:
    index(const double &_probability, const size_t &_idx):
        probability(_probability), idx(_idx) {}
    double probability;
    size_t idx;
private:

};

double Network::probability_f(const size_t &_distance, const size_t &_reward, const Ambient &_amb,
                              int _velocity, const Genome &_genome)
{
    input = VectorXd::Zero(5);
    input[0]=1;
    input[1]=(double) _distance;
    input[2]=400*color_interpreter(_amb.red,_amb.green,_amb.blue);
    input[3]=(double) _reward;
    input[4]=_velocity;
    feed_forward(_genome);
    return output[0];
}

unsigned int Network::action(vector<vector<Ambient>> &_amb, pos &_pacman, vector<vector<int>> &_velocity,
                             vector<pos> _store_pos,const Genome &_genome)
{
    index output_max(0,6);
    for(size_t i=0; i!=4; i++){
        vector<double> probability;
        for(size_t j=0;j!=_amb[i].size();j++){
            double mem=0;
            if(_amb[i][j].d_2==0&&_amb[i][j].d_3==0)
                mem=probability_f(_amb[i][j].d, _amb[i][j].reward, _amb[i][j], _velocity[i][j],_genome);
            else
                if(_amb[i][j].d_3==0){
                    Ambient mm;
                    mem=probability_f(_amb[i][j].d, _amb[i][j].reward, mm, 0,_genome)*probability_f(_amb[i][j].d_2, _amb[i][j].reward_2, _amb[i][j], _velocity[i][j],_genome);
                }
                else{
                    Ambient mm;
                    mem=probability_f(_amb[i][j].d, _amb[i][j].reward, mm, 0,_genome)*probability_f(_amb[i][j].d_2, _amb[i][j].reward_2, mm, 0,_genome)*probability_f(_amb[i][j].d_3, _amb[i][j].reward_3, _amb[i][j], _velocity[i][j],_genome);
                }
            probability.push_back(mem);
            if(mem>output_max.probability){
                    output_max.probability=mem;
                    output_max.idx=i;
            }
        }
    }
    return output_max.idx;
}

