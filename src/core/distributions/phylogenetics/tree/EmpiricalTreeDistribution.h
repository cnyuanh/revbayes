 /**
 * @file
 * This file contains the declaration and implementation
 * of the EmpiricalTreeDistribution class. The distribution 
 * is constructed from an input tree trace.
 * 
 *
 * @brief Declaration and implementation of the EmpiricalTreeDistribution class
 *
 * (c) Copyright 2014-
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @since Version 1.0
 *
 */

#ifndef EmpiricalTreeDistribution_H
#define EmpiricalTreeDistribution_H


#include "MemberObject.h"
#include "Sample.h"
#include "TypedDistribution.h"
#include "Tree.h"
#include "TraceTree.h"

namespace RevBayesCore {
    
    class EmpiricalTreeDistribution : public TypedDistribution<Tree>, public MemberObject<int> {
        
    public:
		
		EmpiricalTreeDistribution(const TraceTree &t, int b );
		
		virtual                                             ~EmpiricalTreeDistribution(void); 

		EmpiricalTreeDistribution*                          clone(void) const;                                          //!< Create an independent clone
		double                                              computeLnProbability(void);                                 //!< Compute ln prob of current value
        void                                                executeMethod(const std::string &n, const std::vector<const DagNode*> &args, int &rv) const;     //!< Map the member methods to internal function calls
        size_t                                              getBurnin(void) const;
        size_t                                              getCurrentTreeIndex(void) const;
        size_t                                              getNumberOfTrees(void) const;
        void                                                redrawValue(void);                                          //!< Draw a new random value from distribution
		void                                                setCurrentTree(size_t index);
        void                                                setValue(Tree *v, bool f=false);
		
	protected:

		void                                                swapParameterInternal(const DagNode *oldP, const DagNode *newP);            //!< Swap a parameter
		
	private:
		
		double                                              probability;
		size_t                                              burnin;
		size_t                                              current_tree_index;
		TraceTree                                           trace;
		
	};

} 


#endif
