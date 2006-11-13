typedef union {
    char*   sval;
    int     ival;
    bool    bval;
    OA::ProcHandle* procptr;
    OA::StmtHandle* stmtptr;
    OA::SymHandle* symptr;
    std::list<OA::SymHandle>* symlistptr;
    OA::ExprHandle* exprptr;
    std::list<OA::ExprHandle>* exprlistptr;
    OA::MemRefHandle* memrefptr;
    OA::OpHandle* opptr;
    OA::ConstSymHandle* constsymptr;
    OA::ConstValHandle* constvalptr;

    OA::Location::Location* locptr;

    OA::ExprTree::ExprTree* etreeptr;
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
#define	T_CHILDREN	269
#define	T_MEMREFNODE	270
#define	T_FULLOVERLAP	271
#define	T_PARTOVERLAP	272
#define	T_LOCAL	273
#define	T_FULL	274
#define	T_NAMEDLOC	275
#define	T_UNNAMEDLOC	276
#define	T_UNKNOWNLOC	277
#define	T_INVISIBLELOC	278
#define	T_LOCBASICSUBSET	279
#define	T_LOCIDXSUBSET	280
#define	T_MAPSTO	281
#define	T_SYMS	282
#define	T_PARAMS	283
#define	T_CALLSITES	284
#define	T_CALLSITEPARAMS	285
#define	T_CALLSITEPARAMINFO	286
#define	T_FORMAL	287
#define	T_EXPRTREE	288
#define	T_REFPARAM	289


extern YYSTYPE callgraphdfproblemlval;
