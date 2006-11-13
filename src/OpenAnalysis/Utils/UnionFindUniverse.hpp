/*! \file
  
  \brief Declarations for UnionFind

 \authors This code was taken from the HPCVIEW system, which took it from the D system.
  \version $Id: UnionFindUniverse.hpp,v 1.2 2004/11/19 19:21:53 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef UnionFindUniverse_H
#define UnionFindUniverse_H

namespace OA {

class UnionFindElement;

class UnionFindUniverse {
public:
  UnionFindUniverse(unsigned int highWaterMark);
  ~UnionFindUniverse();
  void Union(int i, int j, int k);
  int Find(int i);

private: // methods
  int &Count(int i);
  int &Root(int i);
  int &Parent(int i);
  int &Name(int i);
  int do_FIND(int i);

private: // data
  UnionFindElement *e;
};
 
} // end of OA namespace

#endif

