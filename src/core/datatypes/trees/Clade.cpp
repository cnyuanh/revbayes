#include "Clade.h"
#include "RbVectorUtilities.h"

#include <algorithm>
#include <iostream>
#include <sstream>


using namespace RevBayesCore;


/**
 * Default constructor required by the revlanguage code.
 * We use an empty string and an age of 0.0 for
 * this default object.
 */
Clade::Clade( void ) :
    age( 0.0 ),
    taxa()
{
    
}


/**
 * Default constructor that instantiates the object.
 * Additionally, we sort the vector of taxon names.
 *
 * \param[in]   n    The vector containing the taxon names.
 */
Clade::Clade(const std::vector<Taxon> &n, double a) :
    age( a ),
    taxa( n )
{
    
    // for identifiability we always keep the taxon names sorted
//    std::sort(taxa.begin(), taxa.end());
    VectorUtilities::sort( taxa );
}


/**
 * Overloaded equals operator.
 * Only if we have the extact same taxon names then these two clades are equal.
 */
bool Clade::operator==(const Clade &c) const 
{
    
    if ( c.size() != taxa.size() )
    {
        return false;
    }
    
    for (size_t i = 0; i < taxa.size(); ++i)
    {
        if ( taxa[i] != c.getTaxon(i) )
        {
            return false;
        }
    }
    
    return true;
}


/**
 * Not equals operator that uses the equals operator.
 */
bool Clade::operator!=(const Clade &c) const 
{
    return !operator==( c );
}


/**
 * Less than operator so that we can sort the clades.
 */
bool Clade::operator<(const Clade &c) const 
{
    
    if ( taxa.size() < c.size() )
    {
        return true;
    }
    else if ( taxa.size() > c.size() )
    {
        return false;
    }
    
    for (size_t i = 0; i < taxa.size(); ++i)
    {
        
        if ( taxa[i] < c.getTaxon(i) )
        {
            return true;
        }
        else if ( taxa[i] > c.getTaxon(i) )
        {
            return false;
        }
        
    }
    
    return false;
}


/**
 * Less than operator so that we can sort the clades.
 */
bool Clade::operator<=(const Clade &c) const
{
    return operator<( c ) || operator==( c );
}


/**
 * Less than operator so that we can sort the clades.
 */
bool Clade::operator>(const Clade &c) const
{
    return operator<( c ) == false && operator==( c ) == false;
}


/**
 * Less than operator so that we can sort the clades.
 */
bool Clade::operator>=(const Clade &c) const
{
    return operator>( c ) == false;
}



/**
 * Get the const-iterator to the first taxon name.
 */
std::vector<Taxon>::const_iterator Clade::begin(void) const
{
    return taxa.begin();
}


/**
 * Get the iterator to the first taxon name.
 */
std::vector<Taxon>::iterator Clade::begin(void)
{
    return taxa.begin();
}


/**
 * Get the const-iterator after the last taxon name.
 */
std::vector<Taxon>::const_iterator Clade::end(void) const
{
    return taxa.end();
}


/**
 * Get the iterator after the last taxon name.
 */
std::vector<Taxon>::iterator Clade::end(void)
{
    return taxa.end();
}


/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'B'.
 *
 * \return A new copy of myself 
 */
Clade* Clade::clone(void) const 
{
    return new Clade(*this);
}


/**
 * Add a taxon to the list.
 *
 * \param[in]    t    The new taxon.
 */
void Clade::addTaxon(const Taxon &t)
{
    return taxa.push_back( t );
}


/**
 * Get the clade age.
 *
 * \return    The stored age.
 */
double Clade::getAge( void ) const 
{
    
    return age;
}


/**
 * Get all taxon names.
 *
 * \return       The vector of taxon names.
 */
std::vector<Taxon>& Clade::getTaxa( void )
{
    return taxa;
}


/**
 * Get all taxon names.
 *
 * \return       The vector of taxon names.
 */
const std::vector<Taxon>& Clade::getTaxa( void ) const
{
    return taxa;
}


/**
 * Get the taxon name at position i.
 *
 * \param[in]    i    The index for the taxon name we are interested in.
 *
 * \return       The name of the taxon.
 */
const Taxon& Clade::getTaxon(size_t i) const
{
    return taxa[i];
}


/**
 * Get the taxon name at position i.
 *
 * \param[in]    i    The index for the taxon name we are interested in.
 *
 * \return       The name of the taxon.
 */
const std::string& Clade::getTaxonName(size_t i) const
{
    return taxa[i].getName();
}


/**
 * Get the number of taxa contained in this clade.
 *
 * \return       Size of the taxon name vector.
 */
size_t Clade::size(void) const 
{
    return taxa.size();
}


/**
 * Write the value of this clade as a string.
 *
 * \return    A single string containing the entire clade.
 */
std::string Clade::toString( void ) const
{
    std::string s = "{";
    
    for (size_t i = 0; i < taxa.size(); ++i)
    {
        if ( i > 0 )
        {
            s += ",";
        }
        s += taxa[i].getName();
    }
    s += "}";
    
    return s;
}


std::ostream& RevBayesCore::operator<<(std::ostream& o, const Clade& x) {
   
    o << x.toString();
   
    return o;
}

