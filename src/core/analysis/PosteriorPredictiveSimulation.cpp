#include "DagNode.h"
#include "PosteriorPredictiveSimulation.h"
#include "RbFileManager.h"

using namespace RevBayesCore;

PosteriorPredictiveSimulation::PosteriorPredictiveSimulation( const Model &m, const std::string &dir, const RbVector<ModelTrace> &t) :
    model( m ),
    directory( dir ),
    traces( t )
{
    
}



PosteriorPredictiveSimulation::~PosteriorPredictiveSimulation()
{
    
}



/** Clone function */
PosteriorPredictiveSimulation* PosteriorPredictiveSimulation::clone() const
{
    
    return new PosteriorPredictiveSimulation(*this);
}


void RevBayesCore::PosteriorPredictiveSimulation::run( int thinning )
{
    
    // some general constant variables
    RbFileManager fm = RbFileManager( directory );
    const std::string path_separator = fm.getPathSeparator();
    
    // this is where we need to implement the posterior predictive simulation
    
    size_t n_samples = traces[0].size();
    size_t n_traces = traces.size();
    
    std::vector<DagNode*> nodes = model.getDagNodes();
    
    for (size_t i=0; i<(n_samples/thinning); ++i)
    {
        
        // create a new directory name for this simulation
        std::stringstream s;
        s << directory << path_separator << "posterior_predictive_sim_" << i;
        std::string sim_directory_name = s.str();
        
        // now for the numerical parameters
        for ( size_t j=0; j<n_traces; ++j )
        {
            std::string parameter_name = traces[j].getParameterName();
            
            // iterate over all DAG nodes (variables)
            for ( std::vector<DagNode*>::iterator it = nodes.begin(); it!=nodes.end(); ++it )
            {
                DagNode *the_node = *it;
                
                if ( the_node->getName() == parameter_name )
                {
                    // set the value for the variable with the i-th sample
                    the_node->setValueFromString( traces[j].objectAt( i ) );
                }
                
            }
            
        }
        
        // next we need to simulate the data and store it
        // iterate over all DAG nodes (variables)
        for ( std::vector<DagNode*>::iterator it = nodes.begin(); it!=nodes.end(); ++it )
        {
            DagNode *the_node = *it;
            
            if ( the_node->isClamped() )
            {
                // redraw new values
                the_node->redraw();
                
                // we need to store the new simulated data
                the_node->writeToFile(sim_directory_name);
                
            }
            
        }
        
    }
    
}
