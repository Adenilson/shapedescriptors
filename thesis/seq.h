
//ARQUIVO cxtypes.h
/*********************************** Sequence *******************************************/

typedef struct CvSeqBlock
{
    struct CvSeqBlock*  prev; /* previous sequence block */
    struct CvSeqBlock*  next; /* next sequence block */
    int    start_index;       /* index of the first element in the block +
                                 sequence->first->start_index */
    int    count;             /* number of elements in the block */
    char*  data;              /* pointer to the first element of the block */
}
CvSeqBlock;


#define CV_TREE_NODE_FIELDS(node_type)                          \
    int       flags;         /* micsellaneous flags */          \
    int       header_size;   /* size of sequence header */      \
    struct    node_type* h_prev; /* previous sequence */        \
    struct    node_type* h_next; /* next sequence */            \
    struct    node_type* v_prev; /* 2nd previous sequence */    \
    struct    node_type* v_next  /* 2nd next sequence */

/*
   Read/Write sequence.
   Elements can be dynamically inserted to or deleted from the sequence.
*/
#define CV_SEQUENCE_FIELDS()                                            \
    CV_TREE_NODE_FIELDS(CvSeq);                                         \
    int       total;          /* total number of elements */            \
    int       elem_size;      /* size of sequence element in bytes */   \
    char*     block_max;      /* maximal bound of the last block */     \
    char*     ptr;            /* current write pointer */               \
    int       delta_elems;    /* how many elements allocated when the seq grows */  \
    CvMemStorage* storage;    /* where the seq is stored */             \
    CvSeqBlock* free_blocks;  /* free blocks list */                    \
    CvSeqBlock* first; /* pointer to the first sequence block */

typedef struct CvSeq
{
    CV_SEQUENCE_FIELDS()
}
CvSeq;

#define CV_TYPE_NAME_SEQ             "opencv-sequence"
#define CV_TYPE_NAME_SEQ_TREE        "opencv-sequence-tree"


/****************************************************************************************/
/*                            Sequence writer & reader                                  */
/****************************************************************************************/

#define CV_SEQ_WRITER_FIELDS()                                     \
    int          header_size;                                      \
    CvSeq*       seq;        /* the sequence written */            \
    CvSeqBlock*  block;      /* current block */                   \
    char*        ptr;        /* pointer to free space */           \
    char*        block_min;  /* pointer to the beginning of block*/\
    char*        block_max;  /* pointer to the end of block */

typedef struct CvSeqWriter
{
    CV_SEQ_WRITER_FIELDS()
    int  reserved[4]; /* some reserved fields */
}
CvSeqWriter;


#define CV_SEQ_READER_FIELDS()                                      \
    int          header_size;                                       \
    CvSeq*       seq;        /* sequence, beign read */             \
    CvSeqBlock*  block;      /* current block */                    \
    char*        ptr;        /* pointer to element be read next */  \
    char*        block_min;  /* pointer to the beginning of block */\
    char*        block_max;  /* pointer to the end of block */      \
    int          delta_index;/* = seq->first->start_index   */      \
    char*        prev_elem;  /* pointer to previous element */


typedef struct CvSeqReader
{
    CV_SEQ_READER_FIELDS()
    int  reserved[4];
}
CvSeqReader;

/****************************************************************************************/
/*                                Operations on sequences                               */
/****************************************************************************************/

#define  CV_SEQ_ELEM( seq, elem_type, index )                    \
/* assert gives some guarantee that <seq> parameter is valid */  \
(   assert(sizeof((seq)->first[0]) == sizeof(CvSeqBlock) &&      \
    (seq)->elem_size == sizeof(elem_type)),                      \
    (elem_type*)((seq)->first && (unsigned)index <               \
    (unsigned)((seq)->first->count) ?                            \
    (seq)->first->data + (index) * sizeof(elem_type) :           \
    cvGetSeqElem( (CvSeq*)(seq), (index) )))
#define CV_GET_SEQ_ELEM( elem_type, seq, index ) CV_SEQ_ELEM( (seq), elem_type, (index) )

/* macro that adds element to sequence */
#define CV_WRITE_SEQ_ELEM_VAR( elem_ptr, writer )     \
{                                                     \
    if( (writer).ptr >= (writer).block_max )          \
    {                                                 \
        cvCreateSeqBlock( &writer);                   \
    }                                                 \
    memcpy((writer).ptr, elem_ptr, (writer).seq->elem_size);\
    (writer).ptr += (writer).seq->elem_size;          \
}

#define CV_WRITE_SEQ_ELEM( elem, writer )             \
{                                                     \
    assert( (writer).seq->elem_size == sizeof(elem)); \
    if( (writer).ptr >= (writer).block_max )          \
    {                                                 \
        cvCreateSeqBlock( &writer);                   \
    }                                                 \
    assert( (writer).ptr <= (writer).block_max - sizeof(elem));\
    memcpy((writer).ptr, &(elem), sizeof(elem));      \
    (writer).ptr += sizeof(elem);                     \
}


/* move reader position forward */
#define CV_NEXT_SEQ_ELEM( elem_size, reader )                 \
{                                                             \
    if( ((reader).ptr += (elem_size)) >= (reader).block_max ) \
    {                                                         \
        cvChangeSeqBlock( &(reader), 1 );                     \
    }                                                         \
}


/* move reader position backward */
#define CV_PREV_SEQ_ELEM( elem_size, reader )                \
{                                                            \
    if( ((reader).ptr -= (elem_size)) < (reader).block_min ) \
    {                                                        \
        cvChangeSeqBlock( &(reader), -1 );                   \
    }                                                        \
}

/* read element and move read position forward */
#define CV_READ_SEQ_ELEM( elem, reader )                       \
{                                                              \
    assert( (reader).seq->elem_size == sizeof(elem));          \
    memcpy( &(elem), (reader).ptr, sizeof((elem)));            \
    CV_NEXT_SEQ_ELEM( sizeof(elem), reader )                   \
}

/* read element and move read position backward */
#define CV_REV_READ_SEQ_ELEM( elem, reader )                     \
{                                                                \
    assert( (reader).seq->elem_size == sizeof(elem));            \
    memcpy(&(elem), (reader).ptr, sizeof((elem)));               \
    CV_PREV_SEQ_ELEM( sizeof(elem), reader )                     \
}


#define CV_READ_CHAIN_POINT( _pt, reader )                              \
{                                                                       \
    (_pt) = (reader).pt;                                                \
    if( (reader).ptr )                                                  \
    {                                                                   \
        CV_READ_SEQ_ELEM( (reader).code, (*((CvSeqReader*)&(reader)))); \
        assert( ((reader).code & ~7) == 0 );                            \
        (reader).pt.x += (reader).deltas[(int)(reader).code][0];        \
        (reader).pt.y += (reader).deltas[(int)(reader).code][1];        \
    }                                                                   \
}

#define CV_CURRENT_POINT( reader )  (*((CvPoint*)((reader).ptr)))
#define CV_PREV_POINT( reader )     (*((CvPoint*)((reader).prev_elem)))

#define CV_READ_EDGE( pt1, pt2, reader )               \
{                                                      \
    assert( sizeof(pt1) == sizeof(CvPoint) &&          \
            sizeof(pt2) == sizeof(CvPoint) &&          \
            reader.seq->elem_size == sizeof(CvPoint)); \
    (pt1) = CV_PREV_POINT( reader );                   \
    (pt2) = CV_CURRENT_POINT( reader );                \
    (reader).prev_elem = (reader).ptr;                 \
    CV_NEXT_SEQ_ELEM( sizeof(CvPoint), (reader));      \
}


//ARQUIVO cxcore.h

/* Creates new empty sequence that will reside in the specified storage */
CVAPI(CvSeq*)  cvCreateSeq( int seq_flags, int header_size,
                            int elem_size, CvMemStorage* storage );

/* Changes default size (granularity) of sequence blocks.
   The default size is ~1Kbyte */
CVAPI(void)  cvSetSeqBlockSize( CvSeq* seq, int delta_elems );


/* Adds new element to the end of sequence. Returns pointer to the element */
CVAPI(char*)  cvSeqPush( CvSeq* seq, void* element CV_DEFAULT(NULL));


/* Adds new element to the beginning of sequence. Returns pointer to it */
CVAPI(char*)  cvSeqPushFront( CvSeq* seq, void* element CV_DEFAULT(NULL));


/* Removes the last element from sequence and optionally saves it */
CVAPI(void)  cvSeqPop( CvSeq* seq, void* element CV_DEFAULT(NULL));


/* Removes the first element from sequence and optioanally saves it */
CVAPI(void)  cvSeqPopFront( CvSeq* seq, void* element CV_DEFAULT(NULL));


#define CV_FRONT 1
#define CV_BACK 0
/* Adds several new elements to the end of sequence */
CVAPI(void)  cvSeqPushMulti( CvSeq* seq, void* elements,
                             int count, int in_front CV_DEFAULT(0) );

/* Removes several elements from the end of sequence and optionally saves them */
CVAPI(void)  cvSeqPopMulti( CvSeq* seq, void* elements,
                            int count, int in_front CV_DEFAULT(0) );

/* Inserts a new element in the middle of sequence.
   cvSeqInsert(seq,0,elem) == cvSeqPushFront(seq,elem) */
CVAPI(char*)  cvSeqInsert( CvSeq* seq, int before_index,
                           void* element CV_DEFAULT(NULL));

/* Removes specified sequence element */
CVAPI(void)  cvSeqRemove( CvSeq* seq, int index );


/* Removes all the elements from the sequence. The freed memory
   can be reused later only by the same sequence unless cvClearMemStorage
   or cvRestoreMemStoragePos is called */
CVAPI(void)  cvClearSeq( CvSeq* seq );


/* Retrives pointer to specified sequence element.
   Negative indices are supported and mean counting from the end
   (e.g -1 means the last sequence element) */
CVAPI(char*)  cvGetSeqElem( const CvSeq* seq, int index );

/* Calculates index of the specified sequence element.
   Returns -1 if element does not belong to the sequence */
CVAPI(int)  cvSeqElemIdx( const CvSeq* seq, const void* element,
                         CvSeqBlock** block CV_DEFAULT(NULL) );

/* Initializes sequence writer. The new elements will be added to the end of sequence */
CVAPI(void)  cvStartAppendToSeq( CvSeq* seq, CvSeqWriter* writer );


/* Combination of cvCreateSeq and cvStartAppendToSeq */
CVAPI(void)  cvStartWriteSeq( int seq_flags, int header_size,
                              int elem_size, CvMemStorage* storage,
                              CvSeqWriter* writer );

/* Closes sequence writer, updates sequence header and returns pointer
   to the resultant sequence
   (which may be useful if the sequence was created using cvStartWriteSeq))
*/
CVAPI(CvSeq*)  cvEndWriteSeq( CvSeqWriter* writer );


/* Updates sequence header. May be useful to get access to some of previously
   written elements via cvGetSeqElem or sequence reader */
CVAPI(void)   cvFlushSeqWriter( CvSeqWriter* writer );


/* Initializes sequence reader.
   The sequence can be read in forward or backward direction */
CVAPI(void) cvStartReadSeq( const CvSeq* seq, CvSeqReader* reader,
                           int reverse CV_DEFAULT(0) );


/* Returns current sequence reader position (currently observed sequence element) */
CVAPI(int)  cvGetSeqReaderPos( CvSeqReader* reader );


/* Changes sequence reader position. It may seek to an absolute or
   to relative to the current position */
CVAPI(void)   cvSetSeqReaderPos( CvSeqReader* reader, int index,
                                 int is_relative CV_DEFAULT(0));

/* Copies sequence content to a continuous piece of memory */
CVAPI(void*)  cvCvtSeqToArray( const CvSeq* seq, void* elements,
                               CvSlice slice CV_DEFAULT(CV_WHOLE_SEQ) );

/* Creates sequence header for array.
   After that all the operations on sequences that do not alter the content
   can be applied to the resultant sequence */
CVAPI(CvSeq*) cvMakeSeqHeaderForArray( int seq_type, int header_size,
                                       int elem_size, void* elements, int total,
                                       CvSeq* seq, CvSeqBlock* block );

/* Extracts sequence slice (with or without copying sequence elements) */
CVAPI(CvSeq*) cvSeqSlice( const CvSeq* seq, CvSlice slice,
                         CvMemStorage* storage CV_DEFAULT(NULL),
                         int copy_data CV_DEFAULT(0));

CV_INLINE CvSeq* cvCloneSeq( const CvSeq* seq, CvMemStorage* storage CV_DEFAULT(NULL));
CV_INLINE CvSeq* cvCloneSeq( const CvSeq* seq, CvMemStorage* storage )
{
    return cvSeqSlice( seq, CV_WHOLE_SEQ, storage, 1 );
}

/* Removes sequence slice */
CVAPI(void)  cvSeqRemoveSlice( CvSeq* seq, CvSlice slice );

/* Inserts a sequence or array into another sequence */
CVAPI(void)  cvSeqInsertSlice( CvSeq* seq, int before_index, const CvArr* from_arr );


/* Sorts sequence in-place given element comparison function */
CVAPI(void) cvSeqSort( CvSeq* seq, CvCmpFunc func, void* userdata CV_DEFAULT(NULL) );

/* Finds element in a [sorted] sequence */
CVAPI(char*) cvSeqSearch( CvSeq* seq, const void* elem, CvCmpFunc func,
                          int is_sorted, int* elem_idx,
                          void* userdata CV_DEFAULT(NULL) );

/* Reverses order of sequence elements in-place */
CVAPI(void) cvSeqInvert( CvSeq* seq );

/* Splits sequence into one or more equivalence classes using the specified criteria */
CVAPI(int)  cvSeqPartition( const CvSeq* seq, CvMemStorage* storage,
                            CvSeq** labels, CvCmpFunc is_equal, void* userdata );

/************ Internal sequence functions ************/
CVAPI(void)  cvChangeSeqBlock( CvSeqReader* reader, int direction );
CVAPI(void)  cvCreateSeqBlock( CvSeqWriter* writer );
