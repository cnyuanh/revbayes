/**
 * @file
 * This file contains the declaration of InferenceMove, which is the abstract
 * base class for InferenceMoves used in mcmc inference.
 *
 * @brief Declaration of InferenceMove
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2012-03-23 18:42:52 +0100 (Fri, 23 Mar 2012) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-09-08, version 1.0
 *
 * $Id: InferenceMove.h 1358 2012-03-23 17:42:52Z hoehna $
 */

#ifndef InferenceMove_H
#define InferenceMove_H

#include <vector>

class StochasticInferenceNode;

class InferenceMove {
    
public:
    virtual                                    ~InferenceMove(void);                                                              //!< Destructor
     
    // functions you have to override
    virtual InferenceMove*                  clone(void) const = 0;                                                                //!< Clone the move to get an independent copy
//    virtual std::vector<StochasticNode*>&   getDagNodes(void) = 0;                                                                  //!< Get the nodes vector
    
    // functions you should not override
//    virtual void                            addDagNode(StochasticNode* d);                                                                 //!< Add a DAG node to this InferenceMove
    void                                    accept(void);                                                                           //!< Accept the InferenceMove
    double                                  getAcceptanceRatio(void) const;                                                         //!< Get acceptance ratio
    double                                  getUpdateWeight(void) const;                                                            //!< Get update weight of InferenceMove
    double                                  perform(double& probRatio);                                                             //!< Perform the InferenceMove
    void                                    reject(void);                                                                           //!< Reject the InferenceMove
    void                                    resetCounters(void);                                                                    //!< Reset numTried/numAccepted
    
protected:
    InferenceMove();                                                                                           //!< Default constructor
    
    virtual void                            acceptMove(void) = 0;
    virtual double                          performMove(double &probRatio) = 0;
    virtual void                            rejectMove(void) = 0;
    
    
    // parameters
    double                                  weight;
    
    // Hidden member variables
    size_t                                  numAccepted;                                                                            //!< Number of times accepted
    size_t                                  numTried;                                                                               //!< Number of times tried
    
    std::vector<StochasticInferenceNode*>   nodes;
        
};

#endif
