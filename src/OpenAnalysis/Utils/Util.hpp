/*! \file
  
  \brief OA utilities

  \authors Nathan Tallent
  \version $Id: Util.hpp,v 1.1 2004/12/13 20:04:49 ntallent Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef OA_Util_H
#define OA_Util_H

// Could use templates within OA namespace

#ifndef OA_MIN
# define OA_MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef OA_MAX
# define OA_MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#endif
