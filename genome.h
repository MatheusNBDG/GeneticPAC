#ifndef GENOME_H_INCLUDED
#define GENOME_H_INCLUDED

#include <Eigen/Dense>
#include <random>

using namespace std;
using namespace Eigen;

class Network;



class Genome
{
public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Genome(): f_genome(Matrix<double,4,5>::Zero()),
              s_genome(Matrix<double,1,5>::Zero()),
              score(0)  {}
    Genome(mt19937 &_mt): f_genome(Matrix<double,4,5>::Zero()),
                          s_genome(Matrix<double,1,5>::Zero()),
                          score(0)
    {
        uniform_real_distribution<double> dist(-10.0, 10.0);
        for(size_t row=0; row!=f_genome.rows();row++)
            for(size_t col=0; col!=f_genome.cols(); col++)
                f_genome(row,col)=dist(_mt);
        for(size_t row=0; row!=s_genome.rows();row++)
            for(size_t col=0; col!=s_genome.cols(); col++)
                s_genome(row,col)=dist(_mt);
    }
    Genome(const Genome &_genome): f_genome(Matrix<double,5,5>::Random()),
                                   s_genome(Matrix<double,1,5>::Random()),
                                   score(0)
    {
        f_genome=_genome.f_genome;
        s_genome=_genome.s_genome;
        score=0;
    }
    Genome &operator=(const Genome &_genome)
    {
        f_genome=_genome.f_genome;
        s_genome=_genome.s_genome;
        score=0;
    }
    friend Genome cross_over(const Genome &_father, const Genome &_mother, mt19937 &_mt);
    friend void print_actual_genome(const Genome &_genome, const size_t &_i);
    friend void load_genome(vector<Genome> &_population);
    friend void natural_selection(vector<Genome> &_population, const size_t &_initial_num_genomes, const size_t &_num_genomes, mt19937 &_mt);
    friend Network;
    void mutate( mt19937 &_mt);
    unsigned int score;
private:
    MatrixXd f_genome;
    MatrixXd s_genome;

};





#endif // GENOME_H_INCLUDED
