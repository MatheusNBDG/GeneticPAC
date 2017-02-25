#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include "genome.h"
#include "ambient.h"
#include <vector>


using namespace std;
using namespace Eigen;



class Network
{
public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Network(): input(Matrix<double,5,1>::Zero()),
               hidden(Matrix<double,5,1>::Zero()),
               output(Matrix<double,1,1>::Zero()) {}
    void sigmoid(bool version);
    void feed_forward(const Genome &_genome);
    double probability_f(const size_t &_distance, const size_t &_reward, const Ambient &_amb, int _velocity, const Genome &_genome);
    size_t action(vector<vector<Ambient>> &_amb, pos &_pacman, vector<vector<int>> &_velocity,
                  vector<pos> _store_pos,const Genome &_genome);
    friend void train_network(const size_t &_initial_num_genomes, const size_t &_num_genomes);
    friend void print_actual_genome(const Genome &_genome, const size_t &_i);
private:
    VectorXd input;
    VectorXd hidden;
    VectorXd output;

};





#endif // NETWORK_H_INCLUDED
