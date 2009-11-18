/**
 * @file
 * This file contains the declaration of DAGNode, which is the base
 * class for nodes in a model DAG.
 *
 * @brief Declaration of DAGNode
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The REvBayes development core team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-16, version 1.0
 * @interface DAGNode
 * @extends RbObject
 *
 * $Id$
 */

#ifndef DAGNode_H
#define DAGNode_H

#include "../main/RbObject.h"
#include <string>
#include <set>


/**
 * @brief DAGNode, abstract base class for nodes in a model graph (DAG)
 *
 * This is the abstract base class for nodes in a model graph, a DAG (Directed
 * Acyclic Graph). DAGNode objects fall into three types, 
 *
 * A DAGNode has the ability to restore itself to a previous state to facilitate
 * MCMC sampling. A touched flag is used to mark a node for later recalculation.
 * The node will then be recalculated when its getValue method is called. When it
 * is recalculated, the changed flag is set to true so that subsequent calls to
 * get value knows that the work has been done.
 *
 * Note that only some nodes need to recalculate their value; others have a fixed
 * value and the base class behavior of just returning the value unconditionally
 * on a call to getValue is appropriate. When getValue is called, the old value
 * should be kept in storedValue in all derived classes that have a value that
 * can change.  A subsequent call to restore will restore the value of the node to
 * its previous value; a call to keep will simply unmark the node by setting
 * the touched flag to false.
 *
 * To facilitate operation on sections of DAGs, this class also implements a
 * touchAffected function, which touches all affected children nodes until it
 * reaches a node that says no additional children will be affected; this would
 * typically be a stochastic node. Similarly, keep and restore have recursive
 * versions that deal with all affected nodes in the graph (as determined by their
 * touched flags). Normally, it is the recursive versions that should be called
 * during an MCMC run.
 *
 * The language is structured such that all named objects will be folded into
 * DAGNode objects. Syntax elements produced by the parser are also converted to
 * unnamed DAGNode objects when representing dynamically evaluated expressions.
 */
class DAGNode : public RbObject {

    public:
	        virtual ~DAGNode(void);     //!< Destructor

	    void                addChildNode(DAGNode* c) { children.insert(c); }    //!< Add child node
        DAGNode*            clone(const DAGNode& d) const;                      //!< Clone this node
	    set<DAGNode*>&      getChildrenNodes(void) { return children; }         //!< Get children nodes
	    set<DAGNode*>&      getParentNodes(void) { return parents; }            //!< Get parent nodes
        virtual RbObject*   getValue() = 0;                             //!< Get value
	    virtual bool        isChanged(void) const { return changed; }   //!< Has the node recalculated its value?
        bool                isTouched() const { return touched; }       //!< Is the node marked for recalculation?
        void                keep() { touched = changed = false; }   //!< Keep current value of node
        void                keepAffected();                         //!< Keep value of affected nodes recursively
        bool                operator==(const DAGNode& d) const;     //!< Compare two DAG nodes
        void                print(std::ostream& o) const;           //!< Print this DAG node
	    void                printChildren(std::ostream& o) const;   //!< Print children DAG nodes
	    void                printParents(std::ostream& o) const;    //!< Print parent DAG nodes
	    void                removeChildNode(DAGNode* c) { children.erase(c); }  //!< Remove a child node
        void                restore();                              //!< Restore node to previous value
        void                restoreAffected();                      //!< Restore affected nodes recursively
        void                touch() { touched = true; }             //!< Mark node for recalculation
        void                touchAffected();                        //!< Mark affected nodes recursively
	
    protected:
            // The constructors are protected because this is an abstract class.
            // Only instances from derived classes are allowed.
            DAGNode();                  //!< Default constructor
            DAGNode(RbObject* val);     //!< Constructor from value
            DAGNode(const DAGnode& d);  //!< Copy constructor

	    RbObject*       storedValue;    //!< Holds the previous value
        RbObject*       value;          //!< Holds the current value

	    bool            changed;        //!< True if value has been recalculated
	    bool            touched;        //!< Marks node for recalculation

	    set<DAGNode*>   children;       //!< Set of children nodes
	    set<DAGNode*>   parents;        //!< Set of parent nodes
};

#endif
