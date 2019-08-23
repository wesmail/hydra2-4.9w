// @(#)$Id: hpidalgorithm.cc,v 1.2 2002-11-20 17:28:18 jaskula Exp $
//*-- Author   : Marcin Jaskula 27/07/2002
//*-- Modified : Marcin Jaskula 11/10/2002
//               - bMerge flag added
//*-- Modified : Marcin Jaskula 29/10/2002
//               - calculatePDF and calculateCL added

////////////////////////////////////////////////////////////////////////////////
//
// HPidAlgorithm
//
// A base class for all PID algorithms
// The algorithms are recognized by the id (iAlgId) or/and by the name (sName).
// The member bMerge define if the values returned by algorithm should
// included into calculations of the merged vectors.
// Flag fWeight is not used up to know, but is dedicated for the future
// extensions
//
////////////////////////////////////////////////////////////////////////////////

#pragma implementation

#include "hpidreconstructor.h"
#include "hpidalgorithm.h"

// -----------------------------------------------------------------------------

ClassImp(HPidAlgorithm);

// -----------------------------------------------------------------------------

HPidAlgorithm::HPidAlgorithm(void)
{
// Default constructor

    iAlgId  = algNotSet;
    pRec    = NULL;
    fWeight = 1.0f;
    bMerge  = kTRUE;
}

// -----------------------------------------------------------------------------

HPidAlgorithm::HPidAlgorithm(TString sName, Int_t iAlgId)
{
// Constructor with the name and id of the algorithm

    this->sName  = sName;
    this->iAlgId = iAlgId;
    pRec         = NULL;
    fWeight      = 1.0f;
    bMerge       = kTRUE;
}

// -----------------------------------------------------------------------------

HPidAlgorithm::HPidAlgorithm(TString sName, Int_t iAlgId, Float_t fWeight)
{
// Constructor with the name, id and weight of the algorithm

    this->sName   = sName;
    this->iAlgId  = iAlgId;
    pRec          = NULL;
    this->fWeight = fWeight;
    bMerge        = kTRUE;
}

// -----------------------------------------------------------------------------

HPidAlgorithm::~HPidAlgorithm(void)
{
// Destructor. The algorithm calls pRec->removeAlgorithm(this) to remove
// itself from the list of the algorithms.

    if(pRec != NULL)
    {
        pRec->removeAlgorithm(this);
        pRec = NULL;
    }
}

// -----------------------------------------------------------------------------

Bool_t HPidAlgorithm::init(void)
{
// Abstract method for initialization. Returns status of the initialization.

    Error("init", "Abstract method");
    return kFALSE;
}

// -----------------------------------------------------------------------------

Bool_t HPidAlgorithm::reinit(void)
{
// Abstract method for reinitialization. Returns status of the reinitialization.

    Error("reinit", "Abstract method");
    return kFALSE;
}

// -----------------------------------------------------------------------------

Bool_t HPidAlgorithm::finalize(void)
{
// Abstract method for finalizing. Returns status of the finalizing.

    Error("finalizing", "Abstract method");
    return kFALSE;
}

// -----------------------------------------------------------------------------

Bool_t HPidAlgorithm::execute(HPidTrackCand *pTrack,
            Float_t afReturnPDF[], Float_t afReturnCL[], Short_t &nCatIndex)
{
// Method for pid finding.
// pTrack      - pointer to the track candidate
// afReturnPDF - array for PDF values; the size taken from the reconstructor
// afReturnCL  - array for CL values; the size taken from the reconstructor
// nCatIndex   - index of an object in category used by algorithm
//               (eg. HShowerPid) default set to -1
//
// Calls calculatePDF() and calculateCL() method
// Returns status of the calculations.

Bool_t  bReturn = kTRUE;
Bool_t  b;
Short_t nDummy;

    if(afReturnPDF != NULL)
    {
        nDummy = -1;

        if(((b = calculatePDF(pTrack, afReturnPDF, nDummy)) == kTRUE)
            && (nDummy >= 0))
        {
            nCatIndex = nDummy;
        }

        bReturn &= b;
    }

    if(afReturnCL != NULL)
    {
        nDummy = -1;

        if(((b = calculateCL(pTrack, afReturnCL, nDummy)) == kTRUE)
            && (nDummy >= 0))
        {
            nCatIndex = nDummy;
        }

        bReturn &= b;
    }

    return bReturn;
}

// -----------------------------------------------------------------------------

Bool_t HPidAlgorithm::calculatePDF(HPidTrackCand *pTrack,
            Float_t afReturn[], Short_t &nCatIndex)
{
// Abstract method for calculating PDF (returns kTRUE)
// pTrack    - pointer to the track candidate
// afReturn  - array for return values; the size taken from the reconstructor
// nCatIndex - index of an object in category used by algorithm (eg. HShowerPid)
//             default set to -1
//
// Returns status of the calculations.

    return kTRUE;
}

// -----------------------------------------------------------------------------

Bool_t HPidAlgorithm::calculateCL(HPidTrackCand *pTrack,
            Float_t afReturn[], Short_t &nCatIndex)
{
// Abstract method for calculating CL (returns kTRUE)
// For description see calculatePDF()

    return kTRUE;
}

// -----------------------------------------------------------------------------

void HPidAlgorithm::print(void) const
{
// Empty function for printing info of the algorithm
}