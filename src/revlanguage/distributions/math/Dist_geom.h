#ifndef Dist_geom_H
#define Dist_geom_H

#include "GeometricDistribution.h"
#include "Natural.h"
#include "RlTypedDistribution.h"

namespace RevLanguage {
    
    /**
     * The RevLanguage wrapper of the geometric distribution.
     *
     * The RevLanguage wrapper of the geometric distribution simply
     * manages the interactions through the Rev with our core.
     * That is, the internal distribution object can be constructed and hooked up
     * in a model graph.
     * See the Dist_geom.h for more details.
     *
     *
     * @copyright Copyright 2009-
     * @author The RevBayes Development Core Team (Sebastian Hoehna)
     * @since 2012-08-08, version 1.0
     *
     */
    class Dist_geom :  public TypedDistribution<Natural> {
        
    public:
        Dist_geom( void );                                                                                                                //!< Default constructor
        
        // Basic utility functions
        Dist_geom*                          clone(void) const;                                                                              //!< Clone the object
        static const std::string&                       getClassName(void);                                                                             //!< Get class name
        static const TypeSpec&                          getClassTypeSpec(void);                                                                         //!< Get class type spec
        const TypeSpec&                                 getTypeSpec(void) const;                                                                        //!< Get the type spec of the instance
        const MemberRules&                              getMemberRules(void) const;                                                                     //!< Get member rules (const)
        
        
        // Distribution functions you have to override
        RevBayesCore::GeometricDistribution*            createDistribution(void) const;                                                                 //!< Create the internal distribution object
        
    protected:
        
        void                                            setConstMemberVariable(const std::string& name, const RevPtr<const Variable> &var);             //!< Set member variable
        
        
    private:
        RevPtr<const Variable>                          p;                                                                                              //!< The rate of the distribution
        
    };
    
}

#endif