#ifndef BiogeographicTreeHistoryCtmc_H
#define BiogeographicTreeHistoryCtmc_H

#include "AbstractTreeHistoryCtmc.h"
#include "BiogeographicCladoEvent.h"
#include "RateGeneratorSequence_Biogeography.h"
#include "ContinuousCharacterData.h"
#include "DistributionExponential.h"
#include "RateGeneratorSequence.h"
#include "RbConstants.h"
#include "RbVector.h"
#include "StandardState.h"
#include "TopologyNode.h"
#include "TransitionProbabilityMatrix.h"
#include "TypedDistribution.h"

#include "ConstantNode.h"
#include "HomologousDiscreteCharacterData.h"
#include "DiscreteCharacterState.h"
#include "RandomNumberFactory.h"
#include "TopologyNode.h"
#include "TransitionProbabilityMatrix.h"

#include <cmath>
#include <cstring>

namespace RevBayesCore {

    template<class charType>
    class BiogeographicTreeHistoryCtmc : public AbstractTreeHistoryCtmc<charType> {

    public:
        BiogeographicTreeHistoryCtmc(const TypedDagNode< Tree > *t, size_t nChars, size_t nSites, bool useAmbigChar=false, bool forbidExt=true, bool useClado=true, bool ut=false);
        virtual                                            ~BiogeographicTreeHistoryCtmc(void);                                                //!< Virtual destructor

        // public member functions

        BiogeographicTreeHistoryCtmc*                       clone(void) const;                                                           //!< Create an independent clone
        void                                                initializeTipValues(void);
        void                                                redrawValue(void);
        void                                                drawInitValue(void);
        virtual void                                        simulate(void);

        // These will be migrated to PathRejectionSampleProposal and NodeRejectionSampleProposal
        bool                                                samplePathStart(const TopologyNode& node, const std::set<size_t>& indexSet);
        bool                                                samplePathEnd(const TopologyNode& node, const std::set<size_t>& indexSet);
        bool                                                samplePathHistory(const TopologyNode& node, const std::set<size_t>& indexSet);

        // These have been migrated to RateGeneratorSequence_Biogeography and BiogeographyRateGeneratorSequenceFunction
        void                                                setRateGeneratorSequence(const TypedDagNode< RateGeneratorSequence > *rm);
        void                                                setRateGeneratorSequence(const TypedDagNode< RbVector< RateGeneratorSequence > > *rm);
        void                                                setRootFrequencies(const TypedDagNode< RbVector< double > > *f);
        void                                                setSiteRates(const TypedDagNode< RbVector< double > > *r);
        void                                                setDistancePower(const TypedDagNode<double>* dp);
        void                                                setCladogenicStateFrequencies(const TypedDagNode< RbVector<double> >* csf);

        // special tip/root state flags
//        const std::vector<double>&                          getTipProbs(const TopologyNode& nd);
//        const std::vector<std::vector<double> >&            getTipProbs(void);
//        void                                                setTipProbs(const AbstractCharacterData* d);
        const bool                                          getUseTail(void) const;


        // cladogenic state information
        const std::vector<int>&                             getBuddingStates(void);
        int                                                 getBuddingState(const TopologyNode& nd);
        void                                                setBuddingState(const TopologyNode& nd, int s);
        const std::vector<int>&                             getCladogenicStates(void);
        int                                                 getCladogenicState(const TopologyNode& nd);
        void                                                setCladogenicState(const TopologyNode& nd, int s);
        virtual const std::vector<double>&                  getCladogenicStateFrequencies(void);
        const bool                                          useCladogenicEvents(void) const;

        // epoch info
        const std::vector<double>&                          getEpochs(void) const;

    protected:
        // Parameter management functions
        void                                                swapParameterInternal(const DagNode *oldP, const DagNode *newP);            //!< Swap a parameter

        virtual double                                      computeRootLikelihood(const TopologyNode &n);
        virtual double                                      computeInternalNodeLikelihood(const TopologyNode &n);
        virtual double                                      computeTipLikelihood(const TopologyNode &node);
        virtual const std::vector<double>&                  getRootFrequencies(void);
        // (not needed)        void                         keepSpecialization(DagNode* affecter);
        // (not needed)        void                         restoreSpecialization(DagNode *restorer);
        virtual void                                        touchSpecialization(DagNode *toucher, bool touchAll);


    private:



        // helper function
        unsigned                                            getEpochIndex(double age);
        unsigned                                            getEpochIndex(const std::vector<double>& epochs, double age);
        size_t                                              numOn(const std::vector<CharacterEvent*>& s);
        bool                                                historyContainsExtinction(const std::vector<CharacterEvent*>& currState, const std::multiset<CharacterEvent*,CharacterEventCompare>& history);

        // members
        const TypedDagNode< RbVector< double > >*           rootFrequencies;
        const TypedDagNode< RbVector< double > >*           siteRates;
        const TypedDagNode< RateGeneratorSequence >*                      homogeneousRateGeneratorSequence;
        const TypedDagNode< RbVector< RateGeneratorSequence > >*          heterogeneousRateGeneratorSequences;
        const TypedDagNode< RbVector< double > >*           cladogenicStateFreqs;
        std::vector<double>                                 epochs;

        // flags specifying which model variants we use
        std::vector<int>                                    cladogenicState;
        std::vector<int>                                    buddingState;
        bool                                                branchHeterogeneousClockRates;
        bool                                                branchHeterogeneousSubstitutionMatrices;
        bool                                                rateVariationAcrossSites;
        bool                                                cladogenicEvents;
        bool                                                imperfectTipData;
        bool                                                forbidExtinction;
        bool                                                useTail;
        int                                                 redrawCount;

        virtual void                                        simulate(const TopologyNode& node, BranchHistory* bh, std::vector< DiscreteTaxonData< charType > >& taxa);
        void                                                simulateHistory(const TopologyNode& node, BranchHistory* bh);
        void                                                simulateCladogenesis(const TopologyNode& node);

    };

}



#include "RbConstants.h"

template<class charType>
RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::BiogeographicTreeHistoryCtmc(const TypedDagNode<Tree> *t, size_t nChars, size_t nSites, bool useAmbigChar, bool forbidExt, bool useClado, bool ut) : AbstractTreeHistoryCtmc<charType>(  t, nChars, nSites, useAmbigChar )
{

    // initialize with default parameters
    rootFrequencies             = new ConstantNode< RbVector<double> >("rootFrequencies", new RbVector<double>(2, 1.0));
    siteRates                   = NULL;
    homogeneousRateGeneratorSequence          = NULL; // Define a good standard JC RateGeneratorSequence
    heterogeneousRateGeneratorSequences       = NULL;

    std::vector<double> csfInit = std::vector<double>(3, 0.33);
    cladogenicStateFreqs        = new ConstantNode< RbVector<double> >("cladoStateFreqs", new RbVector<double>(csfInit));
    redrawCount                 = 0;


    // flags specifying which model variants we use
    branchHeterogeneousClockRates               = false;
    branchHeterogeneousSubstitutionMatrices     = false;
    rateVariationAcrossSites                    = false;
    cladogenicEvents                            = useClado;
    imperfectTipData                            = false;
    forbidExtinction                            = forbidExt;
    useTail                                     = !false; //ut;

    cladogenicState                             = std::vector<int>(this->histories.size(), 0);
    if (cladogenicEvents && false) {
        for (size_t i = 0; i < cladogenicState.size(); i++) {
            cladogenicState[i] = int(GLOBAL_RNG->uniform01() * 1);
        }
    }
    buddingState                                = std::vector<int>(this->histories.size(), 0);
    epochs                                      = std::vector<double>(1,0.0);

    // add the parameters to our set (in the base class)
    // in that way other class can easily access the set of our parameters
    // this will also ensure that the parameters are not getting deleted before we do
    this->addParameter( rootFrequencies );
    this->addParameter( siteRates );
    this->addParameter( homogeneousRateGeneratorSequence );
    this->addParameter( heterogeneousRateGeneratorSequences );
    this->addParameter( cladogenicStateFreqs );

}


template<class charType>
RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::~BiogeographicTreeHistoryCtmc( void ) {
    // We don't delete the parameters, because they might be used somewhere else too. The model needs to do that!

}


template<class charType>
RevBayesCore::BiogeographicTreeHistoryCtmc<charType>* RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::clone( void ) const {

    return new BiogeographicTreeHistoryCtmc<charType>( *this );
}



template<class charType>
double RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::computeRootLikelihood(const TopologyNode &n)
{
    return 0.0;
}

template<class charType>
double RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::computeInternalNodeLikelihood(const TopologyNode &node)
{

    size_t nodeIndex = node.getIndex();
    double lnL = 0.0;

    BranchHistory* bh = this->histories[nodeIndex];
    std::vector<CharacterEvent*> currState = bh->getParentCharacters();
    unsigned int n1 = (unsigned)numOn(currState);
	unsigned int n0 = (unsigned)(this->numSites - n1);
    unsigned counts[2] = { n0, n1 };

    // check that node ages are consistent with character event ages
    if ( bh->areEventTimesValid(node) == false)
    {
        bh->print();
        return RbConstants::Double::neginf;
    }

    // check parent and child states to make sure they match with the
    // ancestral and descendant branches; otherwise, return -Inf
    std::vector<CharacterEvent*> end_state = bh->getChildCharacters();
    for(size_t i = 0; i < node.getNumberOfChildren(); ++i)
    {
        const TopologyNode &child = node.getChild(i);
        size_t child_index = child.getIndex();
        BranchHistory* child_bh = this->histories[child_index];
        std::vector<CharacterEvent*> child_state = child_bh->getParentCharacters();
        for(size_t j = 0; j < this->numSites; ++j)
        {
            if(end_state[j]->getState() != child_state[j]->getState())
            {
                
                return RbConstants::Double::neginf;
            }
        }
    }

    if (!node.isTip() && cladogenicEvents)
    {
        const std::vector<double>& cladoProbs = cladogenicStateFreqs->getValue();
        int s = cladogenicState[ node.getChild(0).getIndex() ];
        double v = 0.0;
        if (s > 0)
            v = log( cladoProbs[s-1] );
        lnL += v;

    }

    if (node.isRoot() && useTail == false)
    {

        return lnL;
    }

    if (counts[1] == 0 && forbidExtinction)
    {
        
        return RbConstants::Double::neginf;
    }
    else
    {
        const std::multiset<CharacterEvent*,CharacterEventCompare>& history = bh->getHistory();
        std::multiset<CharacterEvent*,CharacterEventCompare>::iterator it_h;

        double currAge = (node.isRoot() ? this->tau->getValue().getRoot().getAge()*2 : node.getParent().getAge());
        double endAge = node.getAge();
        const RateGeneratorSequence_Biogeography& rm = static_cast<const RateGeneratorSequence_Biogeography&>(homogeneousRateGeneratorSequence->getValue());

        // handle stratified/epoch models
        const std::vector<double>& epochs = rm.getEpochs();
        size_t epochIdx = getEpochIndex(epochs, currAge);
        double epochEndAge = epochs[epochIdx];

        // stepwise events
        double eventAge = 0.0;
        bool useEpoch = true;
        for (it_h = history.begin(); it_h != history.end(); it_h++)
        {
            
            // next event time
            double idx = (*it_h)->getSiteIndex();
            eventAge = (*it_h)->getAge();

            // reject extinction
            size_t s = (*it_h)->getState();
            if (counts[1] == 0 && forbidExtinction)
            {
                
                return RbConstants::Double::neginf;
                break;
            }

            // if epoch crossed, compute prob no events until boundary then advance epochIdx
            while (useEpoch && eventAge < epochEndAge)
            {
                // waiting factor
                double sr = rm.getSumOfRates( currState, counts, this->computeBranchRate( node.getIndex() ), currAge);
                lnL += -sr * (currAge - epochEndAge);

                // if before branch end, advance epoch
                if (endAge < epochEndAge)
                {
                    epochIdx++;
                    currAge = epochEndAge;
                    epochEndAge = epochs[epochIdx];
                }
                // otherwise, exit loop
                else
                {
                    break;
                }

            }

            // lnL for stepwise events for p(x->y)
            double tr = rm.getRate( currState, *it_h, counts, this->computeBranchRate( node.getIndex() ), currAge);
            double sr = rm.getSumOfRates( currState, counts, this->computeBranchRate( node.getIndex() ), currAge);
            lnL += -(sr * (currAge - eventAge)) + log(tr);

            // update counts
            counts[currState[idx]->getState()] -= 1;
            counts[s] += 1;

            // update time and state
            currState[idx] = *it_h;
            currAge = eventAge;

        }

        // lnL for final non-event
        if (useEpoch) {
            while (epochEndAge > endAge)
            {
                // waiting factor
                double sr = rm.getSumOfRates( currState, counts, this->computeBranchRate( node.getIndex() ), currAge);
                lnL += -sr * (currAge - epochEndAge);

                // advance epoch
                epochIdx++;
                currAge = epochEndAge;
                epochEndAge = epochs[epochIdx];

            }
            double sr = rm.getSumOfRates( currState, counts, this->computeBranchRate( node.getIndex() ), currAge);
            lnL += -sr * (currAge - endAge);
        }
        else
        {
            double sr = rm.getSumOfRates( currState, counts, this->computeBranchRate( node.getIndex() ), currAge);
            lnL += -sr * (currAge - endAge);
        }


        return lnL;
    }


    // @Michael: My compiler complained about reaching the end of a non-void function. (Sebastian)
    return RbConstants::Double::neginf;
}



template<class charType>
double RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::computeTipLikelihood(const TopologyNode &node)
{
    double lnL = 0.0;
    size_t nodeIndex = node.getIndex();

    BranchHistory* bh = this->histories[nodeIndex];

    // update tip lnLs for ambiguous characters
    if (this->usingAmbiguousCharacters && node.isTip())
    {
        const std::vector<CharacterEvent*>& tipState = bh->getChildCharacters();
        for (size_t i = 0; i < tipState.size(); i++)
        {
            double v = this->tipProbs[nodeIndex][i];
            if (tipState[i]->getState() == 0)
                v = 1 - v;
            lnL += std::log(v);
        }
    }
    return lnL;
}

template<class charType>
const std::vector<double>& RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::getCladogenicStateFrequencies( void ) {

    if (cladogenicStateFreqs != NULL )
    {
        return cladogenicStateFreqs->getValue();
    }
    else
    {
        throw RbException("BUG: Reached end of a non-void function in BiogeographicTreeHistoryCtmc.");
    }

}

template<class charType>
unsigned RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::getEpochIndex(const std::vector<double>& epochs, double age)
{
    unsigned index = 0;
    while (age <= epochs[index] && index < epochs.size() - 1)
    {
        index++;
    };
    return index;
}

template<class charType>
unsigned RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::getEpochIndex(double age)
{
    unsigned index = 0;
    while (age <= epochs[index] && index < epochs.size() - 1)
    {
        index++;
    };
    return index;
}

template<class charType>
const std::vector<double>& RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::getRootFrequencies( void ) {

    if ( branchHeterogeneousSubstitutionMatrices || rootFrequencies != NULL )
    {
        return rootFrequencies->getValue();
    }
    else
    {
        throw RbException("BUG: Reached end of a non-void function in BiogeographicTreeHistoryCtmc.");
    }

}


template<class charType>
const bool RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::getUseTail(void) const
{
    return useTail;
}

template<class charType>
bool RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::historyContainsExtinction(const std::vector<CharacterEvent*>& currState, const std::multiset<CharacterEvent*,CharacterEventCompare>& history)
{
    std::multiset<CharacterEvent*,CharacterEventCompare>::iterator it_h;
    int n = (int)numOn(currState);
    if (n == 0)
        return true;

    for (it_h = history.begin(); it_h != history.end(); it_h++)
    {
        if ( (*it_h)->getState() == 0 )
            n--;
        else if ( (*it_h)->getState() == 1 )
            n++;

        if (n <= 0)
            return true;
    }

    return false;
}

template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::initializeTipValues( void )
{
    
    std::vector<TopologyNode*> nodes = AbstractTreeHistoryCtmc<charType>::tau->getValue().getNodes();
    
    // for all nodes
    for (size_t i = 0; i < nodes.size(); i++)
    {
        TopologyNode* node = nodes[i];
        
        // for tips only
        if (node->isTip())
        {
            DiscreteTaxonData<StandardState>& d = static_cast< DiscreteTaxonData<StandardState>& >( this->value->getTaxonData( node->getName() ) );
            std::vector<CharacterEvent*> tipState;
            
            // for all tips for the character
            for (size_t j = 0; j < d.getNumberOfCharacters(); j++)
            {
                // assign the binary state
                unsigned s = 0;
                if (!this->usingAmbiguousCharacters)
                    s = (unsigned)d[j].getStateIndex();
                else if (GLOBAL_RNG->uniform01() < this->tipProbs[node->getIndex()][j])
                    s = 1;

                CharacterEvent* evt = new CharacterEvent(j, s, 1.0);
                tipState.push_back( evt );
            }

            this->histories[node->getIndex()]->setChildCharacters(tipState);
            tipState.clear();
        }
    }
    this->tipsInitialized = true;
}

template<class charType>
size_t RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::numOn(const std::vector<CharacterEvent*>& s)
{
    size_t n = 0;
    for (size_t i = 0; i < s.size(); i++)
        if (s[i]->getState() == 1)
            n++;
    return n;
}


template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::drawInitValue( void )
{
    std::vector<TopologyNode*> nodes = AbstractTreeHistoryCtmc<charType>::tau->getValue().getNodes();

//    std::cout << this->tau->getValue() << "\n";

    // assign tip values
    initializeTipValues();

    // update all sites
    std::set<size_t> indexSet;
    for (size_t i = 0; i < this->numSites; i++)
        indexSet.insert(i);

    // sample node states
    for (size_t i = 0; i < nodes.size(); i++)
    {
        TopologyNode* nd = nodes[i];

        int samplePathEndCount = 0;
        do
        {
            samplePathEndCount++;
        } while (samplePathEnd(*nd,indexSet) == false && samplePathEndCount < 100);

        int samplePathStartCount = 0;
        do
        {
            samplePathStartCount++;
        } while (samplePathStart(*nd,indexSet) == false && samplePathStartCount < 100);
        
//        this->histories[ nd->getIndex() ]->print();

        ;
    }

    // sample paths
    for (size_t i = 0; i < nodes.size(); i++)
    {
        TopologyNode* nd = nodes[i];

        int samplePathHistoryCount = 0;
        do
        {

            samplePathHistoryCount++;
        } while (samplePathHistory(*nd,indexSet) == false && samplePathHistoryCount < 100);

        this->histories[i]->print();
    }
    
    double lnL = this->computeLnProbability();

    if (lnL == RbConstants::Double::neginf)
    {
        for (size_t i = 0; i < nodes.size(); i++)
        {
            this->fireTreeChangeEvent(*nodes[i]);
        }
        drawInitValue();
    }
}

template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::redrawValue( void )
{
    std::vector<TopologyNode*> nodes = AbstractTreeHistoryCtmc<charType>::tau->getValue().getNodes();

//    if (!true)
//    {
//
//        //    if (this->tipsInitialized == false)
//        if (this->dagNode->isClamped())
//            initializeTipValues();
//
//        std::set<size_t> indexSet;
//        for (size_t i = 0; i < this->numSites; i++)
//            indexSet.insert(i);
//
//        // sample node states
//                for (size_t i = 0; i < nodes.size(); i++)
//        {
//            TopologyNode* nd = nodes[i];
//
//            int samplePathEndCount = 0;
//            do
//            {
//                samplePathEndCount++;
//            } while (samplePathEnd(*nd,indexSet) == false && samplePathEndCount < 100);
//
//            int samplePathStartCount = 0;
//            do
//            {
//                samplePathStartCount++;
//            } while (samplePathStart(*nd,indexSet) == false && samplePathStartCount < 100);
//        }
//
//        // sample paths
//        for (size_t i = 0; i < nodes.size(); i++)
//        {
//            TopologyNode* nd = nodes[i];
//
//            int samplePathHistoryCount = 0;
//            do
//            {
//
//                samplePathHistoryCount++;
//            } while (samplePathHistory(*nd,indexSet) == false && samplePathHistoryCount < 100);
//
//    //        this->histories[i]->print();
//        }
//    }
//    else
    {
        // enabling this gives bad access errors -- std::set<CharacterHistory*> objects may not be copied properly?
        simulate();
    }

    double lnL = this->computeLnProbability();

    if (lnL == RbConstants::Double::neginf)
    {
        for (size_t i = 0; i < nodes.size(); i++)
        {
            this->fireTreeChangeEvent(*nodes[i]);
        }
        redrawValue();
    }
}



template<class charType>
bool RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::samplePathEnd(const TopologyNode& node, const std::set<size_t>& indexSet)
{
//    double lnP = 0.0;
    double parent_age = 0.0;
    if (node.isRoot())
    {
        parent_age = node.getAge() * 2;
    }
    else {
        parent_age = node.getParent().getAge();
    }
    

    if (node.isTip())
    {
        if (this->usingAmbiguousCharacters)
        {
            std::vector<CharacterEvent*> childState = this->histories[node.getIndex()]->getChildCharacters();
            for (std::set<size_t>::iterator it = indexSet.begin(); it != indexSet.end(); it++)
            {
                double u = GLOBAL_RNG->uniform01();
                unsigned s = ( u < this->tipProbs[node.getIndex()][*it] ? 1 : 0);
                childState[*it]->setState(s);
            }

            // forbid extinction
            if (numOn(childState) == 0 && forbidExtinction)
                samplePathEnd(node, indexSet);
        }
        else
        {
            ; // do nothing...
        }
    }
    else
    {
        TransitionProbabilityMatrix leftTpMatrix(this->numChars);
        TransitionProbabilityMatrix rightTpMatrix(this->numChars);
        TransitionProbabilityMatrix ancTpMatrix(this->numChars);

        const RateGeneratorSequence_Biogeography& rm = static_cast<const RateGeneratorSequence_Biogeography&>(homogeneousRateGeneratorSequence->getValue());

        // for sampling probs
        const std::vector<CharacterEvent*>& leftChildState  = this->histories[node.getChild(0).getIndex()]->getChildCharacters();
        const std::vector<CharacterEvent*>& rightChildState = this->histories[node.getChild(1).getIndex()]->getChildCharacters();

        // to update
        std::vector<CharacterEvent*> nodeChildState = this->histories[node.getIndex()]->getChildCharacters();
        for (std::set<size_t>::iterator it = indexSet.begin(); it != indexSet.end(); it++)
        {
            rm.calculateTransitionProbabilities(leftTpMatrix, node.getAge(), node.getChild(0).getAge(), this->computeBranchRate( node.getChild(0).getIndex() ), *it);
            rm.calculateTransitionProbabilities(rightTpMatrix, node.getAge(), node.getChild(1).getAge(), this->computeBranchRate( node.getChild(1).getIndex() ), *it);
            rm.calculateTransitionProbabilities(ancTpMatrix, parent_age, node.getAge(), this->computeBranchRate( node.getIndex() ), *it);

            
//            std::cout << "left\n" << leftTpMatrix << "\n\n";
//            std::cout << "right\n" <<  rightTpMatrix << "\n\n";
//            std::cout << "anc\n" << ancTpMatrix << "\n\n";
            
            size_t desS1 = leftChildState[*it]->getState();
            size_t desS2 = rightChildState[*it]->getState();
            size_t ancS = (size_t)(GLOBAL_RNG->uniform01() * 2);
            

            double u = GLOBAL_RNG->uniform01();
            double g0 = leftTpMatrix[0][desS1] * rightTpMatrix[0][desS2]; // * ancTpMatrix[ancS][0]; // mul by ancTpMatrix[uar][s] to enforce epochs
            double g1 = leftTpMatrix[1][desS1] * rightTpMatrix[1][desS2]; // * ancTpMatrix[ancS][1];

//            std::cout << desS1 << " " << desS2 << " " << ancS << " " << g0 << " " << g1 << "\n";
            unsigned int s = 0;
            if (u < g1 / (g0 + g1) && rm.isAreaAvailable(*it, node.getAge()))
                s = 1;

//            std::cout << s;

            nodeChildState[*it]->setState(s);


            ;
        }
//        std::cout << "\n";

        // forbid extinction
        if (numOn(nodeChildState) == 0 && forbidExtinction)
        {
            return false;
        }

    }
    return true;
}


template<class charType>
bool RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::samplePathHistory(const TopologyNode& node, const std::set<size_t>& indexSet)
{

    if (node.isRoot() && useTail == false)
        return true;

    // get model parameters
//    const Tree& tree = this->tau->getValue();
    double branchLength = node.getBranchLength();
    if (node.isRoot())
        branchLength = node.getAge() * 2;
    const RateGeneratorSequence_Biogeography& rm = static_cast<const RateGeneratorSequence_Biogeography&>(homogeneousRateGeneratorSequence->getValue());

    // begin update
    BranchHistory* bh = this->histories[ node.getIndex() ];

    // get epoch variables
    double startAge;

    if (node.isRoot())
        startAge = 2 * node.getAge();
    else
        startAge = node.getParent().getAge();
    double endAge = node.getAge();

    // reject sample path history
    std::vector<CharacterEvent*> parentVector = bh->getParentCharacters();
    std::vector<CharacterEvent*> childVector =  bh->getChildCharacters();
    std::multiset<CharacterEvent*,CharacterEventCompare> history;

    for (std::set<size_t>::iterator it = indexSet.begin(); it != indexSet.end(); it++)
    {
        std::set<CharacterEvent*> tmpHistory;
        size_t currState = parentVector[*it]->getState();
        size_t endState = childVector[*it]->getState();
        do
        {
            // delete previously rejected events
            tmpHistory.clear();

            // proceed with rejection sampling
            currState = parentVector[*it]->getState();

            double t = 0.0;

            double currAge = startAge;
            int epochIdx = getEpochIndex(startAge);
            double epochAge = epochs[epochIdx];

            // repeated rejection sampling
            do
            {
                size_t nextState = (currState == 1 ? 0 : 1);
                size_t charIdx = (*it);
                double r = rm.getSiteRate( currState, nextState, charIdx, this->computeBranchRate( node.getIndex() ), currAge);

                double dt = 0.0;
                if (r > 0.0)
                    dt = RbStatistics::Exponential::rv(r * branchLength, *GLOBAL_RNG);

                double da = dt * branchLength;

                // sample time from next interval (by memorylessness)
                if (currAge - da < epochAge || r == 0.0)
                {
                    t = (startAge - epochAge) / branchLength;
                    currAge = epochAge;
                    epochIdx++;
                    epochAge = epochs[epochIdx];
                }
                else
                {
                    t += dt;
                    currAge -= da;

                    if (t < 1.0)
                    {
                        currState = nextState;
                        CharacterEvent* evt = new CharacterEvent(*it, nextState, currAge);
                        tmpHistory.insert(evt);
                    }
                    else if (currState != endState)
                    {
                        for (std::set<CharacterEvent*>::iterator it_h = tmpHistory.begin(); it_h != tmpHistory.end(); it_h++)
                            delete *it_h;
                    }
                }
            }
            while(t < 1.0);
        }
        while (currState != endState);

        for (std::set<CharacterEvent*>::iterator it = tmpHistory.begin(); it != tmpHistory.end(); it++)
        {
            history.insert(*it);
        }
    }

//    bh->print();

    if (historyContainsExtinction(parentVector, history) == true && forbidExtinction)
    {
        for (std::multiset<CharacterEvent*,CharacterEventCompare>::iterator it_h = history.begin(); it_h != history.end(); it_h++)
            delete *it_h;
        history.clear();
        bh->clearEvents();

        return false;
//        redrawCount++;
//        samplePathHistory(node, indexSet);
    }
    else
    {
        bh->updateHistory(history,indexSet);
        return true;
//        bh->print();
    }
}


template<class charType>
bool RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::samplePathStart(const TopologyNode& node, const std::set<size_t>& indexSet)
{

    // ignore tips
    if (node.isTip())
        return true;

    // sample post-speciation path start states

    // 0: sympatric speciation, iid range inheritance
    // A = L v R, n(L) = n(R) = n(A), L ^ R = A

    // 1: peripatric speciation
    // A = L v R, n(R) = 1, n(L) = n(A), L ^ R = R

    // 2: allopatric speciation
    // A = L v R, n(R) = 1, n(L) = n(A) - 1, L ^ R = 0

    // sample bud and trunk lineages
    unsigned budChildIndex = (unsigned)(2 * GLOBAL_RNG->uniform01());
    unsigned trunkChildIndex = (budChildIndex == 0 ? 1 : 0);
    const TopologyNode& budNode = node.getChild(budChildIndex);
    const TopologyNode& trunkNode = node.getChild(trunkChildIndex);
    buddingState[ trunkNode.getIndex() ] = 0;
    buddingState[ budNode.getIndex() ] = 1;

    // sample bud area index
    const std::vector<CharacterEvent*>& nodeState = this->histories[ node.getIndex() ]->getChildCharacters();
    std::vector<unsigned> presentAreas;
    for (unsigned i = 0; i < nodeState.size(); i++)
        if (nodeState[i]->getState() == 1)
            presentAreas.push_back(i);

    unsigned budAreaIndex = 0;
    if (presentAreas.size() != 0)
        budAreaIndex = presentAreas[GLOBAL_RNG->uniform01() * presentAreas.size()];

    // update child states
    std::vector<CharacterEvent*> budState, trunkState;
    for (size_t i = 0; i < nodeState.size(); i++)
    {
        trunkState.push_back(new CharacterEvent(*nodeState[i]));

        // sympatry: iid
        if (cladogenicState[node.getIndex()] == 0 || cladogenicEvents == false)
            budState.push_back(new CharacterEvent(*nodeState[i]));

        // subset sympatry, allopatry: sparse
        else
            budState.push_back(new CharacterEvent(i,0, node.getAge()));
    }

    // subset sympatry: add bud area
    if (cladogenicState[node.getIndex()] == 1)
    {
        budState[budAreaIndex]->setState(1);
    }

    // allopatry: add bud area, del trunk area
    else if (cladogenicState[node.getIndex()] == 2)
    {
        trunkState[budAreaIndex]->setState(0);
        budState[budAreaIndex]->setState(1);
    }

    this->histories[ trunkNode.getIndex() ]->setParentCharacters( trunkState );
    this->histories[ budNode.getIndex() ]->setParentCharacters( budState );

    // sample sub-root state as necessary
    if (node.isRoot())
    {
        TransitionProbabilityMatrix nodeTpMatrix(this->numSites);
        double parent_age = node.getAge() * 2;


        // for sampling probs
//        const std::vector<CharacterEvent*>& nodeChildState = this->histories[node.getIndex() ]->getChildCharacters();

        // to update
        std::vector<CharacterEvent*> nodeParentState = this->histories[node.getIndex()]->getParentCharacters();
        for (std::set<size_t>::iterator it = indexSet.begin(); it != indexSet.end(); it++)
        {
//            homogeneousRateGeneratorSequence->getValue().calculateTransitionProbabilities( node.getParent().getAge(), node.getAge(), this->computeBranchRate( node.getIndex() ), nodeTpMatrix, *it);
            homogeneousRateGeneratorSequence->getValue().calculateTransitionProbabilities( nodeTpMatrix, parent_age, node.getAge(), this->computeBranchRate( node.getIndex() ) );
//            unsigned int desS1 = nodeChildState[*it]->getState();

            //            double u = GLOBAL_RNG->uniform01();
            //            double g0 = nodeTpMatrix[0][desS1];
            //            double g1 = nodeTpMatrix[1][desS1];
            //
            unsigned int s = 0;
            //            if (u < g1 / (g0 + g1))

            s = 1;

            nodeParentState[*it]->setState(s);
        }

        // forbid extinction
        if (numOn(nodeParentState) == 0 && forbidExtinction)
            return false;
        else
            this->histories[node.getIndex()]->setParentCharacters(nodeParentState);

    }


    return true;
}


template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::setCladogenicStateFrequencies(const TypedDagNode< RbVector< double > > *csf) {

    // remove the old parameter
    this->removeParameter( cladogenicStateFreqs );

    if (csf != NULL)
    {
        cladogenicEvents = true;
        cladogenicStateFreqs = csf;
    }
    else
    {
        cladogenicEvents = false;
        cladogenicStateFreqs = NULL;

    }

    // add the new parameter
    this->addParameter( cladogenicStateFreqs );

    // redraw the current value
    if ( this->dag_node != NULL && !this->dag_node->isClamped() )
    {
        this->redrawValue();
    }

}


template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::setRateGeneratorSequence(const TypedDagNode< RateGeneratorSequence > *rm)
{

    // remove the old parameter first
    if ( homogeneousRateGeneratorSequence != NULL )
    {
        this->removeParameter( homogeneousRateGeneratorSequence );
        homogeneousRateGeneratorSequence = NULL;
    }
    else
    {
        this->removeParameter( heterogeneousRateGeneratorSequences );
        heterogeneousRateGeneratorSequences = NULL;
    }

    // set the value
    branchHeterogeneousSubstitutionMatrices = false;
    homogeneousRateGeneratorSequence = rm;
    epochs = static_cast<const RateGeneratorSequence_Biogeography&>(rm->getValue()).getEpochs();

    // add the new parameter
    this->addParameter( homogeneousRateGeneratorSequence );

    // redraw the current value
    if ( this->dag_node != NULL && !this->dag_node->isClamped() )
    {
        this->redrawValue();
    }

}


template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::setRateGeneratorSequence(const TypedDagNode< RbVector< RateGeneratorSequence > > *rm) {

    // remove the old parameter first
    if ( homogeneousRateGeneratorSequence != NULL )
    {
        this->removeParameter( homogeneousRateGeneratorSequence );
        homogeneousRateGeneratorSequence = NULL;
    }
    else
    {
        this->removeParameter( heterogeneousRateGeneratorSequences );
        heterogeneousRateGeneratorSequences = NULL;
    }

    // set the value
    branchHeterogeneousSubstitutionMatrices = true;
    heterogeneousRateGeneratorSequences = rm;
    epochs = static_cast<const RateGeneratorSequence_Biogeography&>(rm->getValue()[0]).getEpochs();

    // add the parameter
    this->addParameter( heterogeneousRateGeneratorSequences );

    // redraw the current value
    if ( this->dagNode != NULL && !this->dagNode->isClamped() )
    {
        this->redrawValue();
    }

}


template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::setRootFrequencies(const TypedDagNode< RbVector< double > > *f) {

    // remove the old parameter first
    this->removeParameter( rootFrequencies );

    if ( f != NULL )
    {
        // set the value
        rootFrequencies = f;

        // add the parameter
        this->addParameter( rootFrequencies );
    }
    else
    {
        branchHeterogeneousSubstitutionMatrices = false;
    }

    // add the new parameter
    this->addParameter( rootFrequencies );

    // redraw the current value
    if ( this->dagNode != NULL && !this->dagNode->isClamped() )
    {
        this->redrawValue();
    }
}


template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::setSiteRates(const TypedDagNode< RbVector< double > > *r)
{

    // remove the old parameter first
    this->removeParameter( siteRates );

    if ( r != NULL )
    {
        // set the value
        rateVariationAcrossSites = true;
        siteRates = r;
        this->numSiteRates = r->getValue().size();
        this->resizeLikelihoodVectors();
    }
    else
    {
        // set the value
        rateVariationAcrossSites = false;
        siteRates = NULL;
        this->numSiteRates = 1;
        this->resizeLikelihoodVectors();

    }

    // add the new parameter
    this->addParameter( siteRates );

    // redraw the current value
    if ( this->dagNode != NULL && !this->dagNode->isClamped() )
    {
        this->redrawValue();
    }
}

template<class charType>
const std::vector<int>& RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::getBuddingStates(void)
{
    return buddingState;
}

template<class charType>
const std::vector<int>& RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::getCladogenicStates(void)
{
    return cladogenicState;
}


template<class charType>
int RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::getBuddingState(const TopologyNode& nd)
{
    return buddingState[nd.getIndex()];
}

template<class charType>
int RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::getCladogenicState(const TopologyNode& nd)
{
    return cladogenicState[nd.getIndex()];
}

template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::setBuddingState(const TopologyNode& nd, int s)
{
    buddingState[nd.getIndex()] = s;
}

template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::setCladogenicState(const TopologyNode& nd, int s)
{
    cladogenicState[nd.getIndex()] = s;
}

template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::simulate(void)
{

    this->RevBayesCore::AbstractTreeHistoryCtmc<charType>::simulate();
}

template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::simulateCladogenesis(const TopologyNode& nd)
{

    const std::vector<CharacterEvent*>& nodeChildState = this->histories[ nd.getIndex() ]->getChildCharacters();
    const std::vector<TopologyNode*>& children = nd.getChildren();

    // draw bud/trunk states
    size_t bs_idx = GLOBAL_RNG->uniform01() * children.size();
    for (size_t i = 0; i < children.size(); i++)
        buddingState[ children[i]->getIndex() ] = 0;
    buddingState[ children[bs_idx]->getIndex() ] = 1;

    // draw cladogenic states
    double u_csf = GLOBAL_RNG->uniform01();
    int cs = BiogeographicCladoEvent::SYMPATRY_NARROW;
    const std::vector<double>& csf = cladogenicStateFreqs->getValue();
    for (unsigned i = 0; i < csf.size(); i++)
    {

        u_csf -= csf[i];
        if (u_csf <= 0.0)
        {
            cs = i + 1;
            break;
        }
    }

//    cs = 0;

    // narrow A|A
    if (numOn(nodeChildState) == 1)
    {
        cs = 0;
        for (size_t i = 0; i < children.size(); i++)
        {
            std::vector<CharacterEvent*> childParentState;
            for (size_t j = 0; j < nodeChildState.size(); j++)
                childParentState.push_back(new CharacterEvent(j, nodeChildState[j]->getState(), 0.0));
            this->histories[ children[i]->getIndex() ]->setParentCharacters(childParentState);
        }
    }
    // wide ABCD|ABCD
    else if (cs == BiogeographicCladoEvent::SYMPATRY_WIDESPREAD || !cladogenicEvents)
    {
        for (size_t i = 0; i < children.size(); i++)
        {
            std::vector<CharacterEvent*> childParentState;
            for (size_t j = 0; j < nodeChildState.size(); j++)
            {
//                std::cout << nodeChildState[j]->getState();
                childParentState.push_back(new CharacterEvent(j, nodeChildState[j]->getState(), 0.0));
            }
//            std::cout << "\n";
            this->histories[ children[i]->getIndex() ]->setParentCharacters(childParentState);
//            this->histories[ children[i]->getIndex() ]->print();
        }
    }

    // subset A|ABCD
    else if (cs == BiogeographicCladoEvent::SYMPATRY_SUBSET)
    {
        std::set<size_t> present;
        for (size_t i = 0; i < nodeChildState.size(); i++)
        {
            if (nodeChildState[i]->getState() == 1)
                present.insert(i);
        }
        size_t w = GLOBAL_RNG->uniform01() * present.size();
        std::set<size_t>::iterator it = present.begin();
        std::advance(it, w);
        size_t budAreaIdx = *it;

        for (size_t i = 0; i < children.size(); i++)
        {
            std::vector<CharacterEvent*> childParentState;

            if ( buddingState[ children[i]->getIndex() ] == 0 )
            {
                for (size_t j = 0; j < nodeChildState.size(); j++)
                    childParentState.push_back(new CharacterEvent(j, nodeChildState[j]->getState(), 0.0));
            }
            else
            {
                for (size_t j = 0; j < nodeChildState.size(); j++)
                {
                    int s = 0;
                    if (j == budAreaIdx)
                        s = 1;
                    childParentState.push_back(new CharacterEvent(j, s, 0.0));
                }

            }
            this->histories[ children[i]->getIndex() ]->setParentCharacters(childParentState);
        }
    }

    // allopatry AB|CD
    else if (cs == BiogeographicCladoEvent::ALLOPATRY)
    {
        std::vector<unsigned> trunkAreas(this->numSites, 0);
        std::vector<unsigned> budAreas(this->numSites, 0);
        for (size_t i = 0; i < nodeChildState.size(); i++)
        {
            if (nodeChildState[i]->getState() == 1)
            {
                if (GLOBAL_RNG->uniform01() < 0.5)
                    trunkAreas[i] = 1;
                else
                    budAreas[i] = 1;
            }
        }
        for (size_t i = 0; i < children.size(); i++)
        {
            std::vector<CharacterEvent*> childParentState;
            for (unsigned j = 0; j < nodeChildState.size(); j++)
            {
                if ( buddingState[ children[i]->getIndex() ] == 0 )
                    childParentState.push_back(new CharacterEvent(j, trunkAreas[j], 0.0));
                else if ( buddingState[ children[i]->getIndex() ] == 1 )
                    childParentState.push_back(new CharacterEvent(j, budAreas[j], 0.0));
            }
            this->histories[ children[i]->getIndex() ]->setParentCharacters(childParentState);
        }
    }
    else
    {
        throw RbException("ERROR: simulateCladogenesis did not update parent characters of child nodes.");
    }
    cladogenicState[ children[0]->getIndex() ] = cs;
    cladogenicState[ children[1]->getIndex() ] = cs;
}

template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::simulateHistory(const TopologyNode& node, BranchHistory* bh)
{
    size_t nodeIndex = node.getIndex();

    const RateGeneratorSequence_Biogeography* rm;
    if (branchHeterogeneousSubstitutionMatrices)
        rm = &static_cast<const RateGeneratorSequence_Biogeography&>(heterogeneousRateGeneratorSequences->getValue()[nodeIndex]);
    else
        rm = &static_cast<const RateGeneratorSequence_Biogeography&>(homogeneousRateGeneratorSequence->getValue());

    // get parent BranchHistory state
    std::vector<CharacterEvent*> currState = bh->getParentCharacters();

    // simulate path
//    double sum_rates = rm->getSiteRate(node, 0, 1) + rm->getSiteRate(node, 1, 0);
    std::set<CharacterEvent*,CharacterEventCompare> history;
    double startAge = ( node.isRoot() ? node.getAge() * 5 : node.getParent().getAge() );
    double branchLength = startAge - node.getAge();


    bool failed = false;
    std::vector<double> rates(this->numSites, 0.0);
    std::multiset<CharacterEvent*> tmpHistory;
    do
    {
        // delete previously rejected events
        tmpHistory.clear();
        failed = false;
        int n1 = (int)numOn(currState);


        // simulate
        double t = 0.0;

        double currAge = startAge;
        int epochIdx = getEpochIndex(startAge);
        double epochAge = epochs[epochIdx];

        while (t < 1.0)
        {
            // get competing event rates
            double r = 0.0;
            for (size_t i = 0; i < this->numSites; i++)
            {
                unsigned s = ( currState[i]->getState() == 0 ? 1 : 0 );
                CharacterEvent nextState(i, s, currAge);
                rates[i] = rm->getRate( currState, &nextState, this->computeBranchRate( node.getIndex() ), currAge);
                r += rates[i];
            }

            // next event time
            double dt = 0.0;
            if (r > 0.0)
                dt = RbStatistics::Exponential::rv(r * branchLength, *GLOBAL_RNG);
            double da = dt * branchLength;

            // sample time from next interval (by memorylessness)
            if (currAge - da < epochAge || r == 0.0)
            {
                t = (startAge - epochAge) / branchLength;
                currAge = epochAge;
                epochIdx++;
                epochAge = epochs[epochIdx];
            }
            // or, within this interval
            else
            {
                t += dt;
                currAge -= da;

                if (t < 1.0)
                {
                    CharacterEvent* evt = NULL;
                    double u = GLOBAL_RNG->uniform01() * r;
//                    std::cout << u << " " << r << "\n";
                    for (size_t i = 0; i < rates.size(); i++)
                    {
                        u -= rates[i];
                        if (u <= 0.0)
                        {
                            unsigned s = 0;
                            if (currState[i]->getState() == 0)
                                s = 1;

                            if (s == 0)
                                n1--;
                            else if (s == 1)
                                n1++;
                            if (n1 == 0)
                                failed = true;

//                            std::cout << i << " " << s << " " << t << " " << n1 << "\n";
                            evt = new CharacterEvent(i,s,t);
                            break;
                        }
                    }
                    tmpHistory.insert(evt);
                    currState[ evt->getSiteIndex() ] = evt;
                }
            }
        }

        if (failed)
        {
            for (std::multiset<CharacterEvent*>::iterator it_h = tmpHistory.begin(); it_h != tmpHistory.end(); it_h++)
                delete *it_h;
        }

    } while (failed);

    for (std::multiset<CharacterEvent*>::iterator it = tmpHistory.begin(); it != tmpHistory.end(); it++)
    {
        history.insert(*it);
    }
    bh->setHistory(history);

    for (size_t i = 0; i < this->numSites; i++)
    {
        size_t s = currState[i]->getState();
        currState[i] = new CharacterEvent(i, s, 1.0);
    }

    bh->setChildCharacters(currState);

}

template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::simulate(const TopologyNode& node, BranchHistory* bh, std::vector< DiscreteTaxonData< charType > >& taxa)
{

//    RandomNumberGenerator* rng = GLOBAL_RNG;

    // get the sequence of this node
    size_t nodeIndex = node.getIndex();

    // get rate map for branch leading to node
    const RateGeneratorSequence_Biogeography* rm;
    if (branchHeterogeneousSubstitutionMatrices)
        rm = &static_cast<const RateGeneratorSequence_Biogeography&>(heterogeneousRateGeneratorSequences->getValue()[nodeIndex]);
    else
        rm = &static_cast<const RateGeneratorSequence_Biogeography&>(homogeneousRateGeneratorSequence->getValue());

    // if root, set tail state
    if (node.isRoot())
    {
        std::vector<CharacterEvent*> parentState;
        for (size_t i = 0; i < this->numSites; i++)
        {
            double r01 = rm->getSiteRate(nodeIndex, 0, 1, (unsigned)i, node.getAge());
            double r10 = rm->getSiteRate(nodeIndex, 1, 0, (unsigned)i, node.getAge());
            double pi1 = r01 / (r01 + r10);

            unsigned s = 0;
            if (rm->isAreaAvailable(i, node.getAge()) && GLOBAL_RNG->uniform01() < pi1)
                s = 1;
            parentState.push_back(new CharacterEvent(i, s, node.getAge() * 5));
        }

        bh->setParentCharacters(parentState);
    }

    // simulate anagenic changes
    simulateHistory(node, bh);

    const std::vector<CharacterEvent*>& childState = bh->getChildCharacters();
    size_t n1 = 0;
    for ( size_t i = 0; i < this->numSites; ++i )
    {
        // create the character
        charType c;
        std::string s = "0";
        if (childState[i]->getState() == 1)
        {
            s = "1";
            n1++;
        }
        c.setState( s );

        // add the character to the sequence
        taxa[nodeIndex].addCharacter( c );
    }
//    std::cout << nodeIndex << " " << n1 << "\n";

    if ( node.isTip() )
    {
//        std::cout << "adding " << node.getName() << "\n";
        taxa[nodeIndex].setTaxon( node.getTaxon() );
    }
    else
    {
        const std::vector<TopologyNode*>& children = node.getChildren();
        for (size_t i = 0; i < children.size(); i++)
        {
            this->histories[ children[i]->getIndex() ] = new BranchHistory(this->numSites, this->numChars, children[i]->getIndex() );
        }

//        if (cladogenicEvents)
            simulateCladogenesis(node);

        for (size_t i = 0; i < children.size(); i++)
        {
            BranchHistory* bh_ch = this->histories[ children[i]->getIndex() ];
            simulate( *children[i], bh_ch, taxa );
        }
    }
//    bh->print();
}


/** Swap a parameter of the distribution */
template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::swapParameterInternal( const DagNode *oldP, const DagNode *newP )
{
    if (oldP == homogeneousRateGeneratorSequence)
    {
        homogeneousRateGeneratorSequence = static_cast<const TypedDagNode< RateGeneratorSequence >* >( newP );
    }
    else if (oldP == heterogeneousRateGeneratorSequences)
    {
        heterogeneousRateGeneratorSequences = static_cast<const TypedDagNode< RbVector< RateGeneratorSequence > >* >( newP );
    }
    else if (oldP == rootFrequencies)
    {
        rootFrequencies = static_cast<const TypedDagNode< RbVector< double > >* >( newP );
    }
    else if (oldP == siteRates)
    {
        siteRates = static_cast<const TypedDagNode< RbVector< double > >* >( newP );
    }
    else if (oldP == cladogenicStateFreqs)
    {
        cladogenicStateFreqs = static_cast<const TypedDagNode< RbVector< double > >* >( newP );
    }
    else
    {
        AbstractTreeHistoryCtmc<charType>::swapParameterInternal(oldP,newP);
    }

}

template<class charType>
void RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::touchSpecialization( DagNode* affecter, bool touchAll )
{

    // if the topology wasn't the culprit for the touch, then we just flag everything as dirty
    if ( affecter == rootFrequencies || affecter == rootFrequencies )
    {

        const TopologyNode &root = this->tau->getValue().getRoot();
        this->flagNodeDirty( root );
    }
    else
    {
        AbstractTreeHistoryCtmc<charType>::touchSpecialization( affecter, touchAll );
    }

}

template<class charType>
const bool RevBayesCore::BiogeographicTreeHistoryCtmc<charType>::useCladogenicEvents(void) const
{
    return cladogenicEvents;
}

#endif /* defined(__rb_mlandis__BiogeographicTreeHistoryCtmc__) */
