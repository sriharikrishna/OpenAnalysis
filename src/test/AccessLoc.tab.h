typedef union {
    char*   sval;
    int     ival;
    bool    bval;
    OA::ProcHandle* procptr;
    OA::StmtHandle* stmtptr;
    std::list<OA::StmtHandle>* stmtlistptr;
    OA::SymHandle* symptr;
    std::list<OA::SymHandle>* symlistptr;
    OA::ExprHandle* exprptr;
    OA::MemRefHandle* memrefptr;
    OA::OpHandle* opptr;
    OA::ConstSymHandle* constsymptr;
    OA::ConstValHandle* constvalptr;

    OA::MemRefExpr::MemRefType mrtval;
    OA::MemRefExpr::MemRefExpr* mreptr;
    std::list<OA::MemRefExpr::MemRefExpr*>* mrelistptr;

    OA::Location::Location* locptr;
} YYSTYPE;
#define	T_STR_LITERAL	257
#define	T_INT_LITERAL	258
#define	T_TRUE	259
#define	T_FALSE	260
#define	T_PROCHANDLE	261
#define	T_STMTHANDLE	262
#define	T_SYMHANDLE	263
#define	T_MEMREFHANDLE	264
#define	T_EXPRHANDLE	265
#define	T_OPHANDLE	266
#define	T_CONSTSYMHANDLE	267
#define	T_CONSTVALHANDLE	268
#define	T_USE	269
#define	T_DEF	270
#define	T_NAMEDREF	271
#define	T_UNNAMEDREF	272
#define	T_UNKNOWNREF	273
#define	T_DEREF	274
#define	T_IDXACCESS	275
#define	T_ADDRESSOF	276
#define	T_FULLACCURACY	277
#define	T_MEMREFTYPE	278
#define	T_NUMDEREF	279
#define	T_IDX	280
#define	T_FULLOVERLAP	281
#define	T_PARTOVERLAP	282
#define	T_LOCAL	283
#define	T_FULL	284
#define	T_NAMEDLOC	285
#define	T_UNNAMEDLOC	286
#define	T_UNKNOWNLOC	287
#define	T_INVISIBLELOC	288
#define	T_LOCBASICSUBSET	289
#define	T_LOCIDXSUBSET	290
#define	T_MAPSTO	291
#define	T_FORMALPARAMS	292
#define	T_ACCESSIBLESYMS	293
#define	T_UNNAMEDDYNALLOCSTMTS	294
#define	T_POSSMEMREFEXPRS	295


extern YYSTYPE accessloclval;
