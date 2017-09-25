#ifndef __RxBuf_CONFIG_H
#define __RxBuf_CONFIG_H

#ifndef RxBuf_CONFIG_REENTRANT
  #define RxBuf_CONFIG_REENTRANT        1 /* 1: reentrant implementation; 0: non-reentrant implementation */
#endif

#ifndef RxBuf_CONFIG_BUF_SIZE
  #define RxBuf_CONFIG_BUF_SIZE         64   /* number of elements in the buffer */
#endif

#ifndef RxBuf_CONFIG_ELEM_SIZE
  #define RxBuf_CONFIG_ELEM_SIZE        1   /* size of a single element in bytes */
#endif

#endif /* __RxBuf_CONFIG_H */
