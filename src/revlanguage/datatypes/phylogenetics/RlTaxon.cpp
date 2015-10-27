#include "ConstantNode.h"
#include "ModelVector.h"
#include "RlTaxon.h"
#include "TimeAndDate.h"
#include "RbUtil.h"
#include "RlString.h"
#include "Real.h"
#include "TypeSpec.h"

#include <sstream>

using namespace RevLanguage;

/** Default constructor */
Taxon::Taxon(void) : ModelObject<RevBayesCore::Taxon>()
{

    ArgumentRules* speciesNameArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getSpeciesName", RlString::getClassTypeSpec(), speciesNameArgRules ) );
    
    ArgumentRules* ageArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getAge", RlString::getClassTypeSpec(), ageArgRules ) );
    
}

/** Construct from core Taxon */
Taxon::Taxon(RevBayesCore::Taxon *c) : ModelObject<RevBayesCore::Taxon>( c )
{

    ArgumentRules* speciesNameArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getSpeciesName", RlString::getClassTypeSpec(), speciesNameArgRules ) );
    
    ArgumentRules* ageArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getAge", RlString::getClassTypeSpec(), ageArgRules ) );
    
}

/** Construct from core Taxon */
Taxon::Taxon(const RevBayesCore::Taxon &t) : ModelObject<RevBayesCore::Taxon>( new RevBayesCore::Taxon( t ) )
{

    ArgumentRules* speciesNameArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getSpeciesName", RlString::getClassTypeSpec(), speciesNameArgRules ) );
    
    ArgumentRules* ageArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getAge", RlString::getClassTypeSpec(), ageArgRules ) );
    
}

/** Construct from DAG node */
Taxon::Taxon(RevBayesCore::TypedDagNode<RevBayesCore::Taxon> *n) : ModelObject<RevBayesCore::Taxon>( n )
{

    ArgumentRules* speciesNameArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getSpeciesName", RlString::getClassTypeSpec(), speciesNameArgRules ) );
    
    ArgumentRules* ageArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getAge", RlString::getClassTypeSpec(), ageArgRules ) );

}


/** Clone object */
Taxon* Taxon::clone(void) const
{
    
    return new Taxon(*this);
}


void Taxon::constructInternalObject( void ) 
{
    // we free the memory first
    if ( dagNode != NULL )
    {
        if ( dagNode->decrementReferenceCount() == 0 )
        {
            delete dagNode;
        }
    }
    
    // now allocate a new Taxon
    std::string taxonName = static_cast<const RlString &>( (taxon)->getRevObject() ).getValue() ;
    std::string taxonSpecies = static_cast<const RlString &>( (species)->getRevObject() ).getValue() ;
    double taxonAge = static_cast<const Real &>( age->getRevObject() ).getValue();
    
    dagNode = new RevBayesCore::ConstantNode<RevBayesCore::Taxon>("", new RevBayesCore::Taxon( taxonName ) );
    
    dagNode->getValue().setSpeciesName( taxonSpecies );
    dagNode->getValue().setAge( taxonAge );
    
    dagNode->incrementReferenceCount();
    
}


/* Map calls to member methods */
RevLanguage::RevPtr<RevLanguage::RevVariable> Taxon::executeMethod(std::string const &name, const std::vector<Argument> &args, bool &found)
{
    
    if (name == "getSpeciesName")
    {
        found = true;
        
        std::string n = this->dagNode->getValue().getSpeciesName();
        return RevPtr<RevVariable>( new RevVariable( new RlString( n ) ) );
    }
    if (name == "getAge")
    {
        found = true;
        
        double a = this->dagNode->getValue().getAge();
        return RevPtr<RevVariable>( new RevVariable( new Real( a ) ) );
    }
    
    return ModelObject<RevBayesCore::Taxon>::executeMethod( name, args, found );
}



/** Return member rules (no members) */
const MemberRules& Taxon::getParameterRules(void) const
{
    
    static MemberRules memberRules;
    static bool rulesSet = false;
    
    if ( !rulesSet )
    {
        
        memberRules.push_back( new ArgumentRule("taxonName"  , RlString::getClassTypeSpec(), "The name of the taxon.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
        memberRules.push_back( new ArgumentRule("speciesName", RlString::getClassTypeSpec(), "The name of the species it belongs to.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
        memberRules.push_back( new ArgumentRule("age",         Real::getClassTypeSpec(), "The age before the present when this taxon was sampled.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new Real(0.0) ) );
                
        rulesSet = true;
    }
    
    return memberRules;
}


/** Get Rev type of object */
const std::string& Taxon::getClassType(void) { 
    
    static std::string revType = "Taxon";
    
    return revType; 
}

/** Get class type spec describing type of object */
const TypeSpec& Taxon::getClassTypeSpec(void) { 
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( RevObject::getClassTypeSpec() ) );
    
    return revTypeSpec; 
}


/** Get type spec */
const TypeSpec& Taxon::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Set a member variable */
void Taxon::setConstParameter(const std::string& name, const RevPtr<const RevVariable> &var) {
    
    if ( name == "taxonName") 
    {
        taxon = var ;
    } 
    else if ( name == "speciesName") 
    {
        species = var ;
    } 
    else if ( name == "age") 
    {
        age = var ;
    } 
    else {
        RevObject::setConstParameter(name, var);
    }
}


