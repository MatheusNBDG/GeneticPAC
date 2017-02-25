#include "genome.h"
#include <Eigen/Dense>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iterator>


using namespace std;
using namespace Eigen;

Genome cross_over(const Genome &_father, const Genome &_mother, mt19937 &_mt)
{
    uniform_int_distribution<int> f_dist(0,_father.f_genome.cols()/2);
    uniform_int_distribution<int> s_dist(1,_father.s_genome.cols()/2);
    bool choice=0;
    Genome son;
    size_t f_cross_col = _father.f_genome.cols()/2+f_dist(_mt)-1;
    size_t s_cross_col = (_father.s_genome.cols()/2)+s_dist(_mt)-1;
    //f_genome
    for(size_t col=0; col!=_father.f_genome.cols();col++){
        choice=0;
        if(col>=f_cross_col) choice=1;
        if(!choice) son.f_genome.col(col)=_father.f_genome.col(col);
        else son.f_genome.col(col)=_mother.f_genome.col(col);
    }
    //g_genome
    for(size_t col=0; col!=_father.s_genome.cols();col++){
        choice=0;
        if(col>=s_cross_col) choice=1;
        if(!choice) son.s_genome.col(col)=_father.s_genome.col(col);
        else son.s_genome.col(col)=_mother.s_genome.col(col);
    }
    return son;
}


void Genome::mutate(mt19937 &_mt)
{
    //f_genome
    uniform_real_distribution<double> dist(0,100);
        //normal mutate
    for(size_t row=0; row!=f_genome.rows(); row++)
        for(size_t col=0; col!=f_genome.cols(); col++)
            if(dist(_mt)<=5){
                double var = (double) (f_genome.maxCoeff()-f_genome.minCoeff())/((2*f_genome.size()));
                if(dist(_mt)<50) f_genome(row,col)+=var;
                else f_genome(row,col)-=var;
            }
        //severely mutate it
    for(size_t col=0;col!=f_genome.cols(); col++)
        if(dist(_mt)<=5){
            size_t swap_col = f_genome.cols()*dist(_mt)/100;
            if(swap_col==col)
                if((dist(_mt)<50&&swap_col!=0)||swap_col==f_genome.cols()-1) swap_col--;
                else swap_col++;
            MatrixXd mem = f_genome.col(col);
            f_genome.col(col)=f_genome.col(swap_col);
            f_genome.col(swap_col)=mem;
        }
        else
            if(dist(_mt)<6)
                f_genome.col(col)*=-1;
    //s_genome
        //normal mutate
    for(size_t row=0; row!=s_genome.rows(); row++)
        for(size_t col=0; col!=s_genome.cols(); col++)
            if(dist(_mt)<=5){
                double var = (double) (s_genome.maxCoeff()-s_genome.minCoeff())/(10*s_genome.size());
                s_genome(row,col)+=pow(-1,(bool) dist(_mt)<50)*var;
                if(dist(_mt)<50) s_genome(row,col)+=var;
                else s_genome(row,col)-=var;
            }
        //severely mutate it
    for(size_t col=0;col!=s_genome.cols(); col++)
        if(dist(_mt)<=5){
            size_t swap_col = s_genome.cols()*dist(_mt)/100;
            if(swap_col==col)
                if((dist(_mt)<50&&swap_col!=0)||swap_col==s_genome.cols()-1) swap_col--;
                else swap_col++;
            MatrixXd mem = s_genome.col(col);
            s_genome.col(col)=s_genome.col(swap_col);
            s_genome.col(swap_col)=mem;
        }
        else
            if(dist(_mt)<6)
                s_genome.col(col)*=-1;
}

void print_actual_genome(const Genome &_genome, const size_t &_i)
{
    ostringstream iss;
    iss<<_i;
    ofstream file("f_genome_"+iss.str());
    file<<_genome.f_genome;
    file.close();
    file.open("s_genome_"+iss.str());
    file<<_genome.s_genome;
    file.close();
}

void load_genome(vector<Genome> &_population)
{
    for(size_t i=0; i!=_population.size(); i++){
        ostringstream iss;
        iss<<i;
        ifstream file("f_genome_"+iss.str());
        string line; double num;
        size_t row=0, col=0;
        while(getline(file, line)){
            istringstream to_double(line);
            while(to_double>>num){
                _population[i].f_genome(row,col)=num;
                col++;
            }
            row++;
            col=0;
        }
        file.close();
    }
    for(size_t i=0; i!=_population.size(); i++){
        ostringstream iss;
        iss<<i;
        ifstream file("s_genome_"+iss.str());
        string line; double num;
        size_t row=0, col=0;
        while(getline(file, line)){
            istringstream to_double(line);
            while(to_double>>num){
                _population[i].s_genome(row,col)=num;
                col++;
            }
            row++;
            col=0;
        }
        file.close();
    }
}

void natural_selection(vector<Genome> &_population, const size_t &_initial_num_genomes, const size_t &_num_genomes, mt19937 &_mt)
{
    uniform_real_distribution<double> dist(0,1);
    vector<double> idx(_population.size(),0);
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(),idx.end(), [&_population](size_t _idx1, size_t _idx2) {return _population[_idx1].score < _population[_idx2].score;});
    double sum=(1+idx.size())*idx.size()/2;
    vector<double> accumulate_probability(idx.size(),0);
    for(size_t i=0; i!=idx.size(); i++)
        if(i==0) accumulate_probability[i]= (double)(i+1)/sum;
        else accumulate_probability[i]= (double) (i+1)/sum+accumulate_probability[i-1];
    vector<Genome> population_mem;
    for(size_t i=0;i!=_num_genomes; i++){
        Genome gen;
        population_mem.push_back(gen);
    }
    population_mem[0]=_population[idx.back()];
    population_mem[1]=_population[idx[idx.size()-2]];
    for(size_t i=2; i!=_num_genomes;i++){
        //father
        double selection=dist(_mt);
        auto iterator_ub = upper_bound(accumulate_probability.begin(),accumulate_probability.end(),selection);
        size_t father=distance(accumulate_probability.begin(), iterator_ub);
        //mother
        selection= dist(_mt);
        iterator_ub = upper_bound(accumulate_probability.begin(),accumulate_probability.end(),selection);
        size_t mother=distance(accumulate_probability.begin(), iterator_ub);
        //make the baby!
        Genome baby;
        if(dist(_mt)<50) baby=cross_over(_population[idx[father]],_population[idx[mother]],_mt);
        else baby=cross_over(_population[idx[mother]],_population[idx[father]],_mt);
        baby.mutate(_mt);
        population_mem[i]=baby;
    }
    if(_population.size()==_initial_num_genomes&&_initial_num_genomes!=_num_genomes) _population.resize(_num_genomes);
    _population=population_mem;
}
