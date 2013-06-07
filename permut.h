
#define $$nxchng_2(N,l,r) ( (((N>>l*4)&0xf)<<r*4)|(((N>>r*4)&0xf)<<l*4)|(N&~((0xf<<l*4)|(0xf<<r*4))) )
#define $$permut_2(X,N,a,b,Q) \
            X(N, (a) && (b) && Q); \
            X($$nxchng_2(N,0,1), (b) && (a) && Q )
#define $$permut_3(X,N,a,b,c,Q) \
            $$permut_2(X,N,a,b,(c)&&Q); \
            $$permut_2(X,$$nxchng_2(N,1,2),a,c,(b)&&Q); \
            $$permut_2(X,$$nxchng_2(N,0,2),b,c,(a)&&Q)
#define $$permut_4(X,N,a,b,c,d,Q) \
            $$permut_3(X,N,a,b,c,(d)&&Q); \
            $$permut_3(X,$$nxchng_2(N,2,3),a,b,d,(c)&&Q); \
            $$permut_3(X,$$nxchng_2(N,1,3),a,d,c,(b)&&Q); \
            $$permut_3(X,$$nxchng_2(N,0,3),d,b,c,(a)&&Q)
#define $$permut_5(X,N,a,b,c,d,e,Q) \
            $$permut_4(X,N,a,b,c,d,(e)&&Q); \
            $$permut_4(X,$$nxchng_2(N,3,4),a,b,c,e,(d)&&Q); \
            $$permut_4(X,$$nxchng_2(N,2,4),a,b,e,d,(c)&&Q); \
            $$permut_4(X,$$nxchng_2(N,1,4),a,e,c,d,(b)&&Q); \
            $$permut_4(X,$$nxchng_2(N,0,4),e,b,c,d,(a)&&Q)

#define Q(i) 1
#define X(n,Q) case n: printf("%08x: %d\n",n,Q); break
