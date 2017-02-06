//
//  AdjacentRateModifierFunction.cpp
//  revbayes-proj
//
//  Created by Michael Landis on 2/3/17.
//  Copyright © 2017 Michael Landis. All rights reserved.
//

#include "AdjacentRateModifierFunction.h"
#include "AdjacentRateModifier.h"
#include "TypedDagNode.h"
#include <cmath>

using namespace RevBayesCore;

AdjacentRateModifierFunction::AdjacentRateModifierFunction(const TypedDagNode<double>* f, const TypedDagNode<int>* w, const TypedDagNode<RbVector<RbVector<double> > >* c, size_t ns, size_t nc) : TypedFunction<CharacterHistoryRateModifier>( new AdjacentRateModifier(ns, nc) ),
    factor(f),
    width(w),
    context_matrix(c),
    context_type("width")
{
    if (context_matrix != NULL) {
        context_type = "matrix";
    }
    
    // add the parameters as parents
    addParameter(factor);
    addParameter(width);
    addParameter(context_matrix);
    
    update();
}

AdjacentRateModifierFunction::AdjacentRateModifierFunction(const AdjacentRateModifierFunction& m) : TypedFunction<CharacterHistoryRateModifier>( m )
{
    factor = m.factor;
    width = m.width;
    context_matrix = m.context_matrix;
    context_type = m.context_type;
}


AdjacentRateModifierFunction::~AdjacentRateModifierFunction( void )
{
    // We don't delete the parameters, because they might be used somewhere else too. The model needs to do that!
}





AdjacentRateModifierFunction* AdjacentRateModifierFunction::clone( void ) const
{
    return new AdjacentRateModifierFunction( *this );
}


void AdjacentRateModifierFunction::update( void )
{
    
    double f = factor->getValue();
    static_cast<AdjacentRateModifier*>(value)->setFactor(f);
    
    if (context_type == "width")
    {
        size_t w = width->getValue();
        static_cast<AdjacentRateModifier*>(value)->setWidth(w);
    }
    else if (context_type=="matrix")
    {
        RbVector<RbVector<double> > c = context_matrix->getValue();
        static_cast<AdjacentRateModifier*>(value)->setContextMatrix(c);
    }
    
}



void AdjacentRateModifierFunction::swapParameterInternal(const DagNode *oldP, const DagNode *newP)
{
    
    if (oldP == factor)
    {
        factor = static_cast<const TypedDagNode<double>* >( newP );
    }
    else if (oldP == width)
    {
        width = static_cast<const TypedDagNode<int>* >( newP );
    }
    else if (oldP == context_matrix)
    {
        context_matrix = static_cast<const TypedDagNode<RbVector<RbVector<double> > >* >( newP );
    }
}
