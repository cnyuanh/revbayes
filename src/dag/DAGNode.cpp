/**
 * @file
 * This file contains the implementation of DAGNode, which is the base
 * class for nodes in the model DAG.
 *
 * @brief Implementation of DAGNode
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The REvBayes development core team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-16, version 1.0
 * @interface Distribution
 * @extends RbObject
 *
 * $Id$
 */

#include "DAGNode.h"
#include "RbObject.h"

#include <iostream>


/**
 * @brief DAGNode default constructor
 *
 * This constructor creates an empty DAGNode.
 *
 */
DAGNode::DAGNode()
    : storedValue(NULL), value(NULL), changed(false), touched(false),
      children(), parents() {
}


/**
 * @brief DAGNode constructor from value
 *
 * This constructor creates a DAGNode with a variable attached to the node.
 *
 * @param val   The value of the DAG node
 *
 */
DAGNode::DAGNode(RbObject *val)
    : storedValue(NULL), value(val), changed(false), touched(false),
      children(), parents() {
}

/**
 * @brief DAGNode copy constructor
 *
 * This constructor creates a DAGNode as a copy of another
 * DAGNode.
 *
 * @param d     The DAG node to clone.
 *
 */
DAGNode::DAGNode(const DAGNode &d)
    :   storedValue(d.storedValue.clone()), value(value.clone()),
        changed(d.changed), touched(d.touched),
        children(), parents() {

    for (set<DAGNode*>::iterator i=d.children.begin(); i!=d.children.end(); i++)
            children.insert((*i)->clone());

    for (set<DAGNode*>::iterator i=parents.begin(); i!=parents.end(); i++)
            parents.insert((*i)->clone());
}


/**
 * @brief DAGNode desctructor
 *
 * This is the standard destructor for all DAG nodes.
 *
 */
DAGNode::~DAGNode(void) {

	if ( value != NULL )
		delete value;
	if ( storedValue != NULL )
		delete storedValue;
}


/**
 * @brief Clone a DAGNode
 *
 * This is the clone function for DAGNode objects.
 *
 */
DAGNode* DAGNode::clone(void) const {

	return (new DAGNode(*this));
}


/**
 * @brief Tell affected DAG nodes to keep current value
 *
 * This function calls all affected DAG nodes so that they
 * have a chance to keep the current value and discard the
 * previous value.
 *
 */
void DAGNode::keepAffected() {

    if (changed) {
        for (set<DAGNode*>::iterator i=children.begin(); i!=children.end(); i++)
            (*i)->keepAffected();
        keep();
    }
}


/**
 * @brief Compare DAG nodes
 *
 * To be the same, the DAG nodes must have the same parents and children,
 * as well as the same value and storedValue. It is the pointers to these
 * objects we need to compare, not their current values.
 *
 */
bool DAGNode::operator==(const DAGNode& d) const {

	if (value != d.value || storedValue != d.storedValue)
        return false;

    if (changed != d.changed || touched != d.touched)
        return false;

    if (children.size() != d.children.size() || parents.size() != d.parents.size())
        return false;

    for (set<DAGNode*>::iterator i=d.children.begin(); i!=d.children.end(); i++)
        if (children.find(*i) == children.end())
            return false;

    for (set<DAGNode*>::iterator i=d.parents.begin(); i!=d.parents.end(); i++)
        if (parents.find(*i) == parents.end())
            return false;

    return true;
}


/**
 * @brief Print DAG node
 *
 * This function prints the content of the DAG node.
 *
 * @param o     The ostream for printing.
 *
 */
void DAGNode::print(std::ostream& o) const {

	o << "DAGNode: ";
    //   o << getClass()->printValueVector(o);
    o << std::endl;

    o << "value:" << std::endl;
    if (!value)
        o << "null" << std::endl;
    else
        value->print(o);
    
    o << "storedValue:" << std::endl;
    if (!storedValue)
        o << "null" << std::endl;
    else
        storedValue->print(o);

    printParents(o);
    printChildren(o);
}


/**
 * @brief Print children
 *
 * This function prints the children DAG nodes to an ostream with a header
 * for each child.
 *
 * @param o     The ostream for printing.
 *
 */
void DAGNode::printChildren(std::ostream& o) const {

	if ( children.empty() )	
		{
		o << "No children" << std::endl;
		return;
		}

	for (int count=1, std::set<DAGNode*>::iterator i=children.begin(); i != children.end(); i++, count++) {
		o << "children[" << count << "]:" << std::endl;
        o << (*i) << std::endl;
    }

	o << std::endl;
}


/**
 * @brief Print parents
 *
 * This function prints the parent DAG nodes to an ostream with a header
 * for each parent.
 *
 * @param o     The ostream for printing.
 *
 */
void DAGNode::printParents(std::ostream& o) const {

	if ( parents.empty() )
		{
		o << "No parents" << std::endl;
		return;
		}

	for (int count=1, std::set<DAGNode*>::iterator i=parents.begin(); i != parents.end(); i++, count++) {
		o << "parents[" << count << "]:" << std::endl;
        o << (*i) << std::endl;
    }

	o << std::endl;
}


/**
 * @brief Restore the old value
 *
 * This function restores the old value of the DAG node. It is typically called
 * during MCMC sampling when a proposed value is rejected.
 *
 */
void DAGNode::restore() {

    RbObject* temp;
    
    if (changed) {
        temp        = value;
        value       = storedValue;
        storedValue = temp;
    }

    keep();     // Sets touched and changed to false
}


/**
 * @brief Restore affected nodes.
 *
 * This function calls all nodes that are affected by this DAG node and restores
 * them.
 *
 */
void DAGNode::restoreAffected() {

    if (changed) {
        for (set<DAGNode*>::iterator i=children.begin(); i!=children.end(); i++)
            (*i)->restoreAffected();
        restore();
    }
}


/**
 * @brief Thouch affected nodes.
 *
 * This function touches all affected DAG nodes, i.e. marks them as changed.
 *
 */
void DAGNode::touchAffected() {

    if (!touched) {
        touch();
        for (set<DAGNode*>::iterator i=children.begin(); i!=children.end(); i++)
            (*i)->touchAffected();
    }
}
