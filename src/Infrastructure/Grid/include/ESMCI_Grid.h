// $Id: ESMCI_Grid.h,v 1.50 2008/10/16 21:26:19 peggyli Exp $
//
// Earth System Modeling Framework
// Copyright 2002-2008, University Corporation for Atmospheric Research, 
// Massachusetts Institute of Technology, Geophysical Fluid Dynamics 
// Laboratory, University of Michigan, National Centers for Environmental 
// Prediction, Los Alamos National Laboratory, Argonne National Laboratory, 
// NASA Goddard Space Flight Center.
// Licensed under the University of Illinois-NCSA License.

// ESMCI Grid include file for C++

// (all lines below between the !BOP and !EOP markers will be included in
//  the automated document processing.)
//-------------------------------------------------------------------------
// these lines prevent this file from being read more than once if it
// ends up being included multiple times

#ifndef ESMCI_Grid_H
#define ESMCI_Grid_H

//-------------------------------------------------------------------------
//BOP
// !CLASS: ESMCI_Grid - Grid
//
// !DESCRIPTION:
//
// The code in this file defines the C++ {\tt Grid} members and method
// signatures (prototypes).  The companion file {\tt ESMC\_Grid.C}
// contains the full code (bodies) for the {\tt Grid} methods.
//
///EOP
//-------------------------------------------------------------------------


#include "ESMC_Base.h"
#include "ESMCI_DistGrid.h"
#include "ESMCI_Array.h"


// Eventually move this to ESMCI_Util.h
// Eventually move this to ESMCI_Util.h
enum ESMC_GridStatus {ESMC_GRIDSTATUS_INVALID=-1,
                      ESMC_GRIDSTATUS_UNINIT,
                      ESMC_GRIDSTATUS_NOT_READY,
		      ESMC_GRIDSTATUS_SHAPE_READY
};

enum ESMC_GridDecompType {ESMC_GRID_INVALID=1, 
			ESMC_GRID_NONARBITRARY,
			ESMC_GRID_ARBITRARY
};

enum  ESMC_GridConn {ESMC_GRIDCONN_NONE=0,
                     ESMC_GRIDCONN_PERIODIC,
                     ESMC_GRIDCONN_POLE,
                     ESMF_GRIDCONN_BIPOLE};

#define ESMC_GRID_ARBDIM  -2

// Start name space
namespace ESMCI {  

class Grid;
class ProtoGrid;

// class definition
class Grid : public ESMC_Base {    // inherits from ESMC_Base class
  private:

  // holder for set/commit data
  ProtoGrid *proto;

  // Grid Status
  ESMC_GridStatus status;

  ESMC_GridDecompType decompType;

  // type information
  ESMC_TypeKind typekind;

  int distDimCount;

  int *distgridToGridMap;    // size of distgridToGridMap = distDimCount, entries are 0-based
  int undistDimCount;
  int *undistLBound; // size of undistLBound = undistDimCount
  int *undistUBound; // size of undistUBound = undistDimCount
  
  int dimCount;

  // global grid dimension
  int *minIndex;
  int *maxIndex;

  // Topology information
  ESMC_GridConn *connL;  // size of Grid rank
  ESMC_GridConn *connU;  // size of Grid rank
  
  // information about edge widths of the Grid
  int *gridEdgeLWidth; // size of grid dimCount
  int *gridEdgeUWidth; // size of grid dimCount
  int *gridAlign; // size of grid dimCount

  // User info about coord dimCounts and distgridToGridMap
  int *coordDimCount; // size of coordDimCount = dimCount
  int **coordDimMap; // size of coordDimMap = dimCountxdimCount  [coord][dim of coord array], 0-based

  // Index array for arbitrarily distributed grid
  int localCount;   // number of local cells
  int **coordLocalIndices;  // 2D array holding the local indices [localCount, distRank]

  int staggerLocCount;
  Array ***coordArrayList; // size of coordArrayList = staggerLocCountxdimCount [staggerLoc][coord]
  int   **staggerAlignList;     // hold alignment info [staggerloc][dim]
  int   **staggerEdgeLWidthList;     // hold LWidth info [staggerloc][dim]
  int   **staggerEdgeUWidthList;     // hold UWidth info [staggerloc][dim]
  bool  **didIAllocList;        // if true, I allocated this Array [staggerloc][coord]

  Array *maskArray; // TODO: this needs to be a list per staggerloc, but while DAvid is finishing up....

  // map grid dim to distgrid dim and grid bounds dim
  bool *gridIsDist;  // size=dimCount [grid-dim]
  int *gridMapDim;   // size=dimCount [grid-dim]

  // map coord dim to distgrid dim and coord array bounds dim
  bool **coordIsDist; // size=dimCountxdimCount [coord][coord-dim]
  int **coordMapDim; // size=dimCountxdimCount [coord][coord-dim]

  // if bit d in isDELBnd[localde] is 1, then localde is on the
  // lower boundary of dimension d
  char *isDELBnd;
 
 // if bit d in isDEUBnd[localde] is 1, then localde is on the
  // upper boundary of dimension d
  char *isDEUBnd;

  // if true, then destroy the distgrid with the grid
  bool destroyDistgrid;

  // if true, then destroy the DELayout with the grid
  bool destroyDELayout;


  ESMC_IndexFlag indexflag;
  DistGrid *distgrid;


  // Private methods:
  // Set internal array
  int setCoordArrayInternal(
		    int _staggerloc, // (in)
		    int _coord,      // (in)
		    Array *_array,   // (in)
		    bool _self_alloc // (in)
		    );

  // Get Array holding coordinates
 int getCoordArrayInternal(
		    int _staggerloc, // (in)
		    int _coord,      // (in)
		    Array **_array   // (in)
		    );

  // add/delete protogrid  
  int addProtoGrid();
  int delProtoGrid();

  // make grid usable
  int constructInternal(
       char *name,                            // (in)
       ESMC_TypeKind typekindArg,              // (in)
       DistGrid *distgridArg,                  // (in)
       int distDimCountArg,                        // (in)
       int *distgridToGridMapArg,                         // (in)
       int undistDimCountArg,                      // (in)
       int *undistLBoundArg,                        // (in)
       int *undistUBoundArg,                        // (in)
       int dimCountArg,                            // (in)
       int *gridEdgeLWidth,
       int *gridEdgeUWidth,
       int *gridAlign,
       int *coordDimCountArg,                     // (in)
       int **coordDimMapArg,                   // (in)
       ESMC_IndexFlag indexflagArg,             // (in)
       int *minIndexArg,                       // (in)
       int *maxIndexArg,                       // (in)
       int **localIndicesArg,                       // (in)
       int localCount,                       // (in)
       int arbDim,                           // (in)
       bool destroyDistgrid,
       bool destroyDELayout
       );

  // set information about a stagger location
  int setStaggerInfo(
		    int _staggerloc, // (in)
		    int *staggerAlign,  // (in)
		    int *staggerEdgeLWidth,  // (in)
		    int *staggerEdgeUWidth  // (in)
		    );


 public:

  // accessor methods
  // NOTE: For efficiencies sake the following functions don't error check
  //       so be sure to error check the grid or any input before using.  
  ESMC_GridDecompType getDecompType(void) const {return decompType;}
  void setDecompType(ESMC_GridDecompType type) {decompType=type;}
  ESMC_GridStatus getStatus(void) const {return status;}
  const ESMC_GridConn *getConnL(void) const {return connL;}
  const ESMC_GridConn *getConnU(void) const {return connU;} 
  int getDimCount(void) const {return dimCount;}
  int getDistDimCount(void) const {return distDimCount;}
  int getUndistDimCount(void) const {return undistDimCount;}
  int getTileCount(void) const {return distgrid->getPatchCount();}
  int getStaggerLocCount(void) const {return staggerLocCount;}
  ESMC_IndexFlag getIndexFlag(void) const {return indexflag;}
  ESMC_TypeKind getTypeKind(void) const {return typekind;}
  const DistGrid *getDistGrid(void) const {return distgrid;}
  const int *getDistgridToGridMap(void) const {return distgridToGridMap;}
  const int *getUndistLBound(void) const {return undistLBound;}
  const int *getUndistUBound(void) const {return undistUBound;}
  const int *getCoordDimCount(void) const {return coordDimCount;}
  const int *getGridEdgeLWidth(void) const {return gridEdgeLWidth;}
  const int *getGridEdgeUWidth(void)  const {return gridEdgeUWidth;}
  const int *getGridAlign(void) const {return gridAlign;}
        int getLocalIndexCount(void) const {return localCount;}
        int **getCoordDimMap(void) const {return coordDimMap;}
  const char *getName(void)  const {return ESMC_BaseGetName();}
        bool *getGridIsDist(void) const {return gridIsDist;} 
        int  *getGridMapDim(void) const {return gridMapDim;} 
        bool **getCoordIsDist(void) const {return coordIsDist;} 
        int  **getCoordMapDim(void) const {return coordMapDim;} 
  const int   *getStaggerEdgeLWidth(int staggerloc) const {return staggerEdgeLWidthList[staggerloc];}
  const int   *getStaggerEdgeUWidth(int staggerloc) const {return staggerEdgeUWidthList[staggerloc];}
  const int   *getMinIndex(int tileno) const { return minIndex; }
  const int   *getMaxIndex(int tileno) const { return maxIndex; }
        int  **getLocalIndices(void) const { return coordLocalIndices;}

  Array *getMaskArray() {return maskArray;}

  int setMaskArray(Array *array) {maskArray=array; return ESMF_SUCCESS;}

  // Use these when DistGrid topology stuff is in place
  // bool isLBnd(int localDE, int dim) {return (isDELBnd[localDE] & (0x1 << dim))?true:false;}
  // bool isUBnd(int localDE, int dim) {return (isDEUBnd[localDE] & (0x1 << dim))?true:false;}

  // Temporary and will go away soon
  bool isSphere() { return connL[0]==ESMC_GRIDCONN_PERIODIC && connU[0]==ESMC_GRIDCONN_PERIODIC &&
                           connL[1]==ESMC_GRIDCONN_POLE && connU[1]==ESMC_GRIDCONN_POLE; }
  bool isLBndNT(int localDE, int dim) {return (isDELBnd[localDE] & (0x1 << dim))?true:false;}
  bool isUBndNT(int localDE, int dim) {return (isDEUBnd[localDE] & (0x1 << dim))?true:false;}

  bool isLBnd(int localDE, int dim) {return ((connL[dim]!=ESMC_GRIDCONN_PERIODIC)&&(isDELBnd[localDE] & (0x1 << dim)))?true:false;}
  bool isUBnd(int localDE, int dim) {return ((connU[dim]!=ESMC_GRIDCONN_PERIODIC)&&(isDEUBnd[localDE] & (0x1 << dim)))?true:false;}


  // Temporary create sphere until I have topology setting worked out 
  void setSphere() {connL[0]=ESMC_GRIDCONN_PERIODIC; connU[0]=ESMC_GRIDCONN_PERIODIC; connL[1]=ESMC_GRIDCONN_POLE; connU[1]=ESMC_GRIDCONN_POLE;}


  // detect if a given staggerloc is present in the Grid
  bool hasStaggerLoc(int staggerloc);

  // Set data in an empty grid before commit
  int set(
	  int _nameLen,                                // (in)
	  char *_name,                                 // (in)
	  ESMC_TypeKind *_typekind,                    // (in)
	  DistGrid *_distgrid,                    // (in)
	  InterfaceInt *gridEdgeLWidth,          // (in)
	  InterfaceInt *gridEdgeUWidth,          // (in)
	  InterfaceInt *gridAlign,          // (in)
	  InterfaceInt *_distgridToGridMap,                  // (in)
          InterfaceInt *_minIndex,          // (in)
          InterfaceInt *_maxIndex,          // (in)
          InterfaceInt *_localIndices,          // (in)
          int *localCount,          // (in)
	  InterfaceInt *_coordDimCount,              // (in)
	  InterfaceInt *_coordDimMap,             // (in)
	  ESMC_IndexFlag *_indexflag,                  // (in)
	  bool *destroyDistgrid,
	  bool *destroyDELayout
	  );

  // serialize Grid info into bytestream
  int serialize(
                char *buffer,   // inout - byte stream to fill
                int *length,    // inout - buf length
                int *offset);   // inout - original offset, updated to point 
                                //         to first free byte after current obj info

  int deserialize(
                  char *buffer,          // in - byte stream to read
                  int *offset);          // inout - original offset, updated to point 

  // create fully formed grid
 static Grid *create(int nameLen,                                // (in)
	       char *name,                                 // (in)
	       ESMC_TypeKind *typekind,                    // (in)
	       DistGrid *distgrid,                  // (in)
	       InterfaceInt *gridEdgeLWidth,          // (in)
	       InterfaceInt *gridEdgeUWidth,          // (in)
	       InterfaceInt *gridAlign,          // (in)
	       InterfaceInt *distgridToGridMap,                  // (in)
	       InterfaceInt *coordDimCount,              // (in)
	       InterfaceInt *coordDimMap,             // (in)
	       ESMC_IndexFlag *indexflag,                  // (in)
	       bool *destroyDistgrid,
	       bool *destroyDELayout,
	       int *rc                                     // (out) return code
	       );

 // create an arbitrarily distributed grid
 static Grid *create(int nameLen,                                // (in)
	       char *name,                                 // (in)
	       ESMC_TypeKind *typekind,                    // (in)
	       DistGrid *distgrid,                  // (in)
	       InterfaceInt *minIndex,              // (in)
	       InterfaceInt *maxIndex,              // (in)
	       InterfaceInt *localIndices,          // (in)
               int localCount,				  // (in)
	       InterfaceInt *distDimMap,                  // (in)
	       int arbDim, 	     
  	       InterfaceInt *coordDimCount,       // (in) optional
               InterfaceInt *coordDimMap,         // (in) optional
    	       bool *destroyDistgrid,
	       bool *destroyDELayout,
	       int *rc                               // (out) return code
	       );

 // create an empty grid for use with set/commit
  static Grid *create(int *rc);


  // make a create empty grid usable
  int commit();


  // deallocate a grid and all internal structures
  static int destroy(Grid **grid);

  // Grid Construct (grid NOT usable after construction)
  Grid();

  // Grid Destruct
  ~Grid();

  // get lower stagger offset for a particular localDe and staggerloc
  int getLDEStaggerUOffset(
		       int _staggerloc,
		       int _localDE, 
		       int *_UWidth // should be size>=grid dimCount
		       );

  // get upper stagger offset for a particular localDe and staggerloc
  int getLDEStaggerLOffset(
		     int _staggerloc,
		     int _localDE, 
		     int *_LWidth // should be size>=grid dimCount
		     );


int getDistExclusiveLBound(
                           int localDEArg, 
                           int *lBndArg    
                           );


int getDistExclusiveUBound(
                           int localDEArg, 
                           int *lBndArg    
                           );

int getExclusiveLBound(
                       int localDEArg, 
                       int *lBndArg    
                       );

int getExclusiveUBound(
                       int localDEArg, 
                       int *uBndArg    
                       );

int getComputationalLBound(
                           int staggerloc, 
                           int localDEArg, 
                           int *lBndArg    
                           );

int getComputationalUBound(
                           int staggerloc, 
                           int localDEArg, 
                           int *uBndArg    
                           );


 // Set Array for holding coordinates
// This needs to make sure the Coord has already been allocated
 int setCoordArray(
                   int *_staggerloc,
                   int *_coord,
                   Array *_array,
                   ESMC_DataCopy *_docopy
                   );

 int addCoordFromArrayList(
                       int *staggerlocArg,        // (in) optional
                       int arrayCount,             // (in) 
                       Array **arrayList,           // (in)
                       ESMC_DataCopy *docopyArg,   // (in) optional
                       InterfaceInt *_staggerEdgeLWidthArg,
                       InterfaceInt *_staggerEdgeUWidthArg,
                       InterfaceInt *_staggerAlign
                       );


 // Allocate coordinate Arrays for every coord in a staggerloc
 int addCoordArray(
                     int *_staggerloc,
                     InterfaceInt *_staggerEdgeLWidthArg,
                     InterfaceInt *_staggerEdgeUWidthArg,
                     InterfaceInt *_staggerAlign
                     );

 // Allocate coordinate Arrays for every coord in a staggerloc
 int addCoordArrayArb(
                     int *_staggerloc
                     );

 // Get the Array containing the coordinates
 Array *getCoordArray(
                      int *_staggerloc,
                      int _coord,
                      ESMC_DataCopy *_docopy,
                      int *rcArg
                      );


 // Get data from a specific coordinate location
 template <class TYPE> int getCoord(
                                     int staggerloc, // (in)
                                     int localDE,    // (in)
                                     int *index,     // (in)  needs to be of size Grid rank
                                     TYPE *coord     // (out) needs to be of size Grid rank
                                     );

 // Get data from a specific coordinate location without error checking 
 template <class TYPE> void getCoordInternal(
                                 int staggerloc, // (in)
                                 int localDE,    // (in)
                                 int *index,     // (in)  needs to be of size Grid rank
                                 TYPE *coord     // (out) needs to be of size Grid rank
                                 );
 
 // Convert the index of an arb grid point into the 1D index of the cooresponding distGrid
 int convertIndex(
		  int *indexArg
		  );

 // setup internal structures in _grid based on parameters
 friend int construct(
		      Grid *_grid, 
		      int _nameLen,
		      char *_name, 
		      ESMC_TypeKind *_typekind,
		      DistGrid *_distgrid,     
                      InterfaceInt *gridEdgeLWidthArg,
                      InterfaceInt *gridEdgeUWidthArg,
                      InterfaceInt *gridAlignArg,
		      InterfaceInt *_distgridToGridMap,   
		      InterfaceInt *_undistLBound,  
		      InterfaceInt *_undistUBound,  
		      InterfaceInt *_coordDimCount,
		      InterfaceInt *_coordDimMap,
		      ESMC_IndexFlag *_indexflag, 
		      bool *destroyDistgrid,
		      bool *destroyDELayout
		      );
  
 friend int construct(
		      Grid *_grid, 
		      int _nameLen,
		      char *_name, 
		      ESMC_TypeKind *_typekind,
		      DistGrid *_distgrid,     
		      InterfaceInt *minIndex,   
		      InterfaceInt *maxIndex,   
		      InterfaceInt *_localIndices,
		      int localCount,			
		      InterfaceInt *_distDim, 
		      int arbDim,
		      InterfaceInt *_undistLBound,  
		      InterfaceInt *_undistUBound,
		      InterfaceInt *_coordDimCount,
		      InterfaceInt *_coordDimMap,
		      bool *destroyDistgrid,
		      bool *destroyDELayout
		      );
  
};  // end class ESMC_Grid

  // set defaults for LWidth, UWidth, and Align based on user input
  int setGridDefaultsLUA(
                       int dimCount,                // Size of the input arrays
                       InterfaceInt *gridEdgeLWidthIn,  // (in) optional
                       InterfaceInt *gridEdgeUWidthIn,  // (in) optional
                       InterfaceInt *gridAlignIn,   // (in) optional
                       int *gridEdgeLWidthOut,          // (out)
                       int *gridEdgeUWidthOut,          // (out)
                       int *gridAlignOut            // (out)
                       );

  

  // class for iterating through the indices in a grid stagger location
  class GridIter {
  private:
    Grid *grid;     // grid being iterated through
    int staggerloc; // staggerloc in grid being iterated through

    const ESMC_GridConn *connL;
    const ESMC_GridConn *connU;
    
    int rank; // rank of grid and number of valid entires in the below
    int curInd[ESMF_MAXDIM];  // current position in index space

    int lBndInd[ESMF_MAXDIM]; // start position for allon this local DE  
    int uBndInd[ESMF_MAXDIM]; // end position on this local DE  
    int exLBndInd[ESMF_MAXDIM]; // start position on exlusive region on this DE

    int dimOff[ESMF_MAXDIM]; // Offset for each dimension for computing lid
    int lOff;                // lower bound offset
    
    int curDE; // current local DE
    int uBndDE; // end local DE on this PET (the lower bnd is 0)

    int numDE; // the number of DE's on this PET


    bool done; // are we done yet?

    bool cellNodes; // include all the nodes attached to cells on this PET's Grid

    // Temporary and will go away soon
    int minInd[ESMF_MAXDIM];  // minimum of patch
    int maxInd[ESMF_MAXDIM];  // maximum of patch


    void setDEBnds(int localDE);
    void getDEBnds(int localDE,int *uBnd,int *lBnd);

  public:

  bool isDone(void) const {return done;}
  GridIter(Grid *gridArg, int  staggerlocArg, bool cellNodesArg);
  ~GridIter();
  GridIter *toBeg();
  GridIter *adv();
  GridIter *moveToLocalID(int localID);
  int getGlobalID();
  int getLocalID();
  bool isLocal();
  bool isShared();
  int getCount();
  int getDE();
  int getPoleID();
  template <class TYPE> void getCoord(TYPE *coord);
  template <class TYPE> void getArrayData(Array *array, TYPE *data);
  }; 


  // class for iterating through the cells in a grid stagger location
  class GridCellIter {
  private:
    Grid *grid;     // grid being iterated through
    int staggerloc; // staggerloc in grid being iterated through

    const ESMC_GridConn *connL;
    const ESMC_GridConn *connU;
    
    int rank; // rank of grid and number of valid entires in the below
    int curInd[ESMF_MAXDIM];  // current position in index space

    int lBndInd[ESMF_MAXDIM]; // start position for allon this local DE  
    int uBndInd[ESMF_MAXDIM]; // end position on this local DE  
    int exLBndInd[ESMF_MAXDIM]; // start position on exlusive region on this DE

    int dimOff[ESMF_MAXDIM]; // Offset for each dimension for computing lid
    int lOff;                // lower bound offset
    
    int curDE; // current local DE
    int uBndDE; // end local DE on this PET (the lower bnd is 0)

    int numDE; // the number of DE's on this PET

    bool done; // are we done yet?

    void setDEBnds(int localDE);
    void getDEBnds(int localDE,int *uBnd,int *lBnd);

    // Temporary and will go away soon
    int minInd[ESMF_MAXDIM];  // minimum of patch
    int maxInd[ESMF_MAXDIM];  // maximum of patch

  public:

  bool isDone(void) const {return done;}
  GridCellIter(Grid *gridArg, int  staggerlocArg);
  ~GridCellIter();
  GridCellIter *toBeg();
  GridCellIter *adv();
  GridCellIter *moveToLocalID(int localID);
  int getGlobalID();
  int getLocalID();
  int getCount();
  int getDE();
  void getCornersCellNodeLocalID(int *cnrCount, int *cnrList);
  }; 


 // Class for holding data set after grid empty creation, but before grid is finally created.
class ProtoGrid { 
 public:
  int nameLen; 
  char *name;  
  ESMC_TypeKind *typekind;
  DistGrid *distgrid;     
  InterfaceInt *gridEdgeLWidth;
  InterfaceInt *gridEdgeUWidth;
  InterfaceInt *gridAlign;   
  InterfaceInt *distgridToGridMap;   
  InterfaceInt *undistLBound;  
  InterfaceInt *undistUBound;  
  InterfaceInt *coordDimCount;  
  InterfaceInt *coordDimMap; 
  ESMC_IndexFlag *indexflag; 
  InterfaceInt *minIndex;
  InterfaceInt *maxIndex;
  InterfaceInt *coordLocalIndices;
  int localCount;
  int arbDim;
  bool *destroyDistgrid;
  bool *destroyDELayout;

  // Proto Grid Construct
  ProtoGrid();

  // Proto Grid Destruct
  ~ProtoGrid();
         
}; // end class ProtoGrid

 
} // END ESMCI namespace

#endif  // ESMC_GridI_H


