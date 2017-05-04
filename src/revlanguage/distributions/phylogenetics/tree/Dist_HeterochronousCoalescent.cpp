#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "Clade.h"
#include "ConstantPopulationHeterochronousCoalescent.h"
#include "Dist_HeterochronousCoalescent.h"
#include "ModelVector.h"
#include "Natural.h"
#include "Real.h"
#include "RealPos.h"
#include "RlClade.h"
#include "RlString.h"
#include "RlTimeTree.h"
#include "StochasticNode.h"
#include "Taxon.h"

using namespace RevLanguage;

/**
 * Default constructor.
 *
 * The default constructor does nothing except allocating the object.
 */
Dist_HeterochronousCoalescent::Dist_HeterochronousCoalescent() : TypedDistribution<TimeTree>()
{
    
}


/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'b'.
 *
 * \return A new copy of the process.
 */
Dist_HeterochronousCoalescent* Dist_HeterochronousCoalescent::clone( void ) const
{
    return new Dist_HeterochronousCoalescent(*this);
}


/**
 * Create a new internal distribution object.
 *
 * This function simply dynamically allocates a new internal distribution object that can be
 * associated with the variable. The internal distribution object is created by calling its
 * constructor and passing the distribution-parameters (other DAG nodes) as arguments of the
 * constructor. The distribution constructor takes care of the proper hook-ups.
 *
 * \return A new internal distribution object.
 */
RevBayesCore::ConstantPopulationHeterochronousCoalescent* Dist_HeterochronousCoalescent::createDistribution( void ) const
{
    
    // get the parameters
    
    // theta
    RevBayesCore::TypedDagNode<double>* t       = static_cast<const RealPos &>( theta->getRevObject() ).getDagNode();
    // taxon names
    const std::vector<std::string> &names       = static_cast<const ModelVector<RlString> &>( taxonNames->getRevObject() ).getDagNode()->getValue();
    // clade constraints
    const std::vector<RevBayesCore::Clade> &c   = static_cast<const ModelVector<Clade> &>( constraints->getRevObject() ).getValue();
    
    std::vector<RevBayesCore::Taxon> taxa;
    for (size_t i = 0; i < names.size(); ++i)
    {
        taxa.push_back( RevBayesCore::Taxon( names[i] ) );
    }
    
    // create the internal distribution object
    RevBayesCore::ConstantPopulationHeterochronousCoalescent*   d = new RevBayesCore::ConstantPopulationHeterochronousCoalescent(t, taxa, c);
    
    return d;
}



/**
 * Get Rev type of object
 *
 * \return The class' name.
 */
const std::string& Dist_HeterochronousCoalescent::getClassType( void )
{
    
    static std::string rev_type = "Dist_HeterochronousCoalescent";
    
    return rev_type;
}


/**
 * Get class type spec describing type of an object from this class (static).
 *
 * \return TypeSpec of this class.
 */
const TypeSpec& Dist_HeterochronousCoalescent::getClassTypeSpec( void )
{
    
    static TypeSpec rev_type_spec = TypeSpec( getClassType(), new TypeSpec( TypedDistribution<TimeTree>::getClassTypeSpec() ) );
    
    return rev_type_spec;
}


/**
 * Get the Rev name for the distribution.
 * This name is used for the constructor and the distribution functions,
 * such as the density and random value function
 *
 * \return Rev name of constructor function.
 */
std::string Dist_HeterochronousCoalescent::getDistributionFunctionName( void ) const
{
    // create a distribution name variable that is the same for all instance of this class
    std::string d_name = "HeterochronousCoalescent";
    
    return d_name;
}


/**
 * Get the member rules used to create the constructor of this object.
 *
 * The member rules of the diversity-dependent pure-birth process are:
 * (1) the initial speciation rate lambda which must be a positive real.
 * (2) the carrying capacity that must be a natural number.
 * (3) all member rules specified by BirthDeathProcess.
 *
 * \return The member rules.
 */
const MemberRules& Dist_HeterochronousCoalescent::getParameterRules(void) const
{
    
    static MemberRules dist_member_rules;
    static bool rules_set = false;
    
    if ( !rules_set )
    {
        dist_member_rules.push_back( new ArgumentRule( "theta"      , RealPos::getClassTypeSpec(), "The constant population size.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        dist_member_rules.push_back( new ArgumentRule( "names"      , ModelVector<RlString>::getClassTypeSpec(), "The taxon names used when drawing a random tree.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
        dist_member_rules.push_back( new ArgumentRule( "constraints", ModelVector<Clade>::getClassTypeSpec()   , "The topological constraints strictly enforced.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new ModelVector<Clade>() ) );
        
        // add the rules from the base class
        const MemberRules &parentRules = TypedDistribution<TimeTree>::getParameterRules();
        dist_member_rules.insert(dist_member_rules.end(), parentRules.begin(), parentRules.end());
        
        rules_set = true;
    }
    
    return dist_member_rules;
}


/**
 * Get type-specification on this object (non-static).
 *
 * \return The type spec of this object.
 */
const TypeSpec& Dist_HeterochronousCoalescent::getTypeSpec( void ) const
{
    
    static TypeSpec ts = getClassTypeSpec();
    
    return ts;
}


/**
 * Set a member variable.
 *
 * Sets a member variable with the given name and store the pointer to the variable.
 * The value of the variable might still change but this function needs to be called again if the pointer to
 * the variable changes. The current values will be used to create the distribution object.
 *
 * \param[in]    name     Name of the member variable.
 * \param[in]    var      Pointer to the variable.
 */
void Dist_HeterochronousCoalescent::setConstParameter(const std::string& name, const RevPtr<const RevVariable> &var)
{
    
    if ( name == "theta" )
    {
        theta = var;
    }
    else if ( name == "names" )
    {
        taxonNames = var;
    }
    else if ( name == "constraints" )
    {
        constraints = var;
    }
    else
    {
        TypedDistribution<TimeTree>::setConstParameter(name, var);
    }
    
}
